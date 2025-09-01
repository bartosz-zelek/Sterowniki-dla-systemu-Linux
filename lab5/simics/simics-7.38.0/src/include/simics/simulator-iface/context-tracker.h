/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_CONTEXT_TRACKER_H
#define SIMICS_SIMULATOR_IFACE_CONTEXT_TRACKER_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="context_handler_interface_t">
   <note>This interface is not supported, and may change in the future.</note>

   Get and set current context. The <fun>set_current_context</fun> function
   returns zero if the passed object is not of the context class, otherwise
   one is returned.

   <insert-until text="// ADD INTERFACE context_handler_interface"/>
   </add>
   <add id="context_handler_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(context_handler) {
        conf_object_t *(*get_current_context)(conf_object_t *obj);
        int (*set_current_context)(conf_object_t *obj, conf_object_t *ctx);
};

#define CONTEXT_HANDLER_INTERFACE "context_handler"
// ADD INTERFACE context_handler_interface

#if defined(__cplusplus)
}
#endif

#endif
