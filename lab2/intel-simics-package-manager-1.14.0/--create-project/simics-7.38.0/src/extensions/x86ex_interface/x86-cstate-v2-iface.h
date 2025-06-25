/*
  C-state change notification interface version 2

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef X86_CSTATE_V2_IFACE_H
#define X86_CSTATE_V2_IFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

/* <add-type id="x86_cstate_event_t"> </add-type> */
/* Enumeration of all supported reasons for a C-state change */
typedef enum {
        X86_Cstate_Event_Unspecified,
        X86_Cstate_Event_Hlt,
        X86_Cstate_Event_Mwait,
        X86_Cstate_Event_External,
        X86_Cstate_Event_Wakeup,
        X86_Cstate_Event_MonitorWakeup,
        X86_Cstate_Event_IntrWakeup,
        X86_Cstate_Event_MwaitTimerWakeup,
        X86_Cstate_Event_Max
} x86_cstate_event_t;


/* <add-type id="x86_cstate_notify_info_t"> </add-type> */
typedef struct {
        x86_cstate_event_t  event;
        uint32              state;
        uint32              sub_state;
} x86_cstate_notify_info_t;
SIM_PY_ALLOCATABLE(x86_cstate_notify_info_t);


/* <add id="x86_cstate_notification_v2">
   The interface is used to notify the <arg>listener</arg> about the processor
   C-state transitions.

   The <arg>info</arg> contains information about the source of change and
   current C-state of the processor <arg>cpu</arg>.

   A state value of 0 corresponds to C0, a value of 1 corresponds to C1, etc.
   <b>HLT</b> is reported as state 1, substate 0.
   <b>MWAIT</b> is reported based upon the eax hint,
   decoded as state = (eax[7:4] + 1) mod 16, substate = eax[3:0].

   <insert-until text="// ADD INTERFACE x86_cstate_notification_v2"/>
   </add>*/
SIM_INTERFACE(x86_cstate_notification_v2) {
        void (*notification)(conf_object_t *listener, conf_object_t *cpu,
                const x86_cstate_notify_info_t *info);
};

#define X86_CSTATE_NOTIFICATION_V2_INTERFACE "x86_cstate_notification_v2"
// ADD INTERFACE x86_cstate_notification_v2

#ifdef __cplusplus
}
#endif

#endif /* ! X86_CSTATE_V2_IFACE_H */
