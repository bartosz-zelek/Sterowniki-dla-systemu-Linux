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

#ifndef SIMICS_MODEL_IFACE_CO_EXECUTE_H
#define SIMICS_MODEL_IFACE_CO_EXECUTE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="co_execute_interface_t">

   Experimental, may change without notice.

   <insert-until text="// ADD INTERFACE co_execute_interface_t"/>

   </add>
   <add id="co_execute_interface_exec_context">Cell Context</add>
*/
SIM_INTERFACE(co_execute) {
        void (*start_thread)(conf_object_t *NOTNULL obj,
                             void (*entry)(lang_void *arg), lang_void *arg);
        void (*yield)(conf_object_t *NOTNULL obj);
};
#define CO_EXECUTE_INTERFACE "co_execute"
// ADD INTERFACE co_execute_interface_t

/*
   <add id="synchronous_mode_interface_t">

   Experimental, may change without notice.

   <insert-until text="// ADD INTERFACE synchronous_mode_interface_t"/>

   </add>
   <add id="synchronous_mode_interface_exec_context">Cell Context</add>
*/
SIM_INTERFACE(synchronous_mode) {
        int (*enter)(conf_object_t *NOTNULL obj);
        int (*leave)(conf_object_t *NOTNULL obj);
};
#define SYNCHRONOUS_MODE_INTERFACE "synchronous_mode"
// ADD INTERFACE synchronous_mode_interface_t

#if defined(__cplusplus)
}
#endif

#endif
