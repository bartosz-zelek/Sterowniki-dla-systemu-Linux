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

#ifndef GRAPHCON_GFX_BREAK_H
#define GRAPHCON_GFX_BREAK_H

#include <simics/base/conf-object.h>
#include <simics/simulator-iface/consoles.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct gfx_break_data gfx_break_data_t;

void request_vga_refresh_all(gfx_break_data_t *gb);
void enable_gfx_device_refresh(gfx_break_data_t *gb);
void disable_gfx_device_refresh(gfx_break_data_t *gb, bool realtime);
gfx_console_screen_text_t gfx_get_text_data(gfx_break_data_t *gb);
void gfx_update_screen_text(gfx_break_data_t *gb);
bool gfx_refresh_enabled(gfx_break_data_t *gb);

struct gfx_console;
gfx_break_data_t *make_gfx_break(struct gfx_console *gc);
void finalise_gfx_break(gfx_break_data_t *gb);
void destroy_gfx_break(gfx_break_data_t *gb);
void pre_delete_gfx_break(gfx_break_data_t *gb);
void register_gfx_break(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif

#endif

