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

#ifndef SIMICS_MODEL_IFACE_TEMPORAL_STATE_H
#define SIMICS_MODEL_IFACE_TEMPORAL_STATE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(SIMICS_6_API) || defined(SHOW_OBSOLETE_API)

/* <add id="temporal_state_interface_t">
   Deprecated interface. Use global notifiers and/or attributes instead.

   This interface was used to save micro checkpoints for reverse execution.
   It remains in 7, but is only used to get notifications when an
   object's state is about to be restored, or has just been restored.

   All functions in this interface are optional.
   
   The <fun>save</fun> and <fun>merge</fun> functions are never called.

   The function <fun>prepare_restore</fun> is called when a saved state
   is about to be loaded, before any attributes have been set.

   The <fun>finish_restore</fun> function is called when all
   attributes have been set. The <arg>state</arg> argument is
   always NULL.

   <insert-until text="// ADD INTERFACE temporal_state_interface"/>
   </add>
   <add id="temporal_state_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(temporal_state) {
        lang_void *(*save)(conf_object_t *obj);
        void (*merge)(conf_object_t *obj, lang_void *prev, lang_void *killed);
        void (*prepare_restore)(conf_object_t *obj);
        void (*finish_restore)(conf_object_t *obj, lang_void *state);
};

#define TEMPORAL_STATE_INTERFACE "temporal_state"
// ADD INTERFACE temporal_state_interface

#endif /* 6 or SHOW_OBSOLETE_API */

#if defined(__cplusplus)
}
#endif

#endif
