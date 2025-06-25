/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_TRACKER_CONTROL_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_TRACKER_CONTROL_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_tracker_control_interface_t">
   <name>osa_tracker_control_interface_t</name>
   <ndx>osa_tracker_control_interface_t</ndx>

   <ndx>enable!osa_tracker_control interface method</ndx>
   <fun>enable</fun> and
   <ndx>disable!osa_tracker_control interface method</ndx>
   <fun>disable</fun> are called from the OS Awareness framework,
   for all trackers that have been set in the <attr>top_trackers</attr>
   attribute for the <obj>node_tree</obj> object, when OS Awareness is
   enabled or disabled, respectively.

   <ndx>clear_state!osa_tracker_control interface method</ndx>
   <fun>clear_state</fun> is called to clear the trackers state. The tracker
   should clear all its internal data and its data in the node tree when this
   is called.  This call can only occur while the tracker is disabled.

   <ndx>add_processor!osa_tracker_control interface method</ndx>
   <fun>add_processor</fun> and
   <ndx>remove_processor!osa_tracker_control interface method</ndx>
   <fun>remove_processor</fun> are called to add or remove a processor
   <arg>cpu</arg> to/from the tracker which the tracker should start/stop
   tracking. If the tracker is registered as a top level tracker these methods
   will be called by the OS Awareness framework for all processors available
   to the framework when it is enabled or disabled. If the tracker is a guest
   under a hypervisor the hypervisor should call these methods when a
   processor becomes available or unavailable to the guest.
   These functions should return true if a processor was successfully added or
   removed, otherwise the function should return false.

   <insert-until text="// ADD INTERFACE osa_tracker_control_interface"/>
   </add>

   <add id="osa_tracker_control_interface_exec_context">
   Global Context for <fun>enable</fun>, <fun>disable</fun> and
   <fun>clear_state</fun>.
   Cell Context for <fun>add_processor</fun> and
   <fun>remove_processor</fun>.
   </add>
*/
SIM_INTERFACE(osa_tracker_control) {
        void (*disable)(conf_object_t *NOTNULL obj);
        bool (*enable)(conf_object_t *NOTNULL obj);
        void (*clear_state)(conf_object_t *NOTNULL obj);
        bool (*add_processor)(conf_object_t *NOTNULL obj,
                              conf_object_t *NOTNULL cpu);
        bool (*remove_processor)(conf_object_t *NOTNULL obj,
                                 conf_object_t *NOTNULL cpu);
};

#define OSA_TRACKER_CONTROL_INTERFACE "osa_tracker_control"
// ADD INTERFACE osa_tracker_control_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_TRACKER_CONTROL_INTERFACE_H */
