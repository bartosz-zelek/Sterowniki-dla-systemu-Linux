/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <simics/simulator-api.h>
#include <simics/util/os.h>

#define SHOW_OBSOLETE_API
#include <simics/model-iface/temporal-state.h>
#undef SHOW_OBSOLETE_API

#include <simics/simulator-iface/recorder.h>

typedef struct rec_event rec_event_t;

struct rec_event {
        conf_object_t *clock;           /* clock that the time applies to */
        cycles_t cycles;                /* time when the event occurs */
        bytes_t data;                   /* event data, allocated and owned */
        uint32 udata;                   /* scalar data (old interface only) */

        conf_object_t *sender;          /* who sent the data (new interface) */
};

typedef struct {
        conf_object_t    obj;

        FILE            *infile;               /* non-NULL while replaying */
        FILE            *outfile;
        char            *infile_name;
        char            *outfile_name;

        bool            input_during_playback;
        bool            stop_at_end_of_recording;

        rec_event_t     *file_ev;              /* next file playback event */ 
        conf_object_t   *last_event_clock;     /* last playback event's clock */

        int             warned;
} rec_obj_t;

struct recorder {
        rec_obj_t       *rec_obj;
        conf_object_t   *client_obj;
        void            (*input_from_recorder)(conf_object_t *,
                                               dbuffer_t *, uint32);
};

static event_class_t *ev_file_playback;
static event_class_t *ev_end_of_recording;

static inline rec_obj_t *
from_obj(conf_object_t *obj)
{
        return (rec_obj_t *)obj;
}

static inline conf_object_t *
to_obj(rec_obj_t *rec)
{
        return &rec->obj;
}

static void
write_event(FILE *fp, conf_object_t *clock, cycles_t cycles,
            bytes_t data, uint32 udata, conf_object_t *sender)
{
        static const char hex[] = "0123456789abcdef";

        uint32 hi = (uint32)(cycles >> 32);
        uint32 lo = (uint32)cycles;

        fprintf(fp, "%s %s %u %u %x %d ",
                SIM_object_name(sender), SIM_object_name(clock), hi, lo,
                udata, (int)data.len);

        for (int i = 0; i < data.len; i++) {
                fputc(hex[data.data[i] >> 4], fp);
                fputc(hex[data.data[i] & 0xf], fp);
        }
        fputc('\n', fp);
}

static rec_event_t *
new_rec_event(conf_object_t *clock, cycles_t cycles,
              bytes_t data, uint32 udata,
              conf_object_t *sender)
{
        ASSERT(sender != NULL);
        ASSERT(udata == 0);
        rec_event_t *ev = MM_ZALLOC(1, rec_event_t);
        ev->clock = clock;
        ev->cycles = cycles;
        ev->data = data;
        ev->udata = udata;
        ev->sender = sender;
        return ev;
}

static int
hex_to_nybble(int c)
{
        return (c >= '0' && c <= '9') ? c - '0'
               : (c >= 'a' && c <= 'f') ? c - 'a' + 10
               : (c >= 'A' && c <= 'F') ? c - 'A' + 10
               : -1;
}

/* Read an event line from file 'f' and return true if successful. On success,
   the caller is responsible of freeing the strings (s1, s2, s3) with
   MM_FREE. */
static bool
read_event_data(FILE *f, char **s1, char **s2, unsigned *hi, unsigned *lo,
                uint32 *udata, unsigned *size, char **s3)
{
        strbuf_t line = SB_INIT;
        // coverity[overrun-buffer-val]
        if (!sb_readline(&line, f))
                return false;

        int max_obj_name = sb_len(&line);
        char *s1_tmp = MM_MALLOC(max_obj_name, char);
        char *s2_tmp = MM_MALLOC(max_obj_name, char);
        char *s3_tmp = MM_MALLOC(max_obj_name, char);
        unsigned hi_tmp;
        unsigned lo_tmp;
        uint32 udata_tmp;
        unsigned size_tmp;
        int num_items = sscanf(
                sb_str(&line), "%s %s %u %u %x %d %s", s1_tmp, s2_tmp, &hi_tmp,
                &lo_tmp, &udata_tmp, &size_tmp, s3_tmp);
        if (num_items < 6)
                goto exit_error;
        if (num_items == 6)
                *s3_tmp = 0; /* if missing, just let s3 be empty. */

        *s1 = s1_tmp;
        *s2 = s2_tmp;
        *s3 = s3_tmp;
        *hi = hi_tmp;
        *lo = lo_tmp;
        *udata = udata_tmp;
        *size = size_tmp;
        return true;

  exit_error:
        MM_FREE(s1_tmp);
        MM_FREE(s2_tmp);
        MM_FREE(s3_tmp);
        return false;
}

static rec_event_t *
read_event(rec_obj_t *rec, int *done)
{
        *done = 0;

        char *sender_name = NULL;
        char *clock_name = NULL;
        unsigned hi;
        unsigned lo;
        uint32 udata;
        unsigned size;
        char *bytes_str = NULL;
        if (!read_event_data(rec->infile, &sender_name, &clock_name, &hi, &lo,
                             &udata, &size, &bytes_str)) {
                *done = 1;
                return NULL;
        }

        conf_object_t *sender = SIM_get_object(sender_name);
        MM_FREE(sender_name);
        if (!sender) {
                (void)SIM_clear_exception();
                MM_FREE(clock_name);
                MM_FREE(bytes_str);
                goto error;
        }
        conf_object_t *clock = SIM_get_object(clock_name);
        MM_FREE(clock_name);
        if (!clock) {
                (void)SIM_clear_exception();
                MM_FREE(bytes_str);
                goto error;
        }

        bytes_t bytes = {NULL, 0};
        if (size) {
                uint8 *data = MM_MALLOC(size, uint8);
                char *s = bytes_str;
                for (int i = 0; i < size; i++) {
                        int nh = hex_to_nybble(s[0]);
                        int nl = hex_to_nybble(s[1]);
                        s += 2;
                        if (nh < 0 || nl < 0) {
                                MM_FREE(bytes_str);
                                MM_FREE(data);
                                goto error;
                        }
                        data[i] = nh << 4 | nl;
                }
                bytes.data = data;
                bytes.len = size;
        }
        MM_FREE(bytes_str);
        return new_rec_event(clock, (cycles_t)hi << 32 | lo, bytes, udata,
                             sender);

error:
        if (!rec->warned++)
                SIM_LOG_ERROR(&rec->obj, 0,
                              "playback data skipped due to an error");
        for (;;) {
                int ch = fgetc(rec->infile);
                if (ch == '\n' || ch == EOF)
                        break;
        }
        return NULL;
}

static void
stop_file_playback(rec_obj_t *rec)
{
        rec_event_t *ev = rec->file_ev;
        if (ev) {
                SIM_event_cancel_time(ev->clock,
                                      ev_file_playback,
                                      &rec->obj,
                                      0,
                                      rec);

                rec->file_ev = NULL;
        }

        if (rec->infile) {
                fclose(rec->infile);
        }
        rec->infile = NULL;
}

static void
free_event(rec_event_t *e)
{
        MM_FREE((uint8 *)e->data.data);
        MM_FREE(e);
}

static void
send_input_to_client(conf_object_t *rec_obj, conf_object_t *sender,
                     bytes_t data, bool playback)
{
        /* TODO: optimise by looking up the interface key only once */
        const recorded_interface_t *ri = SIM_C_GET_INTERFACE(
                sender, recorded);
        if (unlikely(!ri)) {
                SIM_LOG_ERROR(rec_obj, 0,
                              "%s does not implement the recorded interface",
                              SIM_object_name(sender));
                return;
        }
        ri->input(sender, data, playback);
}

static void
fire_event(rec_obj_t *rec, rec_event_t *e) 
{
        SIM_LOG_INFO(3, to_obj(rec), 0, "firing event");
        if (e->sender != to_obj(rec))
                send_input_to_client(to_obj(rec), e->sender, e->data, true);

        if (rec->outfile) {
                /* add to output file if recording active */
                write_event(rec->outfile, e->clock, e->cycles,
                            e->data, e->udata, e->sender);
        }

        rec->last_event_clock = e->clock;

        free_event(e);
}

static bool
is_old_event(rec_event_t *e)
{
        cycles_t cnt = SIM_cycle_count(e->clock);
        return (e->cycles < cnt);
}

static bool
is_current_event(conf_object_t *queue, rec_event_t *e)
{
        return e->clock == queue && e->cycles == SIM_cycle_count(e->clock);
}

static void
post_file_event(rec_obj_t *rec, rec_event_t *e)
{
        cycles_t cnt = SIM_cycle_count(e->clock);

        cnt = e->cycles - cnt;
        SIM_LOG_INFO(3, to_obj(rec), 0, "posting file playback on %s at %lld",
                     SIM_object_name(e->clock),
                     (long long)cnt);
        SIM_event_post_cycle(e->clock,
                             ev_file_playback,
                             to_obj(rec),
                             cnt,
                             NULL);
}

static void
at_end_of_recording(rec_obj_t *rec)
{
        if (rec->stop_at_end_of_recording)
                VT_stop_message(to_obj(rec), "End of recording");
        else {
                SIM_LOG_INFO(1, to_obj(rec), 0, "End of recording");
        }
}

static void
end_of_recording_event(conf_object_t *obj, void *data)
{
        at_end_of_recording(from_obj(obj));
}

static void
post_end_of_recording(rec_obj_t *rec)
{
        if (rec->last_event_clock) {
                SIM_event_post_cycle(rec->last_event_clock,
                                     ev_end_of_recording,
                                     to_obj(rec),
                                     0,
                                     NULL);
                rec->last_event_clock = NULL;
        } else {
                at_end_of_recording(rec);
        }

}

/* Make sure that we know what the next recorded event from the file is (and
   store it in rec->file_ev). If EOF is reached, the history file is closed and
   rec->file_ev is set to NULL. */
static void
setup_next_file_event(rec_obj_t *rec)
{
        while (!rec->file_ev) {
                int done = 0;
                rec->file_ev = read_event(rec, &done);
                if (done) {
                        stop_file_playback(rec);
                        post_end_of_recording(rec);
                        return;
                }
                if (rec->file_ev && is_old_event(rec->file_ev)) {
                        free_event(rec->file_ev);
                        rec->file_ev = NULL;
                }
        }
}

/* when playback has started we wait with finding input until the continuation
   service. This way the user can start playback before creating the devices
   that are used for the playback */
static void
do_start_playback(void *obj, conf_object_t *hap_obj)
{
        rec_obj_t *rec = from_obj(obj);
        SIM_LOG_INFO(3, to_obj(rec), 0, "starting playback");

        SIM_hap_delete_callback("Core_Continuation",
                                (obj_hap_func_t)do_start_playback, rec);

        /* there is no last event at the start of playback */
        rec->last_event_clock = NULL;

        if (rec->infile) {
                setup_next_file_event(rec);
                if (rec->file_ev)
                        post_file_event(rec, rec->file_ev);
        }
}

static void
replay_file_event(conf_object_t *obj, void *data)
{
        rec_obj_t *rec = from_obj(obj);
        rec_event_t *e = rec->file_ev;
        conf_object_t *queue = e->clock;
        rec->file_ev = NULL;
        fire_event(rec, e);
        setup_next_file_event(rec);
        while ((e = rec->file_ev)) {
                if (is_current_event(queue, e)) {
                        fire_event(rec, e);
                        rec->file_ev = NULL;
                } else {
                        post_file_event(rec, e);
                        break;
                }
                setup_next_file_event(rec);
        }
}

static void
restore_temporal_state(conf_object_t *obj, lang_void *state)
{
        rec_obj_t *rec = from_obj(obj);

        /* are we replaying from file? if so, then repost the next event */
        if (rec->file_ev) {
                ASSERT(!is_old_event(rec->file_ev));
                post_file_event(rec, rec->file_ev);
        }
}

static bool
playback(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return rec->infile != NULL;
}

static void
record_event(rec_obj_t *rec, bytes_t data, uint32 udata, conf_object_t *sender)
{
        ASSERT(sender != NULL);

        conf_object_t *clock = SIM_current_clock();
        if (!clock) {
                clock = SIM_object_clock(sender);
                if (!clock)
                        clock = SIM_object_clock(to_obj(rec));
                ASSERT(clock);
        }
        if (clock) {
                /* Use the pico-seconds clock since this improves
                   replay timing. */
                conf_object_t *ps_clk = SIM_picosecond_clock(clock);
                if (ps_clk)
                        clock = ps_clk;
        }
        cycles_t now = SIM_cycle_count(clock);

        write_event(rec->outfile, clock, now, data, udata, sender);
}

static bool
should_skip_input(rec_obj_t *rec)
{
        return rec->infile && !rec->input_during_playback;
}

static bool
currently_recording(rec_obj_t *rec)
{
        return rec->outfile;
}

static void
record(conf_object_t *NOTNULL obj, conf_object_t *NOTNULL sender, bytes_t data)
{
        rec_obj_t *rec = from_obj(obj);

        if (should_skip_input(rec))
                return;

        if (currently_recording(rec))
                record_event(rec, data, 0, sender);

        send_input_to_client(obj, sender, data, false);
}

/* object handling */

static set_error_t
set_infile(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);

        if (rec->infile) {
                SIM_attribute_error("Input file already opened");
                return Sim_Set_Illegal_Value;
	}

        MM_FREE(rec->infile_name);

        rec->infile_name =
            SIM_attr_is_string(*val) ? MM_STRDUP(SIM_attr_string(*val)) : NULL;
        return Sim_Set_Ok;
}

static attr_value_t
get_infile(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);

        return SIM_make_attr_string(rec->infile_name);
}

static set_error_t
set_outfile(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);

        if (rec->outfile) {
                SIM_attribute_error("Output file already opened");
                return Sim_Set_Illegal_Value;
	}

        MM_FREE(rec->outfile_name);
        rec->outfile_name =
            SIM_attr_is_string(*val) ? MM_STRDUP(SIM_attr_string(*val)) : NULL;
        return Sim_Set_Ok;
}

static attr_value_t
get_outfile(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);

        return SIM_make_attr_string(rec->outfile_name);
}

static void
write_endmark(rec_obj_t *rec)
{
        if (!SIM_object_clock(to_obj(rec)))
                return;

        static const uint8 val[] = { 0, 0, 0, 0 };
        bytes_t data = { val, 4 };
        record_event(rec, data, 0, to_obj(rec));
}

static void
finalize_recording(rec_obj_t *rec)
{
        if (rec->outfile) {
                write_endmark(rec);
                fclose(rec->outfile);
        }
        rec->outfile = NULL;
}

static void
at_exit(lang_void *obj, conf_object_t *trigger)
{
        finalize_recording(from_obj(obj));
}

static set_error_t
set_recording(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);

        /* stop recording */
        if (!SIM_attr_boolean(*val)) {
                finalize_recording(rec);
                return Sim_Set_Ok;
        }

        /* start recording */
        if (rec->outfile) {
                SIM_attribute_error("Recording already started");
                return Sim_Set_Illegal_Value;
        }

        if (rec->outfile_name == NULL) {
                SIM_attribute_error("No output file set");
                return Sim_Set_Illegal_Value;
        }

        if (!(rec->outfile = os_fopen(rec->outfile_name, "w"))) {
                SIM_attribute_error("Failed opening output file");
                return Sim_Set_Illegal_Value;
        }
        return Sim_Set_Ok;
}

static set_error_t
set_playback(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);

        /* stop playback */
        if (!SIM_attr_boolean(*val)) {
                if (rec->infile)
                        stop_file_playback(rec);
                return Sim_Set_Ok;
        }

        /* start playback */
        if (rec->infile) {
                SIM_attribute_error("Playback already started");
                return Sim_Set_Illegal_Value;
        }

        if (rec->infile_name == NULL) {
                SIM_attribute_error("No input file set");
                return Sim_Set_Illegal_Value;
        }
                
        if (!(rec->infile = os_fopen(rec->infile_name, "r"))) {
                SIM_attribute_error("Failed opening input file");
                return Sim_Set_Illegal_Value;
        }
        SIM_hap_add_callback("Core_Continuation",
                             (obj_hap_func_t)do_start_playback, rec);
        return Sim_Set_Ok;
}

static set_error_t
set_event(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);

        /* schedule next event */
        do_start_playback(rec, NULL);

        return Sim_Set_Ok;
}

static attr_value_t
get_recording(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return SIM_make_attr_boolean(rec->outfile);
}

static attr_value_t
get_playback(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return SIM_make_attr_boolean(rec->infile);
}

static attr_value_t
get_event(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return SIM_make_attr_boolean(rec->file_ev);
}

static set_error_t
set_input_during_playback(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);
        rec->input_during_playback = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_input_during_playback(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return SIM_make_attr_boolean(rec->input_during_playback);
}

static conf_object_t *
alloc(conf_class_t *cl)
{
        rec_obj_t *rec = MM_ZALLOC(1, rec_obj_t);
        return to_obj(rec);
}

static void *
init(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        rec->stop_at_end_of_recording = true;
        SIM_hap_add_callback("Core_At_Exit", (obj_hap_func_t)at_exit, obj);
        return obj;
}

static void
deinit(conf_object_t *obj)
{
        SIM_hap_delete_callback("Core_At_Exit", (obj_hap_func_t)at_exit, obj);
}

static void
dealloc(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);

        if (rec->infile)
                os_fclose(rec->infile);
        if (rec->outfile)
                os_fclose(rec->outfile);

	MM_FREE(rec->infile_name);
	MM_FREE(rec->outfile_name);
        MM_FREE(obj);
}

static attr_value_t
get_stop_at_end(conf_object_t *obj)
{
        rec_obj_t *rec = from_obj(obj);
        return SIM_make_attr_boolean(rec->stop_at_end_of_recording);
}

static set_error_t
set_stop_at_end(conf_object_t *obj, attr_value_t *val)
{
        rec_obj_t *rec = from_obj(obj);
        rec->stop_at_end_of_recording = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

void
init_local()
{
        conf_class_t *class = SIM_create_class("recorder", &(class_info_t) {
                .alloc = alloc,
                .init = init,
                .deinit = deinit,
                .dealloc = dealloc,
                .short_desc = "recorder object",
                .description =
                "The <class>recorder</class> class is used to record and "
                "playback I/O events. Recordable events include network "
                "traffic and mouse and keyboard events."
        });
              
        SIM_register_attribute(class, "in_file",
                               get_infile,
                               set_infile,
                               Sim_Attr_Optional, "s|n",
                               "Name of input file, if any.");

        SIM_register_attribute(class, "out_file",
                               get_outfile,
                               set_outfile,
                               Sim_Attr_Optional, "s|n",
                               "Name of output file, if any.");

        SIM_register_attribute(class, "input_during_playback",
                               get_input_during_playback,
                               set_input_during_playback,
                               Sim_Attr_Pseudo, "b",
                               "If true, input is accepted when "
                               "playback is active.");

        SIM_register_attribute(class, "recording",
                               get_recording,
                               set_recording,
                               Sim_Attr_Pseudo, "b",
                               "If true, recording is active.");

        SIM_register_attribute(class, "playback",
                               get_playback,
                               set_playback,
                               Sim_Attr_Pseudo, "b",
                               "If true, playback is active.");

        SIM_register_attribute(class, "event",
                               get_event,
                               set_event,
                               Sim_Attr_Pseudo, "b",
                               "If true, playback event is active.");

        SIM_register_attribute(class, "stop_at_end_of_recording",
                               get_stop_at_end,
                               set_stop_at_end,
                               Sim_Attr_Pseudo, "b",
                               "Determines if the simulation will be"
                               " stopped when the end of the recording"
                               " is reached. Default value is true.");

        static const recorder_v2_interface_t rec_v2 = {
                .record = record,
                .playback = playback
        };
        SIM_REGISTER_INTERFACE(class, recorder_v2, &rec_v2);

        static const temporal_state_interface_t ts_iface = {
                .finish_restore = restore_temporal_state,
        };
        SIM_REGISTER_INTERFACE(class, temporal_state, &ts_iface);

        ev_file_playback =
                SIM_register_event("file playback",
                                   class,
                                   Sim_EC_Notsaved,
                                   replay_file_event,
                                   NULL, NULL, NULL, NULL);

        ev_end_of_recording = 
                SIM_register_event("end of recording",
                                   class,
                                   Sim_EC_Notsaved | Sim_EC_Slot_Late,
                                   end_of_recording_event,
                                   NULL, NULL, NULL, NULL);
}
