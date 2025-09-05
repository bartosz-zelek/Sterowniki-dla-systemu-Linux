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

#ifndef SIMICS_SIMULATOR_IFACE_FOLLOWER_H
#define SIMICS_SIMULATOR_IFACE_FOLLOWER_H

#include <simics/simulator/follower-time.h>
#include <simics/base/conf-object.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="follower_agent_interface_t">

   This interface is intended to be implemented by a follower agent to accept
   data from the follower.

   The <fun>accept</fun> method delivers a deterministic message from the
   follower to the agent.

   The <fun>accept_async</fun> method delivers an asynchronous
   (non-deterministic) message from the follower to the agent.

   <insert-until text="// ADD INTERFACE follower_agent_interface"/>
   </add>
   <add id="follower_agent_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(follower_agent) {
        void (*accept)(conf_object_t *obj, bytes_t msg);
        void (*accept_async)(conf_object_t *obj, bytes_t msg);
};
#define FOLLOWER_AGENT_INTERFACE "follower_agent"
// ADD INTERFACE follower_agent_interface

/* <add id="leader_message_interface_t">

   This interface is implemented by the leader, and is used from a follower
   agent to send data to the follower.

   The <fun>send</fun> method sends a deterministic message to the follower to
   be delivered at <param>time</param>. If several messages will be delivered at
   same time, an increasing <param>skey</param> determines the sort order.

   The <fun>send_async</fun> method sends a non-deterministic (asynchronous)
   message to the follower to be delivered as soon as possible.

   <insert-until text="// ADD INTERFACE leader_message_interface"/>
   </add>
   <add id="leader_message_interface_exec_context">
     Cell Context for all methods.
   </add> */
SIM_INTERFACE(leader_message) {
        void (*send)(conf_object_t *obj,
                     follower_time_t time, uint64 skey, bytes_t msg);
        void (*send_async)(conf_object_t *obj, bytes_t msg);
};
#define LEADER_MESSAGE_INTERFACE "leader_message"
// ADD INTERFACE leader_message_interface

#if defined(__cplusplus)
}
#endif

#endif
