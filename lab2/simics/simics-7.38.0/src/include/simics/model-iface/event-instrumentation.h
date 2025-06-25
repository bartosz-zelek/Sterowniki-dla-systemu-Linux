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

#ifndef SIMICS_MODEL_IFACE_EVENT_INSTRUMENTATION_H
#define SIMICS_MODEL_IFACE_EVENT_INSTRUMENTATION_H

#include <simics/pywrap.h>
#include <simics/base/conf-object.h>
#include <simics/base/types.h>
#include <simics/base/event.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct step_instrumentation step_handle_t;
typedef struct clock_instrumentation cycle_handle_t;
        
typedef void (*step_event_cb_t)(conf_object_t *obj, conf_object_t *step_obj,
                                conf_object_t *event_obj, pc_step_t steps,
                                const char *event_class_name,
                                const char *description, attr_value_t value,
                                lang_void *user_data);

/* <add id="step_event_instrumentation_interface_t">
   This interface is internal.
   </add>

   <add id="step_event_instrumentation_interface_exec_context">
   Global Context.
   </add>
*/
SIM_INTERFACE(step_event_instrumentation) {
        step_handle_t *(*register_step_event_cb)(conf_object_t *NOTNULL cpu,
                                                  conf_object_t *connection,
                                                  step_event_cb_t cb,
                                                  lang_void *data);
        void (*remove_step_event_cb)(conf_object_t *cpu, step_handle_t *handle);
};

#define STEP_EVENT_INSTRUMENTATION_INTERFACE \
        "step_event_instrumentation"
// ADD INTERFACE step_event_instrumentation

typedef void (*cycle_event_cb_t)(conf_object_t *obj, conf_object_t *step_obj,
                                 conf_object_t *event_obj, cycles_t cycles,
                                 const char *event_class_name,
                                 const char *description, attr_value_t value,
                                 lang_void *user_data);
        
/* <add id="cycle_event_instrumentation_interface_t">
   This interface is internal.
   </add>

   <add id="cycle_event_instrumentation_interface_exec_context">
   Global Context.
   </add>
*/
SIM_INTERFACE(cycle_event_instrumentation) {
        cycle_handle_t *(*register_cycle_event_cb)(conf_object_t *NOTNULL cpu,
                                                   conf_object_t *connection,
                                                   cycle_event_cb_t cb,
                                                   lang_void *data);
        void (*remove_cycle_event_cb)(conf_object_t *cpu, cycle_handle_t *handle);
};

#define CYCLE_EVENT_INSTRUMENTATION_INTERFACE \
        "cycle_event_instrumentation"
// ADD INTERFACE step_event_instrumentation

#if defined(__cplusplus)
}
#endif


#endif // SIMICS_MODEL_IFACE_EVENT_INSTRUMENTATION_H
