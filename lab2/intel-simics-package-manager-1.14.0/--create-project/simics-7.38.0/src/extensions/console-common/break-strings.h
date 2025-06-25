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

#ifndef CONSOLE_COMMON_BREAK_STRINGS_H
#define CONSOLE_COMMON_BREAK_STRINGS_H

#include <simics/base/conf-object.h>
#include <simics/simulator-iface/consoles.h>

#if defined(__cplusplus)
extern "C" {
#endif
        
typedef struct console_break_data console_break_data_t;

void console_check_break_strings(console_break_data_t *bd, char value);

int64 bs_add(console_break_data_t *bd, const char *str,
             break_string_cb_t cb, lang_void *arg);
int64 bs_add_regexp(console_break_data_t *bd, const char *str,
                    break_string_cb_t cb, lang_void *arg);
int64 bs_add_single(console_break_data_t *bd, const char *str,
                    break_string_cb_t cb, lang_void *arg);
void bs_remove(console_break_data_t *bd, int64 bp_id);

attr_value_t bs_attr_break_strings(console_break_data_t *bd);

console_break_data_t *make_break_strings(conf_object_t *obj, int log_group);
void destroy_break_strings(console_break_data_t *bd);
void register_break_strings(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif
        
#endif
