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

#ifndef GRAPHCON_INPUT_H
#define GRAPHCON_INPUT_H

#include <simics/base/conf-object.h>
#include <simics/model-iface/sim-keys.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct gfx_input_data gfx_input_data_t;

void gfx_kbd_input(gfx_input_data_t *gi, sim_key_t code, bool down);
void gfx_mouse_input(gfx_input_data_t *gi, int x, int y, int z, int buttons);
void gfx_set_visible(gfx_input_data_t *gi, bool visible);
void gfx_set_vnc_connected(gfx_input_data_t *gi, bool visible);
void set_window_title_with_grab_info(gfx_input_data_t *gi);

bool console_visible(gfx_input_data_t *gi);

struct gfx_console;
gfx_input_data_t *make_gfx_input(struct gfx_console *gc);
void finalise_gfx_input(gfx_input_data_t *gi);
void destroy_gfx_input(gfx_input_data_t *gi);
void pre_delete_gfx_input(gfx_input_data_t *gi);
void register_gfx_input(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif

#endif

