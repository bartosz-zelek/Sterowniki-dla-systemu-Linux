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

#ifndef CONSOLE_COMMON_CONSOLE_FRONTEND_H
#define CONSOLE_COMMON_CONSOLE_FRONTEND_H

#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

void init_console_frontends();
void register_console_frontend(const char *console, const char *name,
                               conf_class_t *frontend);

conf_object_t *get_console_frontend(conf_object_t *obj, const char *port);
conf_class_t *get_console_frontend_class(conf_object_t *obj,
                                         const char *console);

#if defined(__cplusplus)
}
#endif
        
#endif
