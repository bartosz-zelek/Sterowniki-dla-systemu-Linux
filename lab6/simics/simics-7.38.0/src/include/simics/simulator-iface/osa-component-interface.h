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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_COMPONENT_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_COMPONENT_INTERFACE_H

#include <simics/device-api.h>
#include <simics/base/cbdata.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_component_interface_t">
   <name>osa_component_interface_t</name>
   <ndx>osa_component_interface_t</ndx>

   <ndx>get_admin!osa_component interface method</ndx>
   <fun>get_admin</fun> returns the <obj>osa_admin</obj> object associated
   with the component.

   <ndx>get_root_node!osa_component interface method</ndx>
   <fun>get_root_node</fun> returns a maybe_node_id_t with the <arg>valid</arg>
   field set to true and <arg>id</arg> set to the current root node if a root
   node exists. If no root node exists the <arg>valid</arg> field will be set
   to false.

   It is only valid to call <fun>get_admin</fun> or <fun>get_root_node</fun> on
   instantiated components.

   <ndx>notify_tracker!osa_component interface method</ndx>
   <fun>notify_tracker</fun> registers a callback function <arg>cb</arg> that
   will be called when a tracker is added to the component using the
   <cmd>insert-tracker</cmd> command. Returns a cancel id that can be used to
   cancel the callback using <fun>cancel_notify</fun>. It is a one time
   notification and will automatically be canceled once it has been called. The
   <arg>data</arg> argument will be passed on to the callback.

   <ndx>cancel_notify!osa_component interface method</ndx>
   <fun>cancel_notify</fun> cancels a callback made by
   <fun>notify_tracker</fun>.

   <ndx>has_tracker!osa_component interface method</ndx>
   <fun>has_tracker</fun> returns true if the component has a tracker inserted,
   otherwise false.

   <ndx>get_processors!osa_component interface method</ndx>
   <fun>get_processors</fun> returns a list of processors to use for the
   software domain.

   <insert-until text="// ADD INTERFACE osa_component_interface"/>
   </add>

   <add id="osa_component_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_component) {
        conf_object_t *(*get_admin)(conf_object_t *NOTNULL obj);
        maybe_node_id_t (*get_root_node)(conf_object_t *NOTNULL obj);
        cancel_id_t (*notify_tracker)(conf_object_t *NOTNULL obj,
                                      void (*cb)(cbdata_call_t data),
                                      cbdata_register_t data);
        void (*cancel_notify)(conf_object_t *NOTNULL obj,
                              cancel_id_t cancel_id);
        bool (*has_tracker)(conf_object_t *NOTNULL obj);
        attr_value_t (*get_processors)(conf_object_t *NOTNULL obj);
};

#define OSA_COMPONENT_INTERFACE "osa_component"
// ADD INTERFACE osa_component_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_SIMULATOR_IFACE_OSA_COMPONENT_INTERFACE_H */
