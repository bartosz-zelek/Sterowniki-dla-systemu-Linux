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

#ifndef SIMICS_MODEL_IFACE_EXECUTE_H
#define SIMICS_MODEL_IFACE_EXECUTE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="execute_interface_t">

   The <iface>execute</iface> interface is implemented by objects that
   drive a simulation, which is often processor models. The object
   does not have to implement <iface>cycle</iface> or
   <iface>step</iface>.

   An object implementing the <iface>execute</iface> interface must be
   coupled with one object implementing the <iface>cycle</iface>
   interface. It can be the same object that implements the
   <iface>cycle</iface> interface.

   The <fun>run</fun> function is called when the simulator starts or
   restarts the execution.

   By default the Simics scheduler will assume that the object being called in
   with the execute interface also implements the corresponding
   <iface>processor_info</iface> and <iface>step</iface> interfaces.
   If this assumption is incorrect, the implementation of the <fun>run</fun>
   function is responsible for maintaining the simulators view of the current
   objects implementing the <iface>processor_info</iface> and
   <iface>step</iface> interfaces. It does that by using the appropriate
   functions in the <iface>cell_inspection</iface> interface. The current
   objects must always be correctly set when either the <fun>run</fun> function
   returns, when any API method is called, or when any other object is called
   through an interface. Several Simics features, such as CLI commands, device
   logging, and hap handling make use of the current objects.

   To handle asynchronous events, and thus allow for reasonable interactive
   performance, the implementor of <iface>execute</iface> needs to either make
   sure that <fun>run</fun> returns after not having run for too long, or
   preferably regularly call the <fun>VT_check_async_events</fun> method. In
   the Simics library CPU models, <fun>VT_check_async_events</fun> is called
   after servicing events from the <iface>cycle</iface> or <iface>step</iface>
   interfaces.

   The simulator core will call <fun>stop</fun> when it detects a
   condition that should interrupt the simulation. The callee should
   stop as soon as possible when in a stable state, typically when the
   current executing instruction is finished after getting a request
   to stop. In some cases the callee might receive multiple stop
   requests in a rapid sequence. Conditions leading to a stop request
   include <fun>SIM_break_simulation</fun> being called from a device
   or hap-handler, breakpoint triggered, low-memory situations, the
   user interrupting the simulation with Ctrl-C, and the Simics core
   halting the object when it is at the end of the allowed time window
   in temporal decoupled simulation. It is forbidden to do anything in
   the <fun>stop</fun> function that can lead to a new stop request,
   this includes posting events, printing <fun>SIM_log</fun>-messages,
   etc. Before returning from the <fun>run</fun> method, the
   <fun>VT_stop_event_processing</fun> function should be called. The
   requirement to call <fun>VT_stop_event_processing</fun> is likely
   to be lifted in future versions of Simics.

   The <fun>switch_in</fun> function is called whenever the
   execute object is about to gain control of the simulation
   from some other execute object in the cell. Similarly,
   <fun>switch_out</fun> is invoked before control is relinquished.
   It should be noted that these functions are called in a
   deterministic manner which is not true for <fun>run</fun>.

   The <fun>switch_in</fun> and <fun>switch_out</fun> functions
   are not called at simulation start (or checkpoint load), in
   general.

   <insert-until text="// ADD INTERFACE execute_interface_t"/>

   </add>
   <add id="execute_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(execute) {
        void (*run)(conf_object_t *obj);
        void (*stop)(conf_object_t *obj);

        void (*switch_in)(conf_object_t *obj);
        void (*switch_out)(conf_object_t *obj);
};

#define EXECUTE_INTERFACE "execute"
// ADD INTERFACE execute_interface_t

/*
   <add id="cell_inspection_interface_t">

   The <iface>cell_inspection</iface> interface is implemented by
   objects of the cell class. It is used by objects implementing the
   <iface>execute</iface> interface to update the currently executing
   objects when control is transferred outside of the execute object.

   The current object implementing <iface>processor_info</iface> is set with
   <fun>set_current_processor_obj()</fun>. Similarly, the current object
   implementing <iface>step</iface> is set with
   <fun>set_current_step_obj()</fun>.

   <insert-until text="// ADD INTERFACE cell_inspection_interface_t"/>
   </add>
   <add id="cell_inspection_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(cell_inspection) {
        void (*set_current_processor_obj)(conf_object_t *obj,
                                          conf_object_t *cpu_obj);
        void (*set_current_step_obj)(conf_object_t *obj,
                                     conf_object_t *step_obj);
};

#define CELL_INSPECTION_INTERFACE "cell_inspection"
// ADD INTERFACE cell_inspection_interface_t


/*
   <add id="execute_control_interface_t">

   The <iface>execute_control</iface> interface is implemented by CPUs
   and devices that support threading.

   Warning: This interface is currently considered tech-preview and
   can be changed at any time.

   <insert-until text="// ADD INTERFACE execute_control_interface_t"/>
   </add>

   <add id="execute_control_interface_exec_context">
   Threaded Context
   </add>
*/
SIM_INTERFACE(execute_control) {
        void (*message_pending)(conf_object_t *obj);
        void (*yield_request)(conf_object_t *obj);
};
#define EXECUTE_CONTROL_INTERFACE "execute_control"
// ADD INTERFACE execute_control_interface_t


#if defined(__cplusplus)
}
#endif

#endif
