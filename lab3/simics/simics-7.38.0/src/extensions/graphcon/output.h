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

#ifndef GRAPHCON_OUTPUT_H
#define GRAPHCON_OUTPUT_H

#include <simics/base/conf-object.h>
#include <simics/simulator-iface/consoles.h>
#include "rect.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct gfx_output_data gfx_output_data_t;

int gfx_get_width(gfx_output_data_t *go);
int gfx_get_height(gfx_output_data_t *go);

bool gfx_save_png(gfx_output_data_t *go, const char *filename,
                  int width, int height);
bool gfx_save_png_data(gfx_output_data_t *go, const char *filename,
                       uint32 *data, int width, int height,
                       int png_width, int png_height);
void gfx_cmd_line_output(conf_object_t *obj, uint8 ch);

bool screen_contains_image_patch(
        gfx_output_data_t *go, const uint32 *data, 
        int left, int top, int right, int bottom);
void screen_copy_image_patch(
        gfx_output_data_t *go, uint32 *data,
        int left, int top, int right, int bottom);

const char *gfx_get_window_title(gfx_output_data_t *go);
void gfx_set_window_title(gfx_output_data_t *go, const char *title);

void frontend_warp_mouse(gfx_output_data_t *go, int x, int y);
void frontend_set_grab_mode(gfx_output_data_t *go, bool active);
void frontend_redraw(gfx_output_data_t *go);
void frontend_set_text_mode(gfx_output_data_t *go, bool text_mode);
void frontend_signal_text_update(gfx_output_data_t *go);
void frontend_set_window_title(gfx_output_data_t *go,
                               const char *short_title,
                               const char *long_title);
void frontend_set_grab_modifier(gfx_output_data_t *go, sim_key_t modifier);
void frontend_set_grab_button(gfx_output_data_t *go,
                              gfx_console_mouse_button_t button);
        
void frontend_set_visible(gfx_output_data_t *go, bool visible);
void gfx_enqueue_redraw(gfx_output_data_t *go);

void gfx_log_text(conf_object_t *obj, uint8 ch);

struct gfx_console;
gfx_output_data_t *make_gfx_output(struct gfx_console *gc, const char *name);
void finalise_gfx_output(gfx_output_data_t *go);
void pre_delete_gfx_output(gfx_output_data_t *go);
void destroy_gfx_output(gfx_output_data_t *go);
void register_gfx_output(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif

#endif

