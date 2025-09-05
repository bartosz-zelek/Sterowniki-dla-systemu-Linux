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

#ifndef SIMICS_MODEL_IFACE_STATE_H
#define SIMICS_MODEL_IFACE_STATE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* SIMICS-17129 - this is really a model-model interface, not model-simulator */

/* <add id="uint64_state_interface_t">

   Interface to transfer a state representable in an uint64 from one device to
   another.  Examples of what the state might represent includes:

   <dl>
     <li>a fixed-point value representing the level of an analog signal</li>
     <li>an integer representing a counter</li>
     <li>an integer representing an enum value</li>
   </dl>

   The initiator should call <fun>set</fun> when the value changes, and after a
   new target is connected.  The object implementing
   <iface>uint64_state</iface> should accept multiple calls to <fun>set</fun>
   with the same level, and may let this trigger side-effects. Therefore, any
   repeated calls must be deterministic; in particular, <fun>set</fun> must not
   be called while restoring a checkpoint.

   A device implementing this interface may choose to only accept a certain set
   of integer values; it is then an error to send any other values to the
   <fun>set</fun> method.  A user must therefore be careful to read the
   documentation of both the source and destination object to make sure they
   are compatible.

   No interface call needs to be done after disconnecting a target; the target
   needs to be notified of this through some other channel (typically via a
   connector)

   <note>The <iface>uint64_state</iface> interface should be used instead of
   the deprecated <iface>multi_level_signal</iface> interface when writing new
   models.</note>

   <insert-until text="// ADD INTERFACE uint64_state_interface"/>

   </add>
   <add id="uint64_state_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(uint64_state) {
        void (*set)(conf_object_t *NOTNULL obj, uint64 level);
};
#define UINT64_STATE_INTERFACE "uint64_state"
// ADD INTERFACE uint64_state_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_MODEL_IFACE_STATE_H */
