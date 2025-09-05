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

#ifndef TEXTCON_VT100_H
#define TEXTCON_VT100_H

#include <simics/base-types.h>
#include <simics/base/conf-object.h>
#include <simics/simulator-iface/consoles.h>
#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct vt100_data vt100_data_t;

void vt100_output(vt100_data_t *vt, uint8 value);
void vt100_input(vt100_data_t *vt, text_console_key_t key,
                 text_console_modifier_t modifiers);
void screen_size_change(vt100_data_t *vt, int old_rows, int new_rows);

void get_cursor_pos(vt100_data_t *vt, int *x, int *y);
void get_vt100_reset_string(vt100_data_t *vt, strbuf_t *buf);
void get_vt100_state_string(vt100_data_t *vt, strbuf_t *buf);

vt100_data_t *make_vt100(text_console_t *tc);
void finalise_vt100(vt100_data_t *vt);
void destroy_vt100(vt100_data_t *vt);
void register_vt100(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif
        
#endif

