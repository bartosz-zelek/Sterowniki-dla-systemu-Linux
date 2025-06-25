/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_ARCH_I8051_H
#define SIMICS_ARCH_I8051_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="i8051_interrupt_interface_t">
   <insert-until text="// ADD INTERFACE i8051_interrupt"/>

   The <fun>active_interrupt()</fun> function returns the interrupt vector
   address of the highest pending interrupt. This function should only be
   called when the processor should take an interrupt. This function may also
   set internal interrupt controller state, which can be cleared by the
   <fun>reti</fun> function call when returning from the interrupt.

   The <fun>reti</fun> function is used to notify the interrupt controller that
   the currently active interrupt has completed.

   The <fun>suppress_irq</fun> function is used to determine whether interrupt 
   should be blocked after a writing access to a memory location.
   </add>
   <add id="i8051_interrupt_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(i8051_interrupt) {
        int (*active_interrupt)(conf_object_t *NOTNULL obj);
        void (*reti)(conf_object_t *NOTNULL obj);
        bool (*suppress_irq)(conf_object_t *NOTNULL obj, uint32 addr);        
};
#define I8051_INTERRUPT_INTERFACE "i8051_interrupt"
// ADD INTERFACE i8051_interrupt

/* <add id="i8051_timer_interface_t">
   <insert-until text="// ADD INTERFACE i8051_timer"/>

   The <fun>change_mode</fun> function is used to change the working mode (e.g.
   16bit timer, 8 bit timer or event counting etc.)
   of the timer.

   The <fun>switch_timer</fun> function is used to enable or disable
   the timer. Timers can be switched on or off by either software or hardware.
   The parameter caller is either 0 or 1, indicating the control
   is from software or hardware. Only when the timer is switched on or off
   by hardware, the third parameter onoff is used, which indicates
   whether the timer is to be enabled or disabled.
   </add>
   <add id="i8051_timer_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(i8051_timer) {
        void (*change_mode)(conf_object_t *NOTNULL obj, uint8 mode);
        void (*switch_timer)(conf_object_t *obj, uint8 caller, bool onoff);
};
#define I8051_TIMER_INTERFACE "i8051_timer"
// ADD INTERFACE i8051_timer
        
#if defined(__cplusplus)
}
#endif

#endif
