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

#ifndef GRAPHCON_RECORDER_H
#define GRAPHCON_RECORDER_H

#include <simics/base/conf-object.h>
#include <simics/model-iface/sim-keys.h>
#include <simics/simulator-iface/consoles.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct gfx_record_data gfx_record_data_t;

void gfx_record_kbd_input(gfx_record_data_t *gr, sim_key_t code, bool down);
void gfx_record_mouse_input(gfx_record_data_t *gr,
                            int x, int y, int z,
                            gfx_console_mouse_button_t buttons);

struct gfx_console;
gfx_record_data_t *make_gfx_recording(struct gfx_console *gc);
void destroy_gfx_recording(gfx_record_data_t *gr);
void register_gfx_recording(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif

#endif
