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

#ifndef SIMICS_DEVS_SIGNAL_H
#define SIMICS_DEVS_SIGNAL_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="signal_interface_t">

   The <iface>signal</iface> interface is for modeling a logical signal, such
   as a reset or interrupt. Signals are always active high in Simics with
   respect to their function. This may not correspond to the actual electrical
   level in a real system.

   A signal connection has one initiator and one target object, where the
   initiator calls methods the interface implemented by the target.

   The initiator object should have a configuration attribute that pointing to
   the target object or to an interface port of the target. This attribute,
   like all other attributes representing object connections, should be of the
   object kind, or a list of two entries where the first is
   an object and the second a string representing the name of the port.

   The initiator should call <fun>signal_raise()</fun> to raise the signal
   level to its active level in the target. Once raised, the same initiator may
   not call <fun>signal_raise()</fun> again without an intervening call to
   <fun>signal_lower()</fun>. Similarly, an already low signal may not be
   lowered again by a <fun>signal_lower()</fun> call from the same initiator.
   The two functions represent the rising and the falling edge of the signal.

   The target should handle the case where a signal is lowered directly after
   it has been raised and treat this call sequence as a valid pulse even within
   a single clock cycle. The target should also allow the signal to remain
   raised for some time before it is lowered.

   While a target is disconnected, the input signal level is assumed to be low.
   When an initiator connects to a target by hot plugging,
   <fun>signal_raise()</fun> should be called if the output signal from the
   initiator was high. If the signal was low, then no function in the target
   should be called.

   If the signal level is high on disconnect, then the initiator has to call
   <fun>signal_lower()</fun> before disconnecting from the target. Connect and
   disconnect is typically done by changing the attribute in the initiator
   that identifies the target object.

   When an initiator is reset, it should call <fun>signal_lower()</fun> if the
   actual hardware also lowers the output signal on a reset. The target, on the
   other hand, should not reset its remembered value of the input.

   When a connection attribute is restored from a checkpoint or during reverse
   execution, no function should be called in the <iface>signal</iface>
   interface of the target since the actual signal level does not change. The
   attribute setter code can distinguish between hot-plugging and state
   restoring by using <fun>SIM_object_is_configured()</fun> and
   <fun>SIM_is_restoring_state</fun>. See the latter of the two for more
   documentation.

   When an object is first created and the initial signal level is high, the
   initiator has to call the <fun>signal_raise()</fun> function in the target.
   This can not be done earlier than in <fun>finalize_instance</fun> (C/C++) or
   in <fun>post_init()</fun> (DML) since the target has to be fully configured.
   Again, this should not be done when restoring a checkpoint.

   There must only be a single initiator connected to a target, with the
   exception of the <class>signal-bus</class> that may have several initiators.

   A target that needs more than one input signal should use ports to implement
   several <iface>signal</iface> interfaces.

   <insert-until text="// ADD INTERFACE signal_interface"/>

   </add>
   <add id="signal_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(signal) {
        void (*signal_raise)(conf_object_t *NOTNULL obj);
        void (*signal_lower)(conf_object_t *NOTNULL obj);
};

#define SIGNAL_INTERFACE "signal"
// ADD INTERFACE signal_interface

/* <add id="multi_level_signal_interface_t">
   Interface to model a signal with multiple levels, such as an analog signal.

   <note>Obsoleted by the <iface>uint64_state</iface> interface.</note>

   The signal initiator (i.e. the object producing the signal) should call
   <fun>signal_level_change</fun> when the level changes. Multiple calls with
   the same level should be accepted by the signal receiver.

   The optional function <fun>signal_current_level</fun> can be called when
   a new target is connected.

   <insert-until text="// ADD INTERFACE multi_level_signal_interface"/>

   </add>
   <add id="multi_level_signal_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(multi_level_signal) {
        void (*signal_level_change)(conf_object_t *NOTNULL obj, uint64 level);
        void (*signal_current_level)(conf_object_t *NOTNULL obj, uint64 level);
};
#define MULTI_LEVEL_SIGNAL_INTERFACE "multi_level_signal"
// ADD INTERFACE multi_level_signal_interface

/* <add id="pulse_interface_t">
   Interface to model a pulse, meaning an event that triggers.

   <insert-until text="// ADD INTERFACE pulse_interface"/>

   </add>
   <add id="pulse_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(pulse) {
        void (*pulse)(conf_object_t *NOTNULL obj);
};

#define PULSE_INTERFACE "pulse"
// ADD INTERFACE pulse_interface

#if defined(__cplusplus)
}
#endif

#endif
