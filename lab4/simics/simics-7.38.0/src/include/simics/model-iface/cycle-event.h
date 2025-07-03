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

#ifndef SIMICS_MODEL_IFACE_CYCLE_EVENT_H
#define SIMICS_MODEL_IFACE_CYCLE_EVENT_H

#include <simics/base/types.h>
#include <simics/base/event.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="cycle_event_interface_t">

   TODO: document the <iface>cycle_event</iface> interface.

   <insert-until text="// ADD INTERFACE cycle_event_interface_t"/>
   </add>

   <add id="cycle_event_interface_exec_context">
   Cell Context for all methods.
   </add> 
*/
SIM_INTERFACE(cycle_event) {
        cycles_t (*cycles)(conf_object_t *NOTNULL obj);
        void (*post)(conf_object_t *NOTNULL obj,
                     const event_class_t *NOTNULL evclass,
                     conf_object_t *NOTNULL ev_obj,
                     cycles_t cycles,
                     lang_void *param);

        void (*cancel)(conf_object_t *NOTNULL obj,
                       const event_class_t *NOTNULL evclass,
                       conf_object_t *NOTNULL ev_obj,
                       int (*pred)(lang_void *data, lang_void *match_data),
                       lang_void *match_data);

        cycles_t (*lookup)(conf_object_t *NOTNULL obj,
                           const event_class_t *NOTNULL evclass,
                           conf_object_t *NOTNULL ev_obj,
                           int (*pred)(lang_void *data, lang_void *match_data),
                           lang_void *match_data);

        attr_value_t (*events)(conf_object_t *NOTNULL obj);
};
#define CYCLE_EVENT_INTERFACE "cycle_event"
// ADD INTERFACE cycle_event_interface_t

/*
   <add id="cycle_control_interface_t">

   The <iface>cycle_control</iface> interface is typically for
   controlling a cycle counter with event posting capabilities.

   The initiator object should call <fun>start</fun> or
   <fun>stop</fun> to start/stop the counting of cycles. And use
   <fun>set_cycle_count</fun> is used to configure the current
   cycle count at the target object.

   <insert-until text="// ADD INTERFACE cycle_control_interface_t"/>
   </add>

   <add id="cycle_control_interface_exec_context">
   Cell Context for all methods.
   </add> 
*/
SIM_INTERFACE(cycle_control) {
        void (*stop)(conf_object_t *NOTNULL obj);
        void (*start)(conf_object_t *NOTNULL obj);
        void (*set_cycle_count)(conf_object_t *NOTNULL obj,
                                cycles_t cycle_count);
};
#define CYCLE_CONTROL_INTERFACE "cycle_control"
// ADD INTERFACE cycle_control_interface_t



#if defined(__cplusplus)
}
#endif

#endif   /* SIMICS_MODEL_IFACE_CYCLE_EVENT_H */
