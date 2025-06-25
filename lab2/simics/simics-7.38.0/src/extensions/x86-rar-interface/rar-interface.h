/* © 2022 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/
#ifndef EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_H
#define EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_H
#include <simics/base/types.h>
#include <simics/pywrap.h>
#if defined(__cplusplus)
extern "C" {
#endif

/* Vector used to communicate RAR-specific event as a regular interrupt */
enum {X86_Rar_Fake_Vector = 0x4000};

/* <add id="x86_rar_interrupt_interface_t">
   Methods of this interface are used to provide CPU with
   information about RAR interrupt status from APIC

   The <b>is_rar_requested</b> method returns whether a RAR event is pending.

   The <b>ack_rar</b> method acknowledges RAR interrupt.

   <insert-until text="// ADD INTERFACE x86_rar_interrupt_interface"/>
   </add>
   <add id="x86_rar_interrupt_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_rar_interrupt) {
    bool (*is_rar_requested)(conf_object_t *obj);
    int (*ack_rar)(conf_object_t *obj);
};
#define X86_RAR_INTERRUPT_INTERFACE "x86_rar_interrupt"
// ADD INTERFACE x86_rar_interrupt_interface

#if defined(__cplusplus)
}
#endif
#endif // EXTENSIONS_X86_RAR_INTERFACE_RAR_INTERFACE_H
