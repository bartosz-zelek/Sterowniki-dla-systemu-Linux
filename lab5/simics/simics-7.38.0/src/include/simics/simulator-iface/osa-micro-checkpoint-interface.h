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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_MICRO_CHECKPOINT_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_MICRO_CHECKPOINT_INTERFACE_H

#include <simics/device-api.h>
#include <simics/base/cbdata.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_micro_checkpoint_interface_t">
   <name>osa_micro_checkpoint_interface_t</name>
   <ndx>osa_micro_checkpoint_interface_t</ndx>

   This interface is used to get information when micro checkpointing (for
   reverse execution) starts and stops. The functions will only be called
   when the tracker framework is enabled.

   <ndx>started!osa_micro_checkpoint interface method</ndx>
   <fun>started</fun> is called when a saved state is about to be loaded,
   before any attributes have been set.

   <ndx>finished!osa_micro_checkpoint interface method</ndx>
   <fun>finished</fun> is called once all attributes have been set for all
   objects. At this point callbacks calls to the machine interfaces can be
   done.

   It is allowed to implement only one of these functions if notification are
   only wanted before or after setting attributes of a micro checkpoint.

   This interface is optional and can be implemented by either a tracker or a
   mapper.

   <insert-until text="// ADD INTERFACE osa_micro_checkpoint_interface"/>
   </add>

   <add id="osa_micro_checkpoint_interface_exec_context">
   Global Context for all methods.
   </add>

*/

SIM_INTERFACE(osa_micro_checkpoint) {
        void (*started)(conf_object_t *NOTNULL obj);
        void (*finished)(conf_object_t *NOTNULL obj);
};

#define OSA_MICRO_CHECKPOINT_INTERFACE "osa_micro_checkpoint"
// ADD INTERFACE osa_micro_checkpoint_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_SIMULATOR_IFACE_OSA_MICRO_CHECKPOINT_INTERFACE_H */
