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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_CONTROL_V2_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_CONTROL_V2_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_control_v2_interface_t">
   <name>osa_control_v2_interface_t</name>
   <ndx>osa_control_v2_interface_t</ndx>

   <ndx>request!osa_control_v2 interface method</ndx> <fun>request</fun> is
   used to register clients that are interested in using the tracker framework
   and activates the tracker framework if it is not already activated. The
   <arg>initiator</arg> argument is a string which should describe the client
   that requests to activate the OS Awareness framework. The return format is
   [bi|s]. If the first element is True, then the second element will contain
   the request_id, which can be passed to the <fun>release</fun>
   function to signal that the client no longer needs the OS Awareness
   framework. If the first element is False, the second element will be an
   error message.

   <ndx>release!osa_control_v2 interface method</ndx> <fun>release</fun>
   removes a client that has previously requested to use the tracker
   framework. The <arg>id</arg> argument is the returned value from
   <fun>request</fun>. The tracker framework will be disabled when there are no
   more registered users.

   <ndx>clear_state!osa_control_v2 interface method</ndx>
   <fun>clear_state</fun> can be called to clear the state in the tracker
   framework that could exist after loading a checkpoint. This can only be
   called while the tracker is disabled. The return value will be on the format
   [bi|s]. If the first element is True then clearing state succeeded and the
   second element can be ignored. If the first element is False then clearing
   state failed and the second element will contain a string with a message of
   what went wrong.

   <insert-until text="// ADD INTERFACE osa_control_v2_interface"/>
   </add>

   <add id="osa_control_v2_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_control_v2) {
        attr_value_t (*request)(conf_object_t *NOTNULL obj,
                                const char *initiator);
        void (*release)(conf_object_t *NOTNULL obj, request_id_t id);
        attr_value_t (*clear_state)(conf_object_t *NOTNULL obj);
};

#define OSA_CONTROL_V2_INTERFACE "osa_control_v2"
// ADD INTERFACE osa_control_v2_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_CONTROL_V2_INTERFACE_H */
