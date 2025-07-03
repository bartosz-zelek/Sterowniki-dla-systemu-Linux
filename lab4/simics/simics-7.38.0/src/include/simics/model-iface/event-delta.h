/*
  © 2011 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_EVENT_DELTA_H
#define SIMICS_MODEL_IFACE_EVENT_DELTA_H

#include <simics/base/types.h>
#include <simics/base/event.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="event_delta_interface_t">

   The <iface>event_delta</iface> interface is implemented by CPUs and
   clocks that advance time.

   The <fun>set_delta</fun> method notifies the CPU about
   the number of cycles the CPU should run before the next event
   should be dispatched by a call to the <fun>handle_event</fun>
   method of the <iface>event_handler</iface> interface.
   
   The <fun>get_delta</fun> queries the CPU for the current count
   of remaning cycles to the next event. The returned number should
   always be smaller or equal to the number of cycles established by
   a previous call to <fun>set_delta</fun>.

   <insert-until text="// ADD INTERFACE event_delta_interface_t"/>
   </add>

   <add id="event_delta_interface_exec_context">
   Cell Context for all methods.
   </add> 
*/
SIM_INTERFACE(event_delta) {
        uint64 (*set_delta)(conf_object_t *NOTNULL obj,
                            conf_object_t *NOTNULL event_handler_obj,
                            const event_class_t *next_event_ec,
                            uint64 delta);
        uint64 (*get_delta)(conf_object_t *NOTNULL obj,
                            conf_object_t *NOTNULL event_handler_obj);
};
#define EVENT_DELTA_INTERFACE "event_delta"
// ADD INTERFACE event_delta_interface_t


/*
   <add id="event_handler_interface_t">

   The <iface>event_handler</iface> interface is implemented by
   the <tt>vtime</tt> port object and is invoked by clocks or CPUs
   implementing the <iface>event_delta</iface> interface.

   The <fun>handle_event</fun> method should be called when the number
   of cycles to the next event has reached zero. The method
   invokes the next event and notifies the CPU or clock about the cycle count
   to the next pending event by invoking the <fun>set_delta</fun> method.

   The <fun>stop</fun> method should be called if a dispatched
   event requests the simulation to be stopped. The method is typically
   called from the <fun>stop</fun> method of the <iface>execute</iface>
   interface. If the <fun>stop</fun> method is not called, then time may be
   advanced by a fraction of a cycle after an event has been dispatched.

   <insert-until text="// ADD INTERFACE event_handler_interface_t"/>
   </add>

   <add id="event_handler_interface_exec_context">
   Cell Context for all methods.
   </add> 
*/
SIM_INTERFACE(event_handler) {
        bool (*handle_event)(conf_object_t *NOTNULL obj);
        void (*stop)(conf_object_t *NOTNULL obj);
};
#define EVENT_HANDLER_INTERFACE "event_handler"
// ADD INTERFACE event_handler_interface_t


#if defined(__cplusplus)
}
#endif

#endif   /* SIMICS_MODEL_IFACE_EVENT_DELTA_H */
