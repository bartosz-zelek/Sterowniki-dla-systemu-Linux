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

#ifndef SIMICS_DEVS_FREQUENCY_H
#define SIMICS_DEVS_FREQUENCY_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="frequency_interface_t">

   The <iface>frequency</iface> interface is used to publish
   a frequency to other objects.

   The <fun>get</fun> method returns the current frequency.

   The object implementing this interface is expected to implement
   the <tt>Sim_Notify_Frequency_Change</tt> notifier, which should be
   raised when the object changes its frequency. Objects subscribing
   to this notifier can acquire the new frequency by doing a new call
   to the <fun>get</fun> method.

   <insert-until text="// ADD INTERFACE frequency_interface_t"/>
   </add>

   <add id="frequency_interface_exec_context">
   Cell Context for all methods.
   </add> 
*/
SIM_INTERFACE(frequency) {
        double (*get)(conf_object_t *NOTNULL obj);
};
#define FREQUENCY_INTERFACE "frequency"
// ADD INTERFACE frequency_interface_t


/* <add id="frequency_listener_interface_t">

   The <iface>frequency_listener</iface> interface is used for
   modeling frequency changes.

   The frequency change initiator should call <fun>set()</fun> to set the
   new frequency. The <fun>set()</fun> function may be called multiple times
   with the same frequency value. The frequency is specified by the
   <tt>numerator</tt> and <tt>denominator</tt> arguments in units of Hz.

   The <fun>set()</fun> function should also be used to set the initial
   value for a target.

   An object that implements the <iface>frequency_listener</iface>
   interface typically has an attribute that refers to an object and
   port that implements the <iface>simple_dispatcher</iface>
   interface, from where the frequency originates.

   The initiator of a frequency change should implement the
   <iface>simple_dispatcher</iface> interface, and it should only call
   the <fun>set()</fun> function on objects that have registered with
   this interface.

   <insert-until text="// ADD INTERFACE frequency_listener_interface"/>
   </add>
   <add id="frequency_listener_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(frequency_listener) {
        void (*set)(conf_object_t *NOTNULL obj, uint64 numerator,
                    uint64 denominator);
};

#define FREQUENCY_LISTENER_INTERFACE "frequency_listener"
// ADD INTERFACE frequency_listener_interface

/* <add id="scale_factor_listener_interface_t">

   The <iface>scale_factor_listener</iface> interface is used for
   modeling changes in scale factors. It is mainly used by the
   <obj>frequency_bus</obj> device, to allow an external device to
   affect how much the bus scales its input frequency.

   The <iface>scale_factor_listener</iface> has exactly the same
   semantics as the <iface>frequency_listener</iface> interface, the
   two interfaces only differ in convention: The parameters to
   <fun>scale_factor_listener.set()</fun> are typically a scale factor
   rather close to 1, while the parameters of
   <fun>frequency_listener</fun> represent a base frequency in Hz,
   which is typically significantly larger than 1000000.

   See the documentation on the <nref
   label="__rm_interface_frequency_listener">frequency_listener</nref>
   interface for more information.

   <insert-until text="// ADD INTERFACE scale_factor_listener_interface"/>
   </add>
   <add id="scale_factor_listener_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(scale_factor_listener) {
        void (*set)(conf_object_t *NOTNULL obj, uint64 numerator,
                    uint64 denominator);
};

#define SCALE_FACTOR_LISTENER_INTERFACE "scale_factor_listener"
// ADD INTERFACE scale_factor_listener_interface

/* <add id="simple_dispatcher_interface_t">

   The <iface>simple_dispatcher</iface> interface is used for
   subscribing to changes of some monitored state. The interface is
   currently used for subscribing to changes in frequencies and to
   changes in frequency scale factors, via the interfaces
   <iface>frequency_listener</iface> and
   <iface>scale_factor_listener</iface>, respectively.

   A device subscribes with the <fun>subscribe</fun> function.  Before
   this function returns, it should send a signal to the calling
   object, telling the current state. After this, signals should be
   sent whenever the monitored state changes.

   The function <fun>unsubscribe</fun> should be used to unsubscribe
   from monitored changes. A listener may not subscribe or unsubscribe
   twice on the same port.

   <insert-until text="// ADD INTERFACE simple_dispatcher_interface"/>
   </add>
   <add id="simple_dispatcher_interface_exec_context">
   Global Context for all methods.
   </add> */
SIM_INTERFACE(simple_dispatcher) {
        void (*subscribe)(conf_object_t *NOTNULL bus,
                          conf_object_t *NOTNULL listener,
                          const char *listener_port);
        void (*unsubscribe)(conf_object_t *NOTNULL bus,
                            conf_object_t *NOTNULL listener,
                            const char *listener_port);
};

#define SIMPLE_DISPATCHER_INTERFACE "simple_dispatcher"
// ADD INTERFACE simple_dispatcher_interface

#if defined(__cplusplus)
}
#endif

#endif
