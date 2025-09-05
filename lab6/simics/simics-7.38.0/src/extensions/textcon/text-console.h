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

#ifndef TEXTCON_TEXT_CONSOLE_H
#define TEXTCON_TEXT_CONSOLE_H

#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

// Main text console object data structure.
typedef struct {
        conf_object_t obj;

        struct console_driver_data *driver_data;
        struct text_record_data *record_data;
        struct console_break_data *break_data;
        struct screen *screen;
        struct vt100_data *vt100_data;
        struct telnet_data *telnet_data;
        struct host_serial_data *host_serial_data;
} text_console_t;

// Log group definitions.        
enum {
        Text_Console_Log_Main    = 1 << 0,
        Text_Console_Log_Break   = 1 << 1,
        Text_Console_Log_Record  = 1 << 2,
        Text_Console_Log_VT100   = 1 << 3,
        Text_Console_Log_Screen  = 1 << 4,
        Text_Console_Log_Telnet  = 1 << 5,
        Text_Console_Log_Pty     = 1 << 6,
        Text_Console_Log_Output  = 1 << 7,
        Text_Console_Log_Num     = 8,
};

conf_object_t *get_text_console_frontend(conf_object_t *obj);

#if defined(__cplusplus)
}
#endif
        
#endif
