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

#ifndef TEXTCON_RECORDER_H
#define TEXTCON_RECORDER_H

#include <simics/base/conf-object.h>
#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct text_record_data text_record_data_t;

void console_record_input(text_record_data_t *tr, uint8 value);
void console_record_output(text_record_data_t *tr, uint8 value);
        
text_record_data_t *make_text_recording(text_console_t *tc);
void destroy_text_recording(text_record_data_t *tr);
void register_text_recording(conf_class_t *cl);

#if defined(__cplusplus)
}
#endif
        
#endif
