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

#include "output.h"
#include <simics/base/conf-object.h>
#include <simics/util/os.h>
#include <simics/util/swabber.h>
#include <simics/base/log.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator/hap-consumer.h>
#include <simics/simulator/output.h>
#include <simics/model-iface/video-interface.h>
#include <simics/model-iface/gfx-console.h>
#include <simics/devs/serial-device.h>
#include <simics/simulator-iface/consoles.h>

#include <pthread.h>

#include "gfx-console.h"
#include "input.h"
#include "vnc.h"
#include "save.h"
#include "gfx-break.h"
#include "rect.h"
#include "gfx-inline.h"

// Number of palette colours in indexed video modes.
#define PALETTE_SIZE 256

// Screen clearing colour.
#define DEFAULT_COL 0

/* Pixels are formatted as 0xRRGGBB (the top 8 bits are unused). */
typedef uint32 pixel_t;

struct gfx_output_data {
        gfx_console_t *gc;          /* Shoddy shortcut to containing object. */

        /* Screen size in pixels. */
        int width;        
        int height;

        /* Screen data (width * height elements).
           Pixel format is xRGB, hence the memory on little endian looks like
           BGRxBGRx... */
        pixel_t *screen;

        // Palette data, only used in some video modes.
        pixel_t palette[PALETTE_SIZE];

        // Current dirty screen rectangle, not yet sent to the frontend.
        gfx_rect_t dirty;

        // If not NULL, this file will receive all text output, received by the
        // console over the vga_text_update interface.
        FILE *output_file;

        // Must be non-NULL, and empty string iff output_file is NULL.
        char *output_file_name;

        // Console frontend object.
        conf_object_t *frontend;
        const gfx_console_frontend_interface_t *frontend_iface;

        // Handle that identifies this console to the frontend, in case the
        // frontend object manages several consoles.
        int frontend_handle;

        // Console GUI window title.
        char *window_title;

        // Current state of keyboard LEDs.
        gfx_console_led_t led_state;

        // Frontend deletion hap callback
        hap_handle_t frontend_del_hap;

        // Should (printable) output be sent to the Simics log and console?
        int cmd_line_output;

        // Text line to be sent to Simics log and console.
        strbuf_t output_line;

        /* Maximum screen size in pixels. */
        int max_width;
        int max_height;
};

// Set console screen data structure to default colour.
static void
clear_screen(gfx_output_data_t *go)
{
        int size = go->width * go->height;
        for (int i = 0; i < size; i++)
                go->screen[i] = DEFAULT_COL;
}

// Return red component from our pixel format.
static uint8
col_r(pixel_t col)
{
        return (col >> 16) & 0xff;
}

// Return green component from our pixel format.
static uint8
col_g(pixel_t col)
{
        return (col >> 8) & 0xff;
}

// Return blue component from our pixel format.
static uint8
col_b(pixel_t col)
{
        return col & 0xff;
}

// Create our internal pixel representation from given colour components.
static pixel_t
colour(uint8 r, uint8 g, uint8 b)
{
        return (pixel_t)b | ((pixel_t)g << 8) | ((pixel_t)r << 16);
}

static gfx_output_data_t *
output_data(conf_object_t *obj)
{
        return from_obj(obj)->output_data;
}

static conf_object_t *
go_obj(gfx_output_data_t *go) { return to_obj(go->gc); }

static gfx_break_data_t *
go_gb(gfx_output_data_t *go) { return go->gc->gfx_break_data; }

static gfx_input_data_t *
go_gi(gfx_output_data_t *go) { return go->gc->input_data; }

// Stride in bytes of internal screen data structure.
static int
screen_stride(gfx_output_data_t *go)
{
        return go->width * sizeof go->screen[0];
}

// Size in bytes of the screen region consisting of lines miny..maxy (inclusive)
static int
region_data_size(gfx_output_data_t *go, int miny, int maxy)
{
        return (maxy - miny + 1) * screen_stride(go);
}

// Total size in bytes of screen data structure.
static int
screen_data_size(gfx_output_data_t *go)
{
        return region_data_size(go, 0, go->height - 1);
}

// Make sure the whole screen is sent to the frontend on the next redraw event.
void
frontend_redraw(gfx_output_data_t *go)
{
        go->dirty = create_rect(0, 0, go->width - 1, go->height - 1);
}

// Wrapper for iface.gfx_console_frontend.set_text_mode.
void
frontend_set_text_mode(gfx_output_data_t *go, bool text_mode)
{
        if (go->frontend) {
                go->frontend_iface->set_text_mode(
                        go->frontend, go->frontend_handle, text_mode);
        }
}

// Wrapper for iface.gfx_console_frontend.set_mouse_pos.
void
frontend_warp_mouse(gfx_output_data_t *go, int x, int y)
{
        go->frontend_iface->set_mouse_pos(
                go->frontend, go->frontend_handle, x, y);
}

// Wrapper for iface.gfx_console_frontend.set_grab_mode.
void
frontend_set_grab_mode(gfx_output_data_t *go, bool active)
{
        go->frontend_iface->set_grab_mode(
                go->frontend, go->frontend_handle, active);
}

// Wrapper for iface.gfx_console_frontend.signal_text_update.
void
frontend_signal_text_update(gfx_output_data_t *go)
{
        if (go->frontend) {
                go->frontend_iface->signal_text_update(
                        go->frontend, go->frontend_handle);
        }
}

// Wrapper for iface.gfx_console_frontend.set_title.
void
frontend_set_window_title(gfx_output_data_t *go,
                          const char *short_title, const char *long_title)
{       
        if (go->frontend)
                go->frontend_iface->set_title(
                        go->frontend, go->frontend_handle,
                        short_title, long_title);
}

// Wrapper for iface.gfx_console_frontend.set_grab_modifier
void
frontend_set_grab_modifier(gfx_output_data_t *go, sim_key_t modifier)
{       
        if (go->frontend)
                go->frontend_iface->set_grab_modifier(
                        go->frontend, go->frontend_handle, modifier);
}

// Wrapper for iface.gfx_console_frontend.set_grab_button
void
frontend_set_grab_button(gfx_output_data_t *go,
                         gfx_console_mouse_button_t button)
{       
        if (go->frontend)
                go->frontend_iface->set_grab_button(
                        go->frontend, go->frontend_handle, button);
}

void
frontend_set_visible(gfx_output_data_t *go, bool visible)
{
        if (go->frontend)
                go->frontend_iface->set_visible(
                        go->frontend, go->frontend_handle, visible);
}


// Determine if the given rectangle on the screen matches the given data.
bool
screen_contains_image_patch(gfx_output_data_t *go, const pixel_t *data,
                            int left, int top, int right, int bottom)
{
        for (int y = top; y <= bottom; y++) {
                for (int x = left; x <= right; x++) {
                        if ((*data & 0xffffff)
                            != (go->screen[y * go->width + x] & 0xffffff))
                                return false;
                        data++;
                }
        }
        return true;
}

// Extract the specified screen rectangle into the given data, which must be
// allocated and of size (right - left + 1) * (bottom - top + 1) pixels.
void
screen_copy_image_patch(gfx_output_data_t *go, pixel_t *data,
                        int left, int top, int right, int bottom)
{
        for (int y = top; y <= bottom; y++)
                for (int x = left; x <= right; x++)
                        *data++ = go->screen[y * go->width + x];
}

// Set dimensions of the console screen. Does not update frontend.
// Return true if the console data structure was updated
// i.e. if the given dimensions are different from the current ones.
static bool
resize_screen(gfx_output_data_t *go, int width, int height)
{
        if (width == go->width && height == go->height)
                return false;

        if (go->frontend)
                go->frontend_iface->invalidate_contents(
                        go->frontend, go->frontend_handle);
        
        size_t new_size = width * height;
        pixel_t *new_screen = MM_MALLOC(new_size, pixel_t);

        /* Keep the topmost left part of the old contents. */
        for (int y = 0; y < MIN(go->height, height); y++) {
                memcpy(new_screen + y * width,
                       go->screen + y * go->width,
                       MIN(go->width, width) * sizeof(pixel_t));

                /* Fill to the right with the default background colour. */
                for (int x = go->width; x < width; x++)
                        new_screen[y * width + x] = DEFAULT_COL;
        }

        /* Fill below with the default background colour. */
        for (int y = go->height; y < height; y++)
                for (int x = 0; x < width; x++)
                        new_screen[y * width + x] = DEFAULT_COL;

        /* Ensure that VNC stops using the old pointer before we release it */
        vnc_console_set_screen(go->gc->vnc_data, new_screen, width, height);

        MM_FREE(go->screen);
        go->screen = new_screen;
        go->width = width;
        go->height = height;

        return true;
}

// Return console screen width in pixels.
int
gfx_get_width(gfx_output_data_t *go)
{
        return go->width;
}

// Return console screen height in pixels.
int
gfx_get_height(gfx_output_data_t *go)
{
        return go->height;
}

// Change console screen dimensions, and update frontend and VNC clients.
static void
update_size(gfx_output_data_t *go, int width, int height)
{
        SIM_LOG_INFO(3, go_obj(go), Gfx_Console_Log_Output,
                     "Resize screen to %dx%d", width, height);
        if (resize_screen(go, width, height)) {
                if (go->frontend) {
                        go->frontend_iface->set_size(
                                go->frontend, go->frontend_handle,
                                width, height);
                        frontend_redraw(go);
                }
        }
}

// Implementation of gfx_con interface.
static int
gfx_con_set_color(conf_object_t *obj, uint8 index, uint8 r, uint8 g, uint8 b)
{
        gfx_output_data_t *go = output_data(obj);
        go->palette[index] = colour(r, g, b);
        return 0;
}

// Implementation of gfx_con interface.
static void
gfx_con_set_size(conf_object_t *obj, int width, int height)
{
        gfx_output_data_t *go = output_data(obj);
        if (width > 0 && height > 0
            && width <= go->max_width && height <= go->max_height) {
                gfx_output_data_t *go = output_data(obj);
                update_size(go, width, height);
        } else {
                SIM_LOG_ERROR(
                        obj, Gfx_Console_Log_Output,
                        "Invalid call to set_size width = %d, height = %d",
                        width, height);
        }
}

// Mark the given rectangular area as dirty, so it is pushed to the frontend
// and VNC clients in the next update event.
static void
mark_dirty_rectangle(gfx_output_data_t *go, gfx_rect_t dirty)
{
        go->dirty = bounding_box(go->dirty, dirty);
}

/* Convert a pixel value on the form 0bRRRRRGGGGGGBBBBB to our pixel_t. */
static pixel_t
rgb565_to_pixel(uint16 v)
{
        /* from:         RRRRRGGGGGGBBBBB
           to:   RRRRRrrrGGGGGGggBBBBBbbb
           where lower-case bits are the same as the first upper-case bits. */
        uint32 r = (v >> 8) & 0xf8;
        r |= r >> 5;
        uint32 g = (v >> 3) & 0xfc;
        g |= g >> 6;
        uint32 b = (v << 3) & 0xf8;
        b |= b >> 5;
        return r << 16 | g << 8 | b;
}

// Implementation of gfx_con interface.
static void
gfx_con_put_block(conf_object_t *obj, bytes_t block,
                  int minx, int miny, int maxx, int maxy,
                  gfx_con_pixel_fmt_t src_fmt, int stride)
{
        gfx_output_data_t *go = output_data(obj);
        if (minx < 0 || miny < 0 || maxx >= go->width || maxy >= go->height) {
                SIM_LOG_SPEC_VIOLATION(2, go_obj(go), Gfx_Console_Log_Output,
                                       "put_block (minx, miny, maxx, maxy) "
                                       "= (%d, %d, %d, %d) is invalid for "
                                       "screen size (%d, %d)",
                                       minx, miny, maxx, maxy, 
                                       go->width, go->height);
                return;
        }
        if (minx > maxx || miny > maxy) {
                SIM_LOG_SPEC_VIOLATION(
                        2, go_obj(go), Gfx_Console_Log_Output,
                        "Invalid call to put_block (minx, miny, maxx, maxy) "
                        "= (%d, %d, %d, %d)", minx, miny, maxx, maxy);
                return;
        }
        maxx = MIN(maxx, go->width - 1);
        maxy = MIN(maxy, go->height - 1);
        minx = MAX(0, minx);
        miny = MAX(0, miny);
        pixel_t *dst = &go->screen[miny * go->width + minx];
        int w = maxx - minx + 1;
        int h = maxy - miny + 1;
        const uint8 *src = block.data;

        pixel_t org[w];
        int skip_x = w;
        int skip_xx = -1;

        for (int j = 0; j < h; j++) {
                memcpy(org, dst, sizeof org);
                switch (src_fmt) {
                case GFX_8BIT_INDEXED:
                        for (int i = 0; i < w; i++)
                                dst[i] = go->palette[src[i]];
                        break;
                case GFX_RGB_565: {
                        const uint16 *s = (const uint16 *)src;
                        for (int i = 0; i < w; i++)
                                dst[i] = rgb565_to_pixel(s[i]);
                        break;
                }
                case GFX_RGB_888: {
                        const uint8 *s = src;
                        /* Source bytes are in B, G, R order. */
                        for (int i = 0; i < w; i++) {
                                dst[i] = colour(s[2], s[1], s[0]);
                                s += 3;
                        }
                        break;
                }
                case GFX_xRGB_8888: {
                        memcpy(dst, src, w * sizeof *dst);
                        break;
                }
                }
                /* Remove area which has not been modified */
                for (int i = 0; i < skip_x; i++) {
                        if (org[i] != dst[i])
                                skip_x = i;
                }
                if (skip_x == w) {
                        miny++;
                } else {
                        for (int i = w - 1; i > skip_xx; i--) {
                                if (org[i] != dst[i])
                                        skip_xx = i;
                        }
                }

                src += stride;
                dst += go->width;
        }
        maxx = minx + skip_xx;
        minx += skip_x;

        // We just keep the bounding box as new dirty rectangle, instead of
        // storing a set of rectangles.
        if (minx <= maxx && miny <= maxy) {
                mark_dirty_rectangle(go, create_rect(minx, miny, maxx, maxy));
        }
}

// Implementation of gfx_con interface.
static void
gfx_con_put_block_old(conf_object_t *obj,
                      uint8 *src, int minx, int miny,
                      int maxx, int maxy, int src_fmt,
                      int stride, int unused)
{
        bytes_t block = {
                .data = src,
                .len = stride * (maxy - miny + 1),
        };
        gfx_con_put_block(obj, block, minx, miny, maxx, maxy, src_fmt, stride);
}

// Send screen data for current dirty rectangle to frontend and VNC clients.
// Returns true if there was a dirty rectange.
static bool
frontend_update(gfx_output_data_t *go)
{
        gfx_rect_t rect = go->dirty;
        bool is_dirty = !empty_rect(rect);
        go->dirty = (gfx_rect_t){0};

        if (go->frontend && is_dirty) {
                int maxx = rect.x_pos + rect.width - 1;
                int maxy = rect.y_pos + rect.height - 1;
                go->frontend_iface->set_contents(
                        go->frontend, go->frontend_handle,
                        rect.x_pos, rect.y_pos, maxx, maxy,
                        go->screen);
        }
        // Call VNC redraw even if the dirty rectangle is empty since
        // the VNC client might have dirty rectangles of its own.
        vnc_screen_rect_dirty(go->gc->vnc_data, rect);
        return is_dirty;
}

// Mark the given pixel as dirty, so it is pushed to the frontend
// and VNC clients in the next update event.
static void
mark_dirty_pixel(gfx_output_data_t *go, int x, int y)
{
        mark_dirty_rectangle(go, create_rect(x, y, x, y));
}

// Implementation of gfx_con interface.
static void
gfx_con_put_pixel_rgb(conf_object_t *obj, int x, int y, uint32 rgb)
{
        gfx_output_data_t *go = output_data(obj);
        if (y < 0 || y >= go->height || x < 0 || x >= go->width) {
                SIM_LOG_SPEC_VIOLATION(
                        2, go_obj(go), Gfx_Console_Log_Output,
                        "Put pixel at (%d, %d) on screen of size %dx%d",
                        x, y, go->width, go->height);
        } else {
                /* The pixel format is already correct. */
                go->screen[y * go->width + x] = rgb;
                mark_dirty_pixel(go, x, y);
        }
}

// Implementation of gfx_con interface.
static void
gfx_con_put_pixel_col(conf_object_t *obj, int x, int y,
                      int r, int g, int b)
{
        gfx_con_put_pixel_rgb(obj, x, y, colour(r, g, b));
}

// Implementation of gfx_con interface.
static void
gfx_con_put_pixel(conf_object_t *obj, int x, int y, uint8 index)
{
        gfx_output_data_t *go = output_data(obj);
        gfx_con_put_pixel_rgb(obj, x, y, go->palette[index]);
}

// Do an immediate redraw of dirty screen rectangle, of frontend and VNC.
static void
gfx_redraw(gfx_output_data_t *go)
{
        bool was_dirty = frontend_update(go);
        if (was_dirty)
                gfx_update_screen_text(go->gc->gfx_break_data);
}

// Make sure a screen redraw is done soon, i.e. do it immediately or from
// next screen update event.
void
gfx_enqueue_redraw(gfx_output_data_t *go)
{
        if (!gfx_refresh_enabled(go_gb(go)))
                gfx_redraw(go);
}

// Implementation of gfx_con interface.
// Called by VGA device when we ask for redraw using the vga_update interface.
// Since we do this using the realtime update event, this is the top-level
// console screen update function.
static void
gfx_con_redraw(conf_object_t *obj)
{
        gfx_redraw(output_data(obj));
}

// Implementation of gfx_con interface.
static void
gfx_con_update_keyboard_leds(conf_object_t *obj, int led_change)
{
        gfx_output_data_t *go = output_data(obj);
        switch (led_change) {
        case KBD_CAPSLOCK_ON:
                go->led_state |= Gfx_Console_Led_Caps;
                break;
        case KBD_CAPSLOCK_OFF:
                go->led_state &= ~Gfx_Console_Led_Caps;
                break;
        case KBD_NUMLOCK_ON:
                go->led_state |= Gfx_Console_Led_Num;
                break;
        case KBD_NUMLOCK_OFF:
                go->led_state &= ~Gfx_Console_Led_Num;
                break;
        case KBD_SCROLLLOCK_ON:
                go->led_state |= Gfx_Console_Led_Scroll;
                break;
        case KBD_SCROLLLOCK_OFF:
                go->led_state &= ~Gfx_Console_Led_Scroll;
                break;
        }
        
        if (go->frontend) {
                go->frontend_iface->set_keyboard_leds(
                        go->frontend, go->frontend_handle, go->led_state);
        }
}

void
gfx_log_text(conf_object_t *obj, uint8 ch)
{
        if (SIM_object_is_configured(obj)) {
                gfx_output_data_t *go = output_data(obj);
                if (go->output_file) {
                        size_t written = fwrite(&ch, 1, 1, go->output_file);
                        if (written != 1)
                                SIM_LOG_ERROR(
                                        go_obj(go), Gfx_Console_Log_Output,
                                        "Could not write to output file");
                        fflush(go->output_file);
                }
        }
}

// Send output_line to Simics console if end of line.
void
gfx_cmd_line_output(conf_object_t *obj, uint8 ch)
{
        gfx_output_data_t *go = output_data(obj);
        if (go->cmd_line_output == 1 ||
            (go->cmd_line_output == -1 && !console_visible(go_gi(go)))) {
                if (ch == '\n') {
                        sb_escape(&go->output_line, 0);
                        SIM_printf("<%s>%s\n", SIM_object_name(obj),
                                   sb_str(&go->output_line));
                        sb_clear(&go->output_line);
                } else {
                        // Append characters to output line for log and console.
                        sb_addc(&go->output_line, ch);
                }
        }
}

// Implementation of extended_serial interface.
// Used by VGA device to send text data to the console.
// Here we only use this to implement a text output logging backwards
// compatible with the old xterm-based "vga text" console.
static void
extended_serial_write_at(conf_object_t *obj,
                         int value, int x, int y, int fg, int bg)
{
        gfx_log_text(obj, value);
}

// Implementation of extended_serial interface.
static void
extended_serial_graphics_mode(conf_object_t *obj, int in_graphics_mode)
{
}

/* Given a RGBA image, return a heap-allocated resized copy of it. */
static uint32 *
img_copy_resize(const uint32 *src_image, int src_width, int src_height,
                int dst_width, int dst_height)
{
        uint32 *dst_image = MM_MALLOC(dst_width * dst_height, uint32);
        for (int y = 0; y < dst_height; y++) {
                int src_y = y * src_height / dst_height;
                for (int x = 0; x < dst_width; x++) {
                        int src_x = x * src_width / dst_width;
                        const uint32 *src =
                                src_image + src_y * src_width + src_x;
                        uint32 *dst = dst_image + y * dst_width + x;
                        *dst = *src;
                }
        }
        return dst_image;
}

// TODO: also expose through gfx-breakpoint interface?
static bool
gfx_save_bmp_data(gfx_output_data_t *go, const char *filename,
                  uint32 *data, int width, int height,
                  int bmp_width, int bmp_height)
{
        FILE *f = os_fopen(filename, "wb");
        if (!f) {
                char *err = os_describe_last_error();
                SIM_LOG_ERROR(go_obj(go), Gfx_Console_Log_Output,
                              "Opening %s: %s", filename, err);
                MM_FREE(err);
                return false;
        }

        ASSERT(bmp_width <= width && bmp_height <= height);
        bool ok;
        if (bmp_width < width && bmp_height < height) {
                uint32 *img = img_copy_resize(
                        data, width, height, bmp_width, bmp_height);
                ok = write_bmp(f, img, bmp_width, bmp_height);
                MM_FREE(img);
        } else
                ok = write_bmp(f, data, width, height);

        os_fclose(f);
        if (!ok) {
                SIM_LOG_ERROR(go_obj(go), Gfx_Console_Log_Output,
                              "BMP write error");
                return false;
        }
        return true;
}

// Save the current screen to the specified file using BMP format, and shrunken
// to the specified size.
// Returns false on file or BMP error.
static bool
gfx_save_bmp(gfx_output_data_t *go, const char *filename, int width, int height)
{
        return gfx_save_bmp_data(go, filename, go->screen,
                                 go->width, go->height, width, height);
}

// Implementation of screenshot interface.
static bool
screenshot_save_bmp(conf_object_t *obj, const char *filename)
{
        gfx_output_data_t *go = output_data(obj);
        return gfx_save_bmp(go, filename, go->width, go->height);
}

bool
gfx_save_png_data(gfx_output_data_t *go, const char *filename,
                  uint32 *data, int width, int height,
                  int png_width, int png_height)
{
        FILE *f = os_fopen(filename, "wb");
        if (!f) {
                char *err = os_describe_last_error();
                SIM_LOG_ERROR(go_obj(go), Gfx_Console_Log_Output,
                              "Opening %s: %s", filename, err);
                MM_FREE(err);
                return false;
        }

        ASSERT(png_width <= width && png_height <= height);
        bool ok;
        if (png_width < width && png_height < height) {
                uint32 *img = img_copy_resize(
                        data, width, height, png_width, png_height);
                ok = write_png(f, img, png_width, png_height);
                MM_FREE(img);
        } else
                ok = write_png(f, data, width, height);
        
        os_fclose(f);
        if (!ok) {
                SIM_LOG_ERROR(go_obj(go), Gfx_Console_Log_Output,
                              "PNG write error");
                return false;
        }
        return true;
}

// Save the current screen to the specified file using PNG format, and shrunken
// to the specified size.
// Returns false on file or PNG error.
bool
gfx_save_png(gfx_output_data_t *go, const char *filename, int width, int height)
{
        return gfx_save_png_data(go, filename, go->screen,
                                 go->width, go->height, width, height);
}

// Implementation of screenshot interface.
static bool
screenshot_save_png(conf_object_t *obj, const char *filename)
{
        gfx_output_data_t *go = output_data(obj);
        return gfx_save_png(go, filename, go->width, go->height);
}

// Return console GUI window title.
const char *
gfx_get_window_title(gfx_output_data_t *go)
{
        return go->window_title;
}

// Set console GUI window title, but does not update frontend.
void
gfx_set_window_title(gfx_output_data_t *go, const char *title)
{
        MM_FREE(go->window_title);
        go->window_title = MM_STRDUP(title);
}

static set_error_t
set_screen_size(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        int width = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        int height = SIM_attr_integer(SIM_attr_list_item(*val, 1));

        if (width > 0 && height > 0
            && width <= go->max_width && height <= go->max_height) {
                update_size(go, width, height);
                return Sim_Set_Ok;
        } else {
                strbuf_t err = sb_newf(
                        "Invalid screen size %dx%d", width, height);
                SIM_attribute_error(sb_str(&err));
                sb_free(&err);
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_screen_size(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(go->width),
                SIM_make_attr_uint64(go->height));
}

static attr_value_t
get_palette(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        attr_value_t ret = SIM_alloc_attr_list(PALETTE_SIZE);
        for (int i = 0; i < PALETTE_SIZE; i++) {
                pixel_t rgb = go->palette[i];
                SIM_attr_list_set_item(
                        &ret, i, SIM_make_attr_list(
                                3,
                                SIM_make_attr_uint64(col_r(rgb)),
                                SIM_make_attr_uint64(col_g(rgb)),
                                SIM_make_attr_uint64(col_b(rgb))));
        }
        return ret;
}

static set_error_t
set_palette(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        for (int i = 0; i < PALETTE_SIZE; i++) {
                attr_value_t col = SIM_attr_list_item(*val, i);
                go->palette[i] =
                        colour(SIM_attr_integer(SIM_attr_list_item(col, 0)),
                               SIM_attr_integer(SIM_attr_list_item(col, 1)),
                               SIM_attr_integer(SIM_attr_list_item(col, 2)));
        }
        // TODO : redraw?
        return Sim_Set_Ok;
}

static set_error_t
set_screen_data(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        const uint8 *data = SIM_attr_data(*val);
        if (SIM_attr_data_size(*val) == screen_data_size(go)) {
                memcpy(go->screen, data, screen_data_size(go));
                if (SIM_object_is_configured(obj))
                        frontend_redraw(go);
                return Sim_Set_Ok;
        } else {
                strbuf_t err = sb_newf(
                        "Screen data must have size %d, not %d",
                        screen_data_size(go),
                        SIM_attr_data_size(*val));
                SIM_attribute_error(sb_str(&err));
                sb_free(&err);
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_screen_data(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_data(screen_data_size(go), go->screen);
}

static set_error_t
set_window_title(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        const char *str = SIM_attr_string(*val);
        gfx_set_window_title(go, str);
        // Add grab info text to window title and update frontend.
        set_window_title_with_grab_info(go->gc->input_data);
        return Sim_Set_Ok;
}

static attr_value_t
get_window_title(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_string(gfx_get_window_title(go));
}

static set_error_t
set_output_file(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        const char *str = SIM_attr_string(*val);
        FILE *file = NULL;
        
        if (strcmp(str, "") != 0) {
                file = os_fopen(str, "ab");
                if (!file)
                        return Sim_Set_Illegal_Value;
        }
        // Close old file.
        if (go->output_file)
                os_fclose(go->output_file);
        // Store new file (or NULL).
        go->output_file = file;
        MM_FREE(go->output_file_name);
        go->output_file_name = MM_STRDUP(str);
        return Sim_Set_Ok;
}

static attr_value_t
get_output_file(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_string(go->output_file_name);
}

static set_error_t
set_cmd_line_output(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        if (SIM_attr_is_nil(*val))
                go->cmd_line_output = -1;
        else
                go->cmd_line_output = SIM_attr_boolean(*val);

        if (!go->cmd_line_output)
                sb_clear(&go->output_line);
        return Sim_Set_Ok;
}

static attr_value_t
get_cmd_line_output(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return (go->cmd_line_output == -1
                ? SIM_make_attr_nil()
                : SIM_make_attr_boolean(go->cmd_line_output));
}

static set_error_t
set_output_line(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        if (SIM_attr_data(*val) && SIM_attr_data_size(*val) > 0)
                sb_set(&go->output_line, (const char *)SIM_attr_data(*val));
        else
                sb_clear(&go->output_line);
        return Sim_Set_Ok;
}

static attr_value_t
get_output_line(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_data(sb_len(&go->output_line) + 1,
                                  sb_str(&go->output_line));
}

static attr_value_t
get_keyboard_leds(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_list(
                3,
                SIM_make_attr_boolean((go->led_state
                                       & Gfx_Console_Led_Caps) > 0),
                SIM_make_attr_boolean((go->led_state
                                       & Gfx_Console_Led_Num) > 0),
                SIM_make_attr_boolean((go->led_state
                                       & Gfx_Console_Led_Scroll) > 0));
}

static set_error_t
set_max_screen_size(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        int width = SIM_attr_integer(SIM_attr_list_item(*val, 0));
        int height = SIM_attr_integer(SIM_attr_list_item(*val, 1));

        if (width >= go->width && height >= go->height) {
                go->max_width = width;
                go->max_height = height;
                return Sim_Set_Ok;
        } else {
                strbuf_t err = sb_newf(
                        "Invalid max screen size %dx%d", width, height);
                SIM_attribute_error(sb_str(&err));
                sb_free(&err);
                return Sim_Set_Illegal_Value;
        }
}

static attr_value_t
get_max_screen_size(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_list(
                2,
                SIM_make_attr_uint64(go->max_width),
                SIM_make_attr_uint64(go->max_height));
}


static void
init_frontend(gfx_output_data_t *go)
{
        set_window_title_with_grab_info(go->gc->input_data);
        go->frontend_iface->set_size(go->frontend, go->frontend_handle,
                                      go->width, go->height);
        vnc_console_set_screen(go->gc->vnc_data, go->screen,
                               go->width, go->height);

        // Set initial dirty rectangle to whole screen.
        frontend_redraw(go);

        // Send immediate screen update to frontend (in case it is visible)
        frontend_update(go);
}

static attr_value_t
get_frontend_handle(conf_object_t *obj)
{
        gfx_output_data_t *go = output_data(obj);
        return SIM_make_attr_uint64((unsigned)go->frontend_handle);
}

static set_error_t
set_frontend_handle(conf_object_t *obj, attr_value_t *val)
{
        gfx_output_data_t *go = output_data(obj);
        go->frontend_handle = SIM_attr_integer(*val);
        init_frontend(go);
        return Sim_Set_Ok;
}

gfx_output_data_t *
make_gfx_output(gfx_console_t *gc, const char *name)
{
        gfx_output_data_t *go = MM_ZALLOC(1, gfx_output_data_t);
        go->gc = gc;
        go->window_title = MM_STRDUP(name);       
        go->width = 640;
        go->height = 480;
        go->screen = MM_MALLOC(go->height * go->width, pixel_t);
        clear_screen(go);
        go->frontend = NULL;
        go->frontend_iface = NULL;
        go->output_file_name = MM_STRDUP("");
        go->dirty = (gfx_rect_t){0};
        go->cmd_line_output = -1;
        sb_init(&go->output_line);
        
        for (int i = 0; i < PALETTE_SIZE; i++)
                go->palette[i] = colour(0, 0, 0);

        go->max_width = 65536;
        go->max_height = 65536;

        return go;
}

static void
frontend_del_cb(lang_void *data, conf_object_t *frontend)
{
        gfx_output_data_t *go = data;
        go->frontend = NULL;
        SIM_hap_delete_callback_obj_id("Core_Conf_Object_Delete", frontend,
                                       go->frontend_del_hap);
}

void
finalise_gfx_output(gfx_output_data_t *go)
{
        go->frontend = get_gfx_console_frontend(go_obj(go));
        ASSERT(go->frontend);
        go->frontend_iface =
                SIM_C_GET_INTERFACE(go->frontend, gfx_console_frontend);
        ASSERT(go->frontend_iface);
        SIM_require_object(go->frontend);
        go->frontend_handle = go->frontend_iface->start(go->frontend,
                                                        go_obj(go));
        
        init_frontend(go);
        go->frontend_del_hap = SIM_hap_add_callback_obj(
                "Core_Conf_Object_Delete", go->frontend, 0,
                (obj_hap_func_t)frontend_del_cb, go);
}

void
pre_delete_gfx_output(gfx_output_data_t *go)
{
        ASSERT(go->frontend_iface);
        go->frontend_iface->stop(go->frontend, go->frontend_handle);
        SIM_delete_object(go->frontend);
}

void
destroy_gfx_output(gfx_output_data_t *go)
{
        MM_FREE(go->output_file_name);
        if (go->output_file)
                os_fclose(go->output_file);
        MM_FREE(go->screen);
        MM_FREE(go->window_title);
        MM_FREE(go);
}

void
register_gfx_output(conf_class_t *cl)
{
        static const gfx_con_interface_t gc_iface = {
                .set_color = gfx_con_set_color,
                .set_size = gfx_con_set_size,
                .put_pixel = gfx_con_put_pixel,
                .put_pixel_rgb = gfx_con_put_pixel_rgb,
                .put_pixel_col = gfx_con_put_pixel_col,
                .put_block = gfx_con_put_block,
                .put_block_old = gfx_con_put_block_old,
                .redraw = gfx_con_redraw,
                .update_keyboard_leds = gfx_con_update_keyboard_leds,
        };
        SIM_REGISTER_INTERFACE(cl, gfx_con, &gc_iface);

        static const screenshot_interface_t screenshot_iface = {
                .save_png = screenshot_save_png,
                .save_bmp = screenshot_save_bmp,
        };
        SIM_REGISTER_INTERFACE(cl, screenshot, &screenshot_iface);

        static const extended_serial_interface_t es_iface = {
                .write_at = extended_serial_write_at,
                .graphics_mode = extended_serial_graphics_mode,
        };
        SIM_REGISTER_INTERFACE(cl, extended_serial, &es_iface);


        SIM_register_attribute(
                cl, "max_screen_size",
                get_max_screen_size,
                set_max_screen_size,
                Sim_Attr_Optional,
                "[ii]",
                "Console window maximum allowed screen"
                " size, in pixels (width, height).");
        
        SIM_register_attribute(
                cl, "window_title",
                get_window_title,
                set_window_title,
                Sim_Attr_Optional,
                "s",
                "Console GUI window title.");
        
        SIM_register_attribute(
                cl, "screen_size",
                get_screen_size,
                set_screen_size,
                Sim_Attr_Optional,
                "[ii]",
                "Console window size in pixels (width, height).");

        SIM_register_attribute(
                cl, "screen_data",
                get_screen_data,
                set_screen_data,
                Sim_Attr_Optional,
                "d",
                "Console screen data.");

        SIM_ensure_partial_attr_order(cl, "screen_size", "screen_data");

        SIM_register_attribute(
                cl, "palette",
                get_palette,
                set_palette,
                Sim_Attr_Optional,
                "[[iii]{256}]",
                "<tt>((<i>r</i>, <i>g</i>, <i>b</i>), ...)</tt>"
                " Palette used in 8-bit mode. 256 colors, 8 bits/channel.");

        SIM_register_attribute(
                cl, "output_file",
                get_output_file,
                set_output_file,
                Sim_Attr_Pseudo,
                "s",
                "If set to a non-empty string, output will be"
                " directed to and appended to this file."
                " Set to an empty string to stop file output.");

        SIM_register_attribute(
                cl, "keyboard_leds",
                get_keyboard_leds, 0,
                Sim_Attr_Pseudo,
                "[bbb]",
                "State of keyboard leds (Caps lock, Num lock, Scroll lock).");

        SIM_register_attribute(
                cl, "cmd_line_output",
                get_cmd_line_output,
                set_cmd_line_output,
                Sim_Attr_Optional,
                "b|n",
                "If set to TRUE, the Simics command line will receive"
                " console text output, which will also be logged at level 3."
                " If set to NIL, command line and log output will happen"
                " automatically when the console GUI is invisible."
                " If set to FALSE, no command line or log output will happen.");

        SIM_register_attribute(
                cl, "output_line",
                get_output_line,
                set_output_line,
                Sim_Attr_Optional,
                "d",
                "The current output line of printable characters from the"
                " attached VGA device.");

        SIM_register_attribute(
                cl, "frontend_handle",
                get_frontend_handle,
                set_frontend_handle,
                Sim_Attr_Pseudo | Sim_Attr_Internal, "i",
                "GUI frontend handle.");
}
