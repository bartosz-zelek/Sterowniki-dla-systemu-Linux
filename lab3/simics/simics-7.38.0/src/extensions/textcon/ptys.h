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

#ifndef TEXTCON_PTYS_H
#define TEXTCON_PTYS_H

#include <simics/base/conf-object.h>
#include "text-console.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Host-serial data structure is mostly dependent on platform.        
typedef struct host_serial_data {
        text_console_t *tc;   /* Dubious shortcut to containing struct. */

        // Either name of pty (Linux) or COM port (Windows).
        char *pty_name;
        // Platform dependent host-serial data.
        struct pty *pty_data;
} host_serial_data_t;

bool host_serial_setup(conf_object_t *obj, const char *name);
void host_serial_shutdown(conf_object_t *obj);

struct pty *init_pty(host_serial_data_t *hs);
void destroy_pty(struct pty *pty);

#if defined(__cplusplus)
}
#endif
        
#endif /* TEXTCON_PTYS_H */
