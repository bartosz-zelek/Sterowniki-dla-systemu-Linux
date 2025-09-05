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

#ifndef GRAPHCON_VNC_H
#define GRAPHCON_VNC_H

#include <simics/base/conf-object.h>
#include "rect.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct vnc_console vnc_console_t;

void vnc_screen_rect_dirty(vnc_console_t *vc, gfx_rect_t dirty);
void vnc_console_set_screen(vnc_console_t *vc, const uint32 *src,
                            int width, int height);

struct gfx_console;
vnc_console_t *make_gfx_vnc(struct gfx_console *gc);
void destroy_gfx_vnc(vnc_console_t *vc);
void register_gfx_vnc(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif

#endif

