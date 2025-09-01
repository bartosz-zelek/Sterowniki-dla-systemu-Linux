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

#ifndef SIMICS_DEVS_X86_CSTATE_IFACE_H
#define SIMICS_DEVS_X86_CSTATE_IFACE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add id="x86_cstate_interface_t">

   The methods in this interface can be used to read or change the current
   power state the CPU is in. A state value of 0 corresponds to C0, a value of
   1 corresponds to C1, etc. HLT will be reported as state 1, substate 0.
   MWAIT will reported based upon the eax hint, decoded as state = (eax[7:4] +
   1) mod 16, substate = eax[3:0].

   <fun>set_cstate</fun> will perform side-effects such as putting the
   processor to sleep or waking it up, and call the registered cstate
   listeners.

   <insert-until text="// ADD INTERFACE x86_cstate_interface"/>

   </add>
   <add id="x86_cstate_interface_exec_context">
   Cell Context for all methods.
   </add> */

typedef struct {
        uint32 state;
        uint32 sub_state;
} x86_cstate_t;

SIM_INTERFACE(x86_cstate) {
        x86_cstate_t (*get_cstate)(conf_object_t *cpu_obj);
        void (*set_cstate)(conf_object_t *cpu_obj,
                           uint32 state, uint32 sub_state);
};

#define X86_CSTATE_INTERFACE "x86_cstate"
// ADD INTERFACE x86_cstate_interface

/* <add id="x86_pkg_cstate_interface_t">

   The methods in this interface can be used to read or change the current
   power state the CPU is in. A state value of 0 corresponds to C0, a value of
   1 corresponds to C1, etc. HLT will be reported as state 1, substate 0.
   MWAIT will reported based upon the eax hint, decoded as state = (eax[7:4] +
   1) mod 16, substate = eax[3:0].

   <fun>pkg_cstate_update</fun> will perform side-effects such as putting the
   processor to sleep or waking it up, and call the cstate notification
   listeners.

   <insert-until text="// ADD INTERFACE x86_pkg_cstate_interface"/>

   </add>
   <add id="x86_pkg_cstate_interface_exec_context">
   Cell Context for all methods.
   </add> */

SIM_INTERFACE(x86_pkg_cstate) {
        x86_cstate_t (*get_pkg_cstate)(conf_object_t *cpu_obj);
        void (*set_pkg_cstate)(conf_object_t *cpu_obj,
                           uint32 state, uint32 sub_state);
        void (*pkg_cstate_update)(conf_object_t *cpu_obj,
                           bool notify);
};

#define X86_PKG_CSTATE_INTERFACE "x86_pkg_cstate"
// ADD INTERFACE x86_pkg_cstate_interface

/* <add id="x86_cstate_notification_interface_t">

   Objects registered in the CPU's <attr>cstate_listeners</attr> attribute will
   be called via the <fun>notification</fun> method whenever the CPU's cstate
   changes. Only changes caused by instruction execution or calls to the
   <fun>x86_cstate.set_cstate</fun> function will trigger a notification, not
   attribute accesses. See the <iface>x86_cstate</iface> interface for how
   the C-state is encoded in the parameters.

   <insert-until text="// ADD INTERFACE x86_cstate_notification_interface"/>

   </add>
   <add id="x86_cstate_notification_interface_exec_context">
   Cell Context.
   </add> */

SIM_INTERFACE(x86_cstate_notification) {
        void (*notification)(conf_object_t *listener, conf_object_t *cpu,
                             uint32 state, uint32 sub_state);
};

#define X86_CSTATE_NOTIFICATION_INTERFACE "x86_cstate_notification"
// ADD INTERFACE x86_cstate_notification_interface

#define X86_CSTATE_CHANGE_NOTIFIER "x86-cstate-change"

#ifdef __cplusplus
}
#endif

#endif /* ! SIMICS_DEVS_X86_CSTATE_IFACE_H */
