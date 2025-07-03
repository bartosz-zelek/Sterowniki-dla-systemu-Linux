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

#ifndef TEXTCON_DRIVER_H
#define TEXTCON_DRIVER_H

#include <simics/base/conf-object.h>
#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct console_driver_data console_driver_data_t;

void add_input(console_driver_data_t *cd, uint8 value);
void console_input(console_driver_data_t *cd, char value);
void console_input_str(console_driver_data_t *cd, const char *str);
void console_visible(console_driver_data_t *cd);

console_driver_data_t *make_driver(text_console_t *tc);
void finalise_driver(console_driver_data_t *cd);
void destroy_driver(console_driver_data_t *cd);
void register_driver(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif
        
#endif

