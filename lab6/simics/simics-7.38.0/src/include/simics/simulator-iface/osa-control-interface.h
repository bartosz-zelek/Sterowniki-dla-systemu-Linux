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

#ifndef SIMICS_SIMULATOR_IFACE_OSA_CONTROL_INTERFACE_H
#define SIMICS_SIMULATOR_IFACE_OSA_CONTROL_INTERFACE_H

#include <simics/device-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        OSA_Request_Error_ID = 0,
} request_id_error_t;


/* <add id="osa_control_interface_t">
   <name>osa_control_interface_t</name>
   <ndx>osa_control_interface_t</ndx>

   <ndx>request!osa_control interface method</ndx> <fun>request</fun> is used
   to register clients that are interested in using the tracker framework and
   activates the tracker framework if it is not already activated. The
   <arg>initiator</arg> argument is a string which should describe the client
   that requests to activate the OS Awareness framework. The return value is an
   ID that should be passed to the <fun>release</fun> function to signal that
   the client no longer needs the OS Awareness framework.

   Upon a failure while initializing the OS Awareness framework,
   OSA_Request_Error_ID will be returned.

   <ndx>release!osa_control interface method</ndx> <fun>release</fun> removes a
   client that has previously requested to use the tracker framework. The
   <arg>id</arg> argument is the returned value from <fun>request</fun>. The
   tracker framework will be disabled when there are no more registered users.

   <insert-until text="// ADD INTERFACE osa_control_interface"/>
   </add>

   <add id="osa_control_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_control) {
        request_id_t (*request)(conf_object_t *NOTNULL obj,
                                const char *initiator);
        void (*release)(conf_object_t *NOTNULL obj, request_id_t id);
};

#define OSA_CONTROL_INTERFACE "osa_control"
// ADD INTERFACE osa_control_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_CONTROL_INTERFACE_H */
