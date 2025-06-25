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

#ifndef SIMICS_DEVS_INTERRUPT_H
#define SIMICS_DEVS_INTERRUPT_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="simple_interrupt_interface_t">

   A device calls <fun>interrupt()</fun> to logically raise an interrupt and
   <fun>interrupt_clear()</fun> to lower an interrupt.

   The first argument is the object to interrupt, usually a cpu. The
   integer argument to both functions may indicate an interrupt level
   or interrupt pin depending on the receiving device.

   On ARM the integer argument indicates whether the interrupt is normal or
   fast, by being either ARM_INT_IRQ or ARM_INT_FIQ defined by the ARM API
   (by including <file>&lt;simics/arch/arm.h&gt;</file>).

   <note>Obsoleted by the <iface>signal</iface> interface.</note>

   <insert-until text="// ADD INTERFACE simple_interrupt_interface"/>

   </add>
   <add id="simple_interrupt_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(simple_interrupt) {
        void (*interrupt)(conf_object_t *NOTNULL obj, int value);
        void (*interrupt_clear)(conf_object_t *NOTNULL obj, int value);
};

#define SIMPLE_INTERRUPT_INTERFACE "simple_interrupt"
// ADD INTERFACE simple_interrupt_interface


/* 
   This interrupt_query interface is used by devices connected to interrupt
   controllers to query whether their irq level is enabled in the interrupt
   controller or not. This is done by the is_enabled function.

   It can also be used to register a callback function to be called when the
   irq is enabled or disabled through the register_callback function.

   This is very old design, do not use in new models!
 */
typedef void (*interrupt_changed_state_callback_t)(
        conf_object_t *interrupt_controller,
        conf_object_t *device,
        int irq_level,
        int enabled,
        void *data);

SIM_INTERFACE(interrupt_query) {
        int (*is_enabled)(conf_object_t *interrupt_controller, int irq_level);
        void (*register_callback)(conf_object_t *interrupt_controller,
                                  conf_object_t *device,
                                  int irq_level,
                                  interrupt_changed_state_callback_t cb,
                                  void *cb_data);
};

#define INTERRUPT_QUERY_INTERFACE "interrupt_query"

#if defined(__cplusplus)
}
#endif

#endif
