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

#ifndef SIMICS_DEVS_INTERRUPT_ACK_H
#define SIMICS_DEVS_INTERRUPT_ACK_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="interrupt_ack_interface_t|interrupt_cpu_interface_t">
   The <tt>interrupt_ack_fn_t</tt> function is called by an interrupt target
   to ack an interrupt. Returns the interrupt vector.
   <ndx>interrupt_ack_fn_t</ndx>
   </add-type> */
typedef int (*interrupt_ack_fn_t)(conf_object_t *NOTNULL);

/* <add id="interrupt_ack_interface_t">
   Interface for acked interrupts. The target is typically a cpu that
   later calls the supplied ack function when the interrupt is
   actually taken.

   The <arg>ack</arg> argument in the <fun>lower_interrupt</fun> function
   serves no purpose and should not be used.

   To recover the <arg>ack</arg> function after a checkpoint restoration,
   read it from the <iface>interrupt_cpu</iface> interface.

   <insert-until text="// ADD INTERFACE interrupt_ack_interface"/>

   </add>
   <add id="interrupt_ack_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(interrupt_ack) {
        void (*raise_interrupt)(conf_object_t *NOTNULL obj,
                                interrupt_ack_fn_t cb,
                                conf_object_t *ack_obj);
        void (*lower_interrupt)(conf_object_t *NOTNULL obj,
                                interrupt_ack_fn_t cb);
};

#define INTERRUPT_ACK_INTERFACE "interrupt_ack"
// ADD INTERFACE interrupt_ack_interface

/* <add id="interrupt_cpu_interface_t">
   Interface that must be implemented by an interrupt source that
   sends interrupts through the <iface>x86</iface> interface. Used to
   reestablish the ack function when loading a checkpoint.

   <insert-until text="// ADD INTERFACE interrupt_cpu_interface"/>

   </add>
   <add id="interrupt_cpu_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(interrupt_cpu) {
        interrupt_ack_fn_t ack;
};

#define INTERRUPT_CPU_INTERFACE "interrupt_cpu"
// ADD INTERFACE interrupt_cpu_interface

#if defined(__cplusplus)
}
#endif

#endif
