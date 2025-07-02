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

#ifndef GRAPHCON_GFX_CONSOLE_H
#define GRAPHCON_GFX_CONSOLE_H

#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define GFX_CONSOLE_STR SYMBOL_TO_STRING(GFX_CONSOLE)
        
// Main graphics console object data structure.
typedef struct gfx_console {
        conf_object_t obj;

        struct gfx_input_data *input_data;
        struct gfx_record_data *record_data;
        struct gfx_output_data *output_data;
        struct gfx_break_data *gfx_break_data;
        struct console_break_data *break_str_data;
        struct vnc_console *vnc_data;
} gfx_console_t;

// Log group definitions.        
enum {
        Gfx_Console_Log_Main      = 1 << 0,
        Gfx_Console_Log_Input     = 1 << 1,
        Gfx_Console_Log_Record    = 1 << 2,
        Gfx_Console_Log_Output    = 1 << 3,
        Gfx_Console_Log_Break     = 1 << 4,
        Gfx_Console_Log_VNC       = 1 << 5,
        Gfx_Console_Log_Break_Str = 1 << 6,
        Gfx_Console_Log_Num       = 7,
};

conf_object_t *get_gfx_console_frontend(conf_object_t *obj);

#if defined(__cplusplus)
}
#endif

#endif
