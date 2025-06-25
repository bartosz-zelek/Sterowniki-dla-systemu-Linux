/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_TRACKER_COMPONENT_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_TRACKER_COMPONENT_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_tracker_component_interface_t">
   <name>osa_tracker_component_interface_t</name>
   <ndx>osa_tracker_component_interface_t</ndx>

   <ndx>get_tracker!osa_tracker_component interface method</ndx>
   <fun>get_tracker</fun> returns the tracker object associated with the
   component or nil if no such tracker exists.

   <ndx>get_tracker!osa_tracker_component interface method</ndx>
   <fun>get_mapper</fun> returns the mapper object associated with the
   component or nil if no such mapper exists.

   <insert-until text="// ADD INTERFACE osa_tracker_component_interface"/>
   </add>

   <add id="osa_tracker_component_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_tracker_component) {
        conf_object_t *(*get_tracker)(conf_object_t *NOTNULL obj);
        conf_object_t *(*get_mapper)(conf_object_t *NOTNULL obj);
};

#define OSA_TRACKER_COMPONENT_INTERFACE "osa_tracker_component"
// ADD INTERFACE osa_tracker_component_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_SIMULATOR_IFACE_OSA_TRACKER_COMPONENT_INTERFACE_H */
