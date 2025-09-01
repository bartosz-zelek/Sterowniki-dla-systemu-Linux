/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "recorder.h"
#include <simics/util/os.h>
#include <simics/base/log.h>
#include <simics/simulator-iface/checkpoint.h>
#include <simics/simulator-iface/consoles.h>
#include <simics/simulator-iface/recorder.h>
#include "input.h"
#include "output.h"
#include "gfx-console.h"
#include "gfx-inline.h"

struct gfx_record_data {
        gfx_console_t *gc;            /* Iffy shortcut to containing object. */

        // Associated recorder object.
        conf_object_t *recorder;
        const recorder_v2_interface_t *rec_int;

        /* Name of a thumbnail picture file being saved in a checkpoint,
           or NULL outside checkpoint saving. */
        char *thumbnail_file;
};

// The recorder saves data packets, so must store type of data.
typedef enum {
        Kbd_Event,
        Mouse_Event,
} gfx_rec_type_t;

// Recorder data for keyboard events.
typedef struct {
        uint8 type;
        uint8 down;
        uint16 pad;
        uint32 code;
} gfx_kbd_rec_data_t;

// Recorder data for mouse events.
typedef struct {
        uint8 type;
        uint8 pad[3];
        uint32 x;
        uint32 y;
        uint32 z;
        uint32 buttons;
} gfx_mouse_rec_data_t;

static gfx_record_data_t *
record_data(conf_object_t *obj)
{
        return from_obj(obj)->record_data;
}

static conf_object_t *
gr_obj(gfx_record_data_t *gr) { return to_obj(gr->gc); }

// Implementation of recorded interface.
// Callback used by recorder object, when it has passed through it.
static void
recorded_receive_data(conf_object_t *obj, bytes_t data, bool playback)
{
        gfx_record_data_t *gr = record_data(obj);
        gfx_rec_type_t type = data.data[0];
        switch (type) {
        case Kbd_Event: {
                gfx_kbd_rec_data_t event = *(gfx_kbd_rec_data_t *)data.data;
                gfx_kbd_input(gr->gc->input_data,
                              event.code, event.down);
                break;
        }
        case Mouse_Event: {
                gfx_mouse_rec_data_t event = *(gfx_mouse_rec_data_t *)data.data;
                gfx_mouse_input(gr->gc->input_data,
                                event.x, event.y, event.z, event.buttons);
                break;
        }
        }
}

// Record console keyboard input by passing it to the associated recorder.
void
gfx_record_kbd_input(gfx_record_data_t *gr, sim_key_t code, bool down)
{
        gfx_kbd_rec_data_t data = {
                .type = Kbd_Event,
                .code = code, .down = down
        };
        gr->rec_int->record(gr->recorder, gr_obj(gr),
                            (bytes_t){(uint8 *)&data, sizeof data});
}

// Record console mouse input by passing it to the associated recorder.
void
gfx_record_mouse_input(gfx_record_data_t *gr, int x, int y, int z,
                       gfx_console_mouse_button_t buttons)
{
        gfx_mouse_rec_data_t data = {
                .type = Mouse_Event,
                .x = x, .y = y, .z = z, .buttons = buttons
        };
        gr->rec_int->record(gr->recorder, gr_obj(gr),
                            (bytes_t){(uint8 *)&data, sizeof data});
}

// Implementation of con_input_code interface.
static void
con_input_code_input(conf_object_t *obj, sim_key_t code, bool down)
{
        gfx_record_data_t *gr = record_data(obj);
        gfx_record_kbd_input(gr, code, down);
}

// Implementation of checkpoint interface.
static void
checkpoint_save(conf_object_t *obj, const char *path, save_flags_t flags)
{
        gfx_record_data_t *gr = record_data(obj);

        strbuf_t filename = SB_INIT;
        if (!path[0] || os_isdir(path)) {
                // Bundle checkpoint.
                sb_set(&filename, path);
                strbuf_t s = sb_newf("%s.png", SIM_object_name(obj));
                os_path_join(&filename, sb_str(&s));
                sb_free(&s);
        } else {
                // Old-style checkpoint.
                sb_fmt(&filename, "%s.%s.png", path, SIM_object_name(obj));
        }
        gr->thumbnail_file = sb_detach(&filename);
        gfx_save_png(gr->gc->output_data, gr->thumbnail_file,
                     MIN(192, gfx_get_width(gr->gc->output_data)),
                     MIN(144, gfx_get_height(gr->gc->output_data)));
}

// Implementation of checkpoint interface.
static void
checkpoint_finish(conf_object_t *obj, int success)
{
        gfx_record_data_t *gr = record_data(obj);
        if (!success) {
                if (os_remove(gr->thumbnail_file) != 0) {
                        char *err = os_describe_last_error();
                        SIM_LOG_ERROR(obj, Gfx_Console_Log_Record,
                                      "Error removing %s: %s",
                                      gr->thumbnail_file, err);
                        MM_FREE(err);
                }
        }
        MM_FREE(gr->thumbnail_file);
        gr->thumbnail_file = NULL;
}

static set_error_t
set_recorder(conf_object_t *obj, attr_value_t *val)
{
        gfx_record_data_t *gr = record_data(obj);
        conf_object_t *rec = SIM_attr_object(*val);
        const recorder_v2_interface_t *iface = SIM_C_GET_INTERFACE(
                rec, recorder_v2);
        if (!iface)
                return Sim_Set_Interface_Not_Found;

        gr->recorder = rec;
        gr->rec_int = iface;
        return Sim_Set_Ok;
}

static attr_value_t
get_recorder(conf_object_t *obj)
{
        return SIM_make_attr_object(record_data(obj)->recorder);
}

gfx_record_data_t *
make_gfx_recording(gfx_console_t *gc)
{
        gfx_record_data_t *gr = MM_ZALLOC(1, gfx_record_data_t);
        gr->gc = gc;
        return gr;
}

void
destroy_gfx_recording(gfx_record_data_t *gr)
{
        MM_FREE(gr);
}

void
register_gfx_recording(conf_class_t *cl)
{
        static const recorded_interface_t recorded_iface = {
                .input = recorded_receive_data
        };
        SIM_REGISTER_INTERFACE(cl, recorded, &recorded_iface);

        static const con_input_code_interface_t con_input_code_iface = {
                .input = con_input_code_input,
        };
        SIM_REGISTER_INTERFACE(cl, con_input_code, &con_input_code_iface);

        static const checkpoint_interface_t cp_if = {
                .save_v2  = checkpoint_save,
                .finish = checkpoint_finish
        };
        SIM_REGISTER_INTERFACE(cl, checkpoint, &cp_if);

        SIM_register_attribute(
                cl, "recorder",
                get_recorder,
                set_recorder,
                Sim_Attr_Required,
                "o",
                "Recorder object");
}
