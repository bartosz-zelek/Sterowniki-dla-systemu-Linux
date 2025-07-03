/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TEXTCON_HOST_SERIAL_H
#define TEXTCON_HOST_SERIAL_H

#include <simics/base/conf-object.h>
#include "text-console.h"
#include "ptys.h"

#if defined(__cplusplus)
extern "C" {
#endif

void host_serial_write(host_serial_data_t *hs, uint8 value);

host_serial_data_t *make_host_serial(text_console_t *tc);
void destroy_host_serial(host_serial_data_t *hs);
void register_host_serial(conf_class_t *cl);

bool host_serial_connected(text_console_t *tc);

#if defined(__cplusplus)
}
#endif
        
#endif /* TEXTCON_HOST_SERIAL_H */
