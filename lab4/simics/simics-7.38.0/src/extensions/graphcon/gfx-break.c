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

#include "gfx-break.h"
#include <simics/util/hashtab.h>
#include <simics/util/os.h>
#include <simics/base/conf-object.h>
#include <simics/base/configuration.h>
#include <simics/base/log.h>
#include <simics/base/hap-producer.h>
#include <simics/simulator/control.h>
#include <simics/simulator/callbacks.h>
#include <simics/model-iface/video-interface.h>
#include <simics/model-iface/gfx-console.h>
#include <simics/model-iface/vga-text-interface.h>
#include <simics/simulator-iface/consoles.h>

#include <pthread.h>

#include "gfx-console.h"
#include "output.h"
#include "break-strings.h"
#include "gfx-inline.h"

#define MAX_PATCH_WIDTH 4000
#define MAX_PATCH_HEIGHT 3000

static event_class_t *break_event;    /* Breakpoint event class. */
static event_class_t *virtual_refresh_event;

// Breakpoint callback data.
typedef struct {
        gfx_break_cb_t handler;
        lang_void *arg;
} break_handler_t;

// Graphical breakpoint data.
typedef struct {
        int64 hap_id;                      /* Break ID used in hap callback. */

        // Screen image patch position.
        uint32 minx;
        uint32 miny;
        uint32 maxx;
        uint32 maxy;

        uint32 *data;   /* Image patch data, size by the coordinates above. */

        bool oneshot;       /* Should breakpoint be deactivated after match? */
        bool active;        /* Is image patch being checked for matches? */
        double interval;    /* Milliseconds of CPU time between checks. */

        char *name;     /* Breakpoint name, used for log and break messages. */

        break_handler_t handler;             /* Callback for match. */
} gfx_break_t;

struct gfx_break_data {
        gfx_console_t *gc;           /* Awful shortcut to containing object. */

        // Associated VGA device.
        conf_object_t *device;
        const vga_update_interface_t *refresh_int;
        const vga_text_info_interface_t *text_int;

        VECT(gfx_break_t) breaks;            /* Graphical breakpoints. */

        int64 next_hap_id;        /* Hap id used for next added breakpoint. */

        gfx_console_screen_text_t text_data;   /* VGA text mode data. */

        // text_data is read by the Winsome GUI thread and updated by
        // Simics main thread (in thread-safe callbacks and real-time events)
        pthread_mutex_t text_mutex;
        
        int refresh_rate;              /* Console update frequency in Hz. */
        int64 refresh_event_id;        /* Console real-time update event ID. */

        bool refresh_in_virtual_time;
};

// We use a separate hap type for graphical breakpoints.
static hap_type_t hap_gfx_break;

static gfx_break_data_t *
break_data(conf_object_t *obj)
{
        return from_obj(obj)->gfx_break_data;
}

static conf_object_t *
gb_obj(gfx_break_data_t *gb) { return to_obj(gb->gc); }

static gfx_output_data_t *
gb_go(gfx_break_data_t *gb) { return gb->gc->output_data; }

// Free all breakpoint data.
static void
clear_break_data(gfx_break_data_t *gb)
{
        VFORT(gb->breaks, gfx_break_t, entry) {
                MM_FREE(entry.name);
                MM_FREE(entry.data);
        }
        VFREE(gb->breaks);
}

// Update internal VGA text mode data from VGA device
void
gfx_update_screen_text(gfx_break_data_t *gb)
{
        // We may not have a device pointer, e.g. if the VGA device does not
        // implement the required interfaces, like framebuffer.
        if (!gb->device || !SIM_object_is_configured(gb->device))
                return;
        
        bool text_mode = gb->text_int->text_mode(gb->device);
        frontend_set_text_mode(gb_go(gb), text_mode);
        if (text_mode) {
                // Query VGA device using the vga_text_info interface.
                int rows;
                int cols;
                gb->text_int->screen_size(gb->device, &cols, &rows);
                gb->text_int->font_size(
                        gb->device, &gb->text_data.font_width,
                        &gb->text_data.font_height);

                if (rows != gb->text_data.rows
                    || cols != gb->text_data.columns) {
                        pthread_mutex_lock(&gb->text_mutex);
                        gb->text_data.text.len = rows * cols;
                        gb->text_data.text.data = MM_REALLOC(
                                (uint8 *)gb->text_data.text.data,
                                gb->text_data.text.len, uint8);
                        gb->text_data.line_lengths.len = rows;
                        gb->text_data.line_lengths.data = MM_REALLOC(
                                (uint8 *)gb->text_data.line_lengths.data,
                                gb->text_data.line_lengths.len, uint8);
                        gb->text_data.rows = rows;
                        gb->text_data.columns = cols;
                        pthread_mutex_unlock(&gb->text_mutex);
                }

                gb->text_int->text(gb->device,
                                   (uint8 *)gb->text_data.text.data,
                                   (uint8 *)gb->text_data.line_lengths.data);
        }
}

gfx_console_screen_text_t
gfx_get_text_data(gfx_break_data_t *gb)
{
        pthread_mutex_lock(&gb->text_mutex);
        gfx_console_screen_text_t data = gb->text_data;
        pthread_mutex_unlock(&gb->text_mutex);
        return data;
}

// Forward declaration for breaking cyclic dependency.
static void request_vga_refresh_dirty(void *arg);

// Make the VGA device do a full screen refresh
// (even if it thinks screen has not changed)
// Will result in gfx_console interface calls, in particular a call to refresh
// at the end, i.e. to refresh_cb in output.c
void
request_vga_refresh_all(gfx_break_data_t *gb)
{
        if (gb->device)
                gb->refresh_int->refresh_all(gb->device);
}

static int
is_virtual_refresh_event(lang_void *data, lang_void *match_data)
{
        return data == match_data;
}

// Is the console currently being updated from the VGA device using events?
bool
gfx_refresh_enabled(gfx_break_data_t *gb)
{
        return (gb->refresh_event_id != 0
                || (gb->refresh_in_virtual_time
                    && SIM_event_find_next_cycle(SIM_picosecond_clock(gb_obj(gb)),
                                                virtual_refresh_event,
                                                gb_obj(gb),
                                                is_virtual_refresh_event,
                                                gb) != -1));
}

// Enqueue a real-time console update event using the current refresh rate.
static void
enqueue_refresh_event(gfx_break_data_t *gb)
{
        if (!gb->refresh_in_virtual_time) {
                int update_delay_ms = 1000 / gb->refresh_rate;
                if (update_delay_ms == 0)
                        update_delay_ms = 1;

                gb->refresh_event_id =
                        SIM_realtime_event(
                                update_delay_ms, request_vga_refresh_dirty,
                                gb, 0, "GFX Console refresh");
        } else {
                ASSERT_FMT(SIM_picosecond_clock(gb_obj(gb)),
                           "Console object %s requires an associated clock.",
                           SIM_object_name(gb_obj(gb)));
                SIM_event_post_time(SIM_picosecond_clock(gb_obj(gb)),
                                    virtual_refresh_event,
                                    gb_obj(gb),
                                    1.0 / gb->refresh_rate,
                                    gb);
        }
}

// Turn on console refresh from the VGA device.
void
enable_gfx_device_refresh(gfx_break_data_t *gb)
{
        // Should not enqueue more events if already enabled
        // otherwise not idempotent operation.
        if (!gfx_refresh_enabled(gb) && gb->refresh_rate > 0)
                enqueue_refresh_event(gb);
}

// Turn off console refresh from the VGA device.
void
disable_gfx_device_refresh(gfx_break_data_t *gb, bool realtime)
{
        if (gfx_refresh_enabled(gb)) {
                if (!gb->refresh_in_virtual_time) {
                        int ret = SIM_cancel_realtime_event(gb->refresh_event_id);
                        ASSERT(ret == 0);
                        gb->refresh_event_id = 0;
                } else if (!realtime && SIM_picosecond_clock(gb_obj(gb))) {
                        SIM_event_cancel_time(SIM_picosecond_clock(gb_obj(gb)),
                                              virtual_refresh_event,
                                              gb_obj(gb),
                                              is_virtual_refresh_event,
                                              gb);
                }
        }
}

// Make the VGA device update the console screen if necessary.
// May result in gfx_console interface calls, at least always a call to refresh
// at the end, i.e. to refresh_cb in output.c
static void
refresh_screen_data(gfx_break_data_t *gb)
{
        if (gb->device)
                gb->refresh_int->refresh(gb->device);
}

// Callback for real-time console update event.
// Make VGA device update relevant parts of screen via gfx_console interface
// and enqueue next event.
void
request_vga_refresh_dirty(void *arg)
{
        gfx_break_data_t *gb = arg;
        refresh_screen_data(gb);
        enqueue_refresh_event(gb);
}

static void
vga_refresh_virtual(conf_object_t *obj, void *arg)
{
        request_vga_refresh_dirty(arg);
}

// Used when changing console update refresh rate.
static void
reset_vga_refresh(gfx_break_data_t *gb)
{
        // Cancel any pending events with previous refresh rate
        disable_gfx_device_refresh(gb, false);

        // Enqueue new refresh event
        enable_gfx_device_refresh(gb);
}

// Implementation of vga_text_update interface.
static void
vga_text_update_write(conf_object_t *obj, char value)
{
        gfx_break_data_t *gb = break_data(obj);

        // The main purpose of vga_text_update is to define a stream of
        // characters on which we can implement break strings.
        // Even though this stream may be ill-defined, it is better than
        // nothing, and preserves compatibility with old xterm based
        // "vga text" graphics console.
        console_check_break_strings(gb->gc->break_str_data, value);

        // Output to log file, if opened.
        gfx_log_text(obj, value);

        // Output to Simics command line and log
        gfx_cmd_line_output(obj, value);

        // Signal activity to the frontend.
        frontend_signal_text_update(gb->gc->output_data);
}

// Test given graphical breakpoint for match on the current screen.
static bool
check_breakpoint(gfx_break_data_t *gb, const uint32 *data,
                 uint32 minx, uint32 miny, uint32 maxx, uint32 maxy)
{
        if (maxx < gfx_get_width(gb->gc->output_data)
            && maxy < gfx_get_height(gb->gc->output_data)) {
                // TODO Only check update part of screen?
                return screen_contains_image_patch(
                        gb->gc->output_data, data, minx, miny, maxx, maxy);
        } else
                return false;
}

// Test given graphical breakpoint for match on the current screen.
static bool
check_breakpoint_entry(gfx_break_data_t *gb, const gfx_break_t *entry)
{
        return check_breakpoint(gb, entry->data,
                                entry->minx, entry->miny,
                                entry->maxx, entry->maxy);
}

// Callback for break event, which is posted once per breakpoint.
// Test the graphical breakpoint specified by the parameter for match.
static void
check_gfx_break(conf_object_t *obj, lang_void *data)
{
        gfx_break_data_t *gb = break_data(obj);
                
        int64 bp_id = (int64)data;
        if (bp_id >= 0 && bp_id < VLEN(gb->breaks)) {
                gfx_break_t entry = VGET(gb->breaks, bp_id);
                if (entry.active) {
                        // Take the latest data from the VGA device.
                        refresh_screen_data(gb);
                        if (check_breakpoint_entry(gb, &entry)) {
                                if (entry.handler.handler)
                                        entry.handler.handler(
                                                obj, entry.hap_id,
                                                entry.handler.arg);
                                int num_cb = SIM_c_hap_occurred_always(
                                        hap_gfx_break, obj,
                                        entry.hap_id, entry.hap_id);
                
                                if (!entry.handler.handler && num_cb == 0) {
                                        strbuf_t buf = sb_newf(
                                                "Break in %s on"
                                                " graphical breakpoint %s",
                                                SIM_object_name(obj),
                                                entry.name);
                                        SIM_LOG_INFO(
                                                1, obj, Gfx_Console_Log_Break,
                                                "%s", sb_str(&buf));
                                        SIM_break_simulation(sb_str(&buf));
                                        sb_free(&buf);
                                }

                                if (entry.oneshot) {
                                        entry.active = false;
                                        VSET(gb->breaks, entry.hap_id, entry);
                                }
                        }
                        // Enqueue next breakpoint-specific test event.
                        SIM_event_post_time(
                                SIM_picosecond_clock(obj), break_event,
                                obj, entry.interval, (lang_void *)entry.hap_id);
                }
        }        
}

/* Number of pixels in a rectangular region. The coordinates are inclusive. */
static int
break_patch_size(int minx, int miny, int maxx, int maxy)
{
        int width = maxx - minx + 1;
        int height = maxy - miny + 1;
        return width * height;
}

// Register new graphical breakpoint with given data.
static int64
add_breakpoint(gfx_break_data_t *gb, const char *name, const uint32 *data,
               int minx, int miny, int maxx, int maxy,
               double interval, bool oneshot,
               gfx_break_cb_t cb, lang_void *arg)
{
        int size = break_patch_size(minx, miny, maxx, maxy);
        uint32 *d = MM_MALLOC(size, uint32);
        memcpy(d, data, size * sizeof *d);
        gfx_break_t entry = {
                .data = d, .hap_id = gb->next_hap_id++,
                .active = true, .oneshot = oneshot,
                .interval = interval, .name = MM_STRDUP(name),
                .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy,
                .handler = {.handler = cb, .arg = arg}};
        VADD(gb->breaks, entry);

        // We must post one test event per breakpoint, since intervals differ.
        SIM_event_post_time(
                SIM_picosecond_clock(gb_obj(gb)), break_event,
                gb_obj(gb), entry.interval, (lang_void *)entry.hap_id);
        return entry.hap_id;
}

static bool
load_gfx_breakpoint(conf_object_t *NOTNULL obj, const char *file,
                    gbp_header_t *header, uint32 **data)
{
        FILE *brk_file = os_fopen(file, "rb");
        if (brk_file == NULL) {
                char *err = os_describe_last_error();
                SIM_LOG_INFO(
                        1, obj, Gfx_Console_Log_Break,
                        "Error opening break file '%s' for input: %s",
                        file, err);
                MM_FREE(err);
                return false;
        }

        int ret = fread(header, sizeof *header, 1, brk_file);
        if (ret != 1) {
                if (feof(brk_file)) {
                        SIM_LOG_INFO(
                                1, obj, Gfx_Console_Log_Break,
                                "EOF encountered while reading header from"
                                " break file '%s'", file);
                } else {
                        char *err = os_describe_last_error();
                        SIM_LOG_INFO(
                                1, obj, Gfx_Console_Log_Break,
                                "Error reading header from break file '%s': %s",
                                file, err);
                        MM_FREE(err);
                }
                os_fclose(brk_file);
                return false;
        }
        
        if (!(header->magic == GBP_MAGIC && header->format == GBP_FMT_V3_32)) {
                SIM_LOG_INFO(
                        1, obj, Gfx_Console_Log_Break,
                        "Illegal data in break file '%s'", file);
                os_fclose(brk_file);
                return false;
        }

        if (!(header->maxx >= header->minx && header->maxy >= header->miny)) {
                SIM_LOG_INFO(1, obj, Gfx_Console_Log_Break,
                             "Invalid graphical breakpoint rectangle"
                             " (%d, %d, %d, %d)", header->minx, header->miny,
                             header->maxx, header->maxy);
                os_fclose(brk_file);
                return false;
        }

        if (header->maxx > MAX_PATCH_WIDTH || header->maxy > MAX_PATCH_HEIGHT) {
                SIM_LOG_INFO(1, obj, Gfx_Console_Log_Break,
                             "Graphical breakpoint rectangle too big"
                             " (%d, %d, %d, %d)", header->minx, header->miny,
                             header->maxx, header->maxy);
                os_fclose(brk_file);
                return false;
        }

        uint32 width = header->maxx - header->minx + 1;
        uint32 height = header->maxy - header->miny + 1;

        *data = MM_MALLOC(width * height, uint32);
        uint64 bytes = width * height * sizeof **data;
        ret = fread(*data, bytes, 1, brk_file);
        if (ret != 1) {
                if (feof(brk_file)) {
                        SIM_LOG_INFO(
                                1, obj, Gfx_Console_Log_Break,
                                "EOF encountered while reading data from"
                                " break file '%s'", file);
                } else {
                        char *err = os_describe_last_error();
                        SIM_LOG_INFO(
                                1, obj, Gfx_Console_Log_Break,
                                "Error reading data from break file '%s': %s",
                                file, err);
                        MM_FREE(err);
                }
                MM_FREE(*data);
                os_fclose(brk_file);
                return false;
        }
        os_fclose(brk_file);
        return true;
}

// Implementation of gfx_break interface.
static bool
gfx_break_remove(conf_object_t *obj, int64 bp_id)
{
        gfx_break_data_t *gb = break_data(obj);
        if (bp_id >= 0 && bp_id < VLEN(gb->breaks)) {
                // To avoid problems with memory, we just deactivate it.
                gfx_break_t entry = VGET(gb->breaks, bp_id);
                entry.active = false;
                VSET(gb->breaks, bp_id, entry);
                return true;
        } else
                return false;
}

// Implementation of gfx_break interface.
static bool
gfx_break_store(conf_object_t *obj, const char *file, 
                int minx, int miny, int maxx, int maxy)
{
        gfx_break_data_t *gb = break_data(obj);
        if (!(maxx >= minx && maxy >= miny
              && minx >= 0 && miny >= 0
              && maxx < gfx_get_width(gb->gc->output_data)
              && maxy < gfx_get_height(gb->gc->output_data))) {
                SIM_LOG_INFO(1, obj, Gfx_Console_Log_Break,
                             "Invalid graphical breakpoint rectangle."
                             " Must be contained in (left, top, right, bottom)"
                             " = (%d, %d, %d, %d)", 0, 0,
                             gfx_get_width(gb->gc->output_data) - 1,
                             gfx_get_height(gb->gc->output_data) - 1);
                return false;
        }
                
        uint32 width = (uint32)maxx - (uint32)minx + 1;
        uint32 height = (uint32)maxy - (uint32)miny + 1;

        FILE *brk_file = os_fopen(file, "wb");
        if (brk_file == NULL) {
                char *err = os_describe_last_error();
                SIM_LOG_INFO(
                        1, obj, Gfx_Console_Log_Break,
                        "Error opening break file '%s' for output: %s",
                        file, err);
                MM_FREE(err);
                return false;
        }

        uint64 bytes = width * height * sizeof(uint32);
        gbp_header_t header = {
                .minx = minx, .maxx = maxx, .miny = miny, .maxy = maxy,
                .bytes = bytes, .magic = GBP_MAGIC, .format = GBP_FMT_V3_32};
        
        int ret = fwrite(&header, sizeof header, 1, brk_file);
        if (ret != 1) {
                char *err = os_describe_last_error();
                SIM_LOG_INFO(
                        1, obj, Gfx_Console_Log_Break,
                        "Error writing header to break file '%s': %s",
                        file, err);
                MM_FREE(err);
                os_fclose(brk_file);
                return false;
        }

        // Store current screen contents.
        refresh_screen_data(gb);
        uint32 *data = MM_MALLOC(width * height, uint32);
        screen_copy_image_patch(gb->gc->output_data,
                                data, minx, miny, maxx, maxy);

        ret = fwrite(data, bytes, 1, brk_file);
        if (ret != 1) {
                char *err = os_describe_last_error();
                SIM_LOG_INFO(
                        1, obj, Gfx_Console_Log_Break,
                        "Error writing break file data to '%s': %s",
                        file, err);
                MM_FREE(err);
                MM_FREE(data);
                os_fclose(brk_file);
                return false;
        }
        os_fclose(brk_file);

        MM_FREE(data);

        SIM_LOG_INFO(2, obj, Gfx_Console_Log_Break,
                     "Added graphical breakpoint from rectangle"
                     "  (left, top, right, bottom) = (%d, %d, %d, %d)",
                     minx, miny, maxx, maxy);
        return true;
}

// Implementation of gfx_break interface.
static int64
gfx_break_add(conf_object_t *NOTNULL obj, const char *file,
              const char *name, bool oneshot, double interval, 
              gfx_break_cb_t cb, lang_void *arg)
{
        gfx_break_data_t *gb = break_data(obj);

        // Load breakpoint data
        uint32 *data;
        gbp_header_t header;
        if (!load_gfx_breakpoint(obj, file, &header, &data))
                return -1;
        
        ASSERT(header.maxx <= MAX_PATCH_WIDTH
               && header.maxy <= MAX_PATCH_HEIGHT
               && header.minx <= header.maxx
               && header.miny <= header.maxy);

        // Name defaults to filename
        ASSERT(name != NULL);
        strbuf_t bp_name = (strlen(name) > 0)
                ? sb_newf("'%s'", name) : sb_newf("%s", file);

        // Register breakpoint
        int64 id = add_breakpoint(
                gb, sb_str(&bp_name), data,
                header.minx, header.miny, header.maxx, header.maxy,
                interval, oneshot, cb, arg);
        sb_free(&bp_name);
        MM_FREE(data);

        // Must turn on refresh from device for gfx breakpoints to work
        enable_gfx_device_refresh(gb);
        return id;
}

// Implementation of gfx_break interface.
static int64
gfx_break_add_bytes(conf_object_t *NOTNULL obj, bytes_t bp,
                    const char *name, bool oneshot, double interval,
                    gfx_break_cb_t cb, lang_void *arg)
{
        gfx_break_data_t *gb = break_data(obj);
        if (bp.len < sizeof(gbp_header_t)) {
                SIM_LOG_ERROR(
                        obj, Gfx_Console_Log_Break,
                        "Breakpoint data size of %llu does not even fit header"
                        " of %llu bytes",
                        (uint64)bp.len, (uint64)sizeof(gbp_header_t));
                return -1;
        }
        gbp_header_t header = *(gbp_header_t *)bp.data;
        if (bp.len != sizeof header + header.bytes) {
                SIM_LOG_ERROR(
                        obj, Gfx_Console_Log_Break,
                        "Breakpoint data must be %llu, not %llu, bytes",
                        sizeof header + header.bytes, (uint64)bp.len);
                return -1;
        }

        // Register breakpoint
        int64 id = add_breakpoint(
                gb, name, (uint32 *)(bp.data + sizeof header),
                header.minx, header.miny, header.maxx, header.maxy,
                interval, oneshot, cb, arg);

        // Must turn on refresh from device for gfx breakpoints to work
        enable_gfx_device_refresh(gb);
        return id;
}

// Implementation of gfx_break interface.
static int
gfx_break_match(conf_object_t *obj, const char *file)
{
        gfx_break_data_t *gb = break_data(obj);

        // Load breakpoint data
        uint32 *data;
        gbp_header_t header;
        if (!load_gfx_breakpoint(obj, file, &header, &data))
                return -1;

        ASSERT(header.maxx <= MAX_PATCH_WIDTH
               && header.maxy <= MAX_PATCH_HEIGHT
               && header.minx <= header.maxx
               && header.miny <= header.maxy);
       bool match = check_breakpoint(
                gb, data, header.minx, header.miny, header.maxx, header.maxy);
        MM_FREE(data);
        return match ? 1 : 0;
}

// Implementation of gfx_break interface.
static gbp_header_t
gfx_break_info(conf_object_t *obj, const char *file)
{
        uint32 *data;
        gbp_header_t header;
        if (!load_gfx_breakpoint(obj, file, &header, &data))
                header = (gbp_header_t){ 0 };
        else {
                ASSERT(header.maxx <= MAX_PATCH_WIDTH
                       && header.maxy <= MAX_PATCH_HEIGHT
                       && header.minx <= header.maxx
                       && header.miny <= header.maxy);
                MM_FREE(data);
        }
        return header;
}

// Implementation of gfx_break interface.
static bool
gfx_break_export_png(conf_object_t *obj, const char *file, const char *png_file)
{
        gfx_break_data_t *gb = break_data(obj);
        uint32 *data;
        gbp_header_t header;
        if (!load_gfx_breakpoint(obj, file, &header, &data))
                return false;

        ASSERT(header.maxx <= MAX_PATCH_WIDTH
               && header.maxy <= MAX_PATCH_HEIGHT
               && header.minx <= header.maxx
               && header.miny <= header.maxy);

        // TODO We're ignoring signed/unsigned conversion
        int width = header.maxx - header.minx + 1;
        int height = header.maxy - header.miny + 1;
        
        bool ok = gfx_save_png_data(gb_go(gb), png_file, data,
                                    width, height, width, height);
        MM_FREE(data);
        return ok;
}

static set_error_t
set_device(conf_object_t *obj, attr_value_t *val)
{
        gfx_break_data_t *gb = break_data(obj);
        conf_object_t *dev = SIM_attr_object_or_nil(*val);
        const vga_update_interface_t *refresh_iface = NULL;
        const vga_text_info_interface_t *text_iface = NULL;
        if (dev) {
                refresh_iface = SIM_C_GET_INTERFACE(dev, vga_update);
                if (!refresh_iface)
                        return Sim_Set_Interface_Not_Found;
                text_iface = SIM_C_GET_INTERFACE(dev, vga_text_info);
                if (!text_iface)
                        return Sim_Set_Interface_Not_Found;
        } else
                disable_gfx_device_refresh(gb, false);
        
        gb->device = dev;
        gb->refresh_int = refresh_iface;
        gb->text_int = text_iface;
        if (gb->device && SIM_object_is_configured(gb->device)
            && !SIM_is_restoring_state(obj))
                request_vga_refresh_all(gb);
        return Sim_Set_Ok;
}

static attr_value_t
get_device(conf_object_t *obj)
{
        return SIM_make_attr_object(break_data(obj)->device);
}

static set_error_t
set_refresh_rate(conf_object_t *obj, attr_value_t *val)
{
        gfx_break_data_t *gb = break_data(obj);
        int refresh_rate = SIM_attr_integer(*val);
        if (refresh_rate > 0) {
                gb->refresh_rate = refresh_rate;
                if (gb->refresh_event_id != 0)
                        reset_vga_refresh(gb);
                return Sim_Set_Ok;
        } else if (refresh_rate == 0) {
                gb->refresh_rate = refresh_rate;
                disable_gfx_device_refresh(gb, false);
                return Sim_Set_Ok;
        } else {
                SIM_attribute_error("Must specify positive refresh rate"
                                    " or 0 to disable refresh.");
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_refresh_rate(conf_object_t *obj)
{
        gfx_break_data_t *gb = break_data(obj);
        return SIM_make_attr_uint64(gb->refresh_rate);
}

static attr_value_t
get_gfx_breaks(conf_object_t *obj)
{
        gfx_break_data_t *gb = break_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(gb->breaks));
        VFORI(gb->breaks, i) {
                gfx_break_t entry = VGET(gb->breaks, i);
                attr_value_t list = SIM_make_attr_list(
                        10,
                        SIM_make_attr_int64(entry.hap_id),
                        SIM_make_attr_string(entry.name),
                        SIM_make_attr_boolean(entry.active),
                        SIM_make_attr_boolean(entry.oneshot),
                        SIM_make_attr_floating(entry.interval),
                        SIM_make_attr_uint64(entry.minx),
                        SIM_make_attr_uint64(entry.miny),
                        SIM_make_attr_uint64(entry.maxx),
                        SIM_make_attr_uint64(entry.maxy),
                        SIM_make_attr_data(
                                break_patch_size(entry.minx, entry.miny,
                                                 entry.maxx, entry.maxy)
                                * sizeof *entry.data,
                                entry.data));
                SIM_attr_list_set_item(&ret, i, list);                
        }
        return ret;
}

static attr_value_t
get_next_gfx_break_id(conf_object_t *obj)
{
        gfx_break_data_t *gb = break_data(obj);
        return SIM_make_attr_int64(gb->next_hap_id);
}

static attr_value_t
get_vga_text_data(conf_object_t *obj)
{
        gfx_break_data_t *gb = break_data(obj);
        if (gb->device && gb->text_int->text_mode(gb->device)) {
                // Update internal state from VGA device.
                gfx_update_screen_text(gb);
                gfx_console_screen_text_t data = gfx_get_text_data(gb);
                attr_value_t lengths = SIM_alloc_attr_list(data.rows);
                for (int i = 0; i < data.rows; i++)
                        SIM_attr_list_set_item(
                                &lengths, i,
                                SIM_make_attr_uint64(
                                        data.line_lengths.data[i]));
                        
                return SIM_make_attr_list(
                        6,
                        SIM_make_attr_uint64(data.rows),
                        SIM_make_attr_uint64(data.columns),
                        SIM_make_attr_uint64(data.font_width),
                        SIM_make_attr_uint64(data.font_height),
                        lengths,
                        SIM_make_attr_data(data.text.len, data.text.data));
        } else
                return SIM_make_attr_nil();
}

static set_error_t
set_refresh_in_virtual_time(conf_object_t *obj, attr_value_t *val)
{
        gfx_break_data_t *gb = break_data(obj);
        bool is_virtual = SIM_attr_boolean(*val);

        if (is_virtual
            && SIM_object_is_configured(obj)
            && !SIM_picosecond_clock(obj)) {
                SIM_attribute_error("Cannot refresh in virtual time"
                                    " without an object clock.");
                return Sim_Set_Illegal_Value;
        }

        gb->refresh_in_virtual_time = is_virtual;
        if (SIM_object_is_configured(obj)
            || !SIM_is_restoring_state(obj))
                reset_vga_refresh(gb);

       return Sim_Set_Ok;
}

static attr_value_t
get_refresh_in_virtual_time(conf_object_t *obj)
{
        gfx_break_data_t *gb = break_data(obj);
        return SIM_make_attr_boolean(gb->refresh_in_virtual_time);
}

gfx_break_data_t *
make_gfx_break(gfx_console_t *gc)
{
        gfx_break_data_t *gb = MM_ZALLOC(1, gfx_break_data_t);
        gb->gc = gc;
        VINIT(gb->breaks);
        gb->refresh_rate = 50;
        pthread_mutex_init(&gb->text_mutex, NULL);
        return gb;
}

void
finalise_gfx_break(gfx_break_data_t *gb)
{
        if (gb->device) {
                SIM_require_object(gb->device);
                // Make sure VGA device sends an initial complete update.
                request_vga_refresh_all(gb);
        }
}

void
pre_delete_gfx_break(gfx_break_data_t *gb)
{
        disable_gfx_device_refresh(gb, false);
}

void
destroy_gfx_break(gfx_break_data_t *gb)
{
        clear_break_data(gb);
        pthread_mutex_lock(&gb->text_mutex);
        MM_FREE((uint8 *)gb->text_data.text.data);
        MM_FREE((uint8 *)gb->text_data.line_lengths.data);
        pthread_mutex_unlock(&gb->text_mutex);
        pthread_mutex_destroy(&gb->text_mutex);
        MM_FREE(gb);
}

void
register_gfx_break(conf_class_t *cl)
{
        static const gfx_break_interface_t gfx_break_iface = {
                .store = gfx_break_store,
                .add = gfx_break_add,
                .remove = gfx_break_remove,
                .match = gfx_break_match,
                .info = gfx_break_info,
                .export_png = gfx_break_export_png,
                .add_bytes = gfx_break_add_bytes,
        };
        SIM_REGISTER_INTERFACE(cl, gfx_break, &gfx_break_iface);

        static const vga_text_update_interface_t vtu_iface = {
                .write = vga_text_update_write,
        };
        SIM_REGISTER_INTERFACE(cl, vga_text_update, &vtu_iface);
        
        hap_gfx_break = SIM_hap_add_type(
                "Gfx_Break",
                "I", "gfx_break",
                "break_id",
                 "Triggered when a graphical breakpoint matches the screen."
                 " <i>break_id</i> is the number returned when a breakpoint"
                 " is set.", 1);

        SIM_register_attribute(
                cl, "device",
                get_device,
                set_device,
                Sim_Attr_Optional,
                "o|n",
                "Video device the console is connected to.");
        
        SIM_register_attribute(
                cl, "refresh_rate",
                get_refresh_rate,
                set_refresh_rate,
                Sim_Attr_Optional, "i",
                "Refresh rate (in hertz)."
                " Set to 0 to disable screen refresh");

        // TODO Should graphical breakpoints be checkpointed?
        SIM_register_attribute(
                cl, "gfx_breaks",
                get_gfx_breaks, 0,
                Sim_Attr_Pseudo,
                "[[isbbfiiiid]*]",
                "List of console graphical breakpoints. Each entry is"
                " (hap id, name, active, oneshot, update interval,"
                " minx, miny, maxx, maxy, image data)");

        // TODO Should graphical breakpoints be checkpointed?
        SIM_register_attribute(
                cl, "next_gfx_break_id",
                get_next_gfx_break_id, 0,
                Sim_Attr_Pseudo,
                "i",
                "Hap ID for next added breakpoint.");

        SIM_register_attribute(
                cl, "vga_text_data",
                get_vga_text_data, 0,
                Sim_Attr_Pseudo,
                "[iiii[i*]d]|n",
                "VGA text screen contents:"
                " (rows, columns, font width, font height,"
                " line lengths, text data)."
                " NIL if not currently in VGA text mode.");

        SIM_register_attribute(
                cl, "refresh_in_virtual_time",
                get_refresh_in_virtual_time,
                set_refresh_in_virtual_time,
                Sim_Attr_Optional, "b",
                "If TRUE, then the refresh rate will refresh the screen based"
                " on virtual time, instead of real time. Default is FALSE.");

        // TODO Should graphical breakpoint event be checkpointed?
        break_event = SIM_register_event(
                "Graphical breakpoint", NULL, Sim_EC_Notsaved,
                check_gfx_break, NULL, NULL, NULL, NULL);

        virtual_refresh_event = SIM_register_event(
                "GFX Console refresh (virtual time)",
                cl, Sim_EC_Notsaved, vga_refresh_virtual,
                NULL, NULL, NULL, NULL);
}
