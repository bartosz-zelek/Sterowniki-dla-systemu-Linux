/*
  © 2023 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_INTERRUPT_SUBSCRIBER_H
#define SIMICS_DEVS_INTERRUPT_SUBSCRIBER_H

#include <simics/base/conf-object.h>
#include <simics/devs/apic.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif


/* <add id="devs api types">
      <name index="true">interrupt_source_t</name>
      <insert id="interrupt_source_t DOC"/>
   </add> */

/* <add id="interrupt_source_t DOC">
   <ndx>interrupt_source_t</ndx>

      <doc>
         <doc-item name="NAME">interrupt_source_t</doc-item>
         <doc-item name="SYNOPSIS"><insert id="interrupt_source_t def"/></doc-item>
         <doc-item name="DESCRIPTION">

         Sources of interrupts.
         <ul>
            <li><const>Interrupt_Source_Icr_Ipr</const> means that the source is
            the Interrupt Control Register - Inter Processor Interrupt.</li>
            <li><const>Interrupt_Source_Msi</const> means that the source is an
            MSI.</li>
            <li><const>Interrupt_Source_Virtual_Wire</const> means that the
            source is the Virtual Wire.</li>
            <li><const>Interrupt_Source_Nmi_Pin</const> means that the source is
            the external NMI pin.</li>
            <li><const>Interrupt_Source_Lvt</const> means that the source is the
            local vector table (LVT).</li>
            <li><const>Interrupt_Source_Iommu</const> means that the source is
            the IOMMU.</li>
            <li><const>Interrupt_Source_Int2</const> means that the source is the
            INT2 instruction.</li>
            <li><const>Interrupt_Source_Vmcs_Injection</const> means that the
            source is an interrupt injected through the VMCS.</li>
            <li><const>Interrupt_Source_Legacy_Apic_Vector</const> means that the
            source is the legacy APIC interrupt vector.</li>
            <li><const>Interrupt_Source_Self_Ipi</const> means that the source is
            the SELF IPI Register.</li>
            <li><const>Interrupt_Source_Unknown</const> means that the source is
            unknown.</li>
         </ul>
         </doc-item>

         <doc-item name="SEE ALSO"><iface>interrupt_subscriber_interface_t</iface>
         </doc-item>

      </doc>
   </add>
*/
/* <add-type id="interrupt_source_t def"></add-type>
 */
typedef enum {
        Interrupt_Source_Icr_Ipr,
        Interrupt_Source_Msi,
        Interrupt_Source_Virtual_Wire,
        Interrupt_Source_Nmi_Pin,
        Interrupt_Source_Lvt,
        Interrupt_Source_Iommu,
        Interrupt_Source_Int2,
        Interrupt_Source_Vmcs_Injection,
        Interrupt_Source_Legacy_Apic_Vector,
        Interrupt_Source_Self_Ipi,
        Interrupt_Source_Unknown,
} interrupt_source_t;


/* <add id="interrupt_subscriber_interface_t">

   The target is typically a CPU that reacts to the interrupt.

   The function <fun>notify</fun> notifies when an interrupt has occurred.

   The function <fun>reset</fun> is run when the interrupt notifier source is
   reset.

   <insert-until text="// ADD INTERFACE interrupt_subscriber_interface"/>
   </add>
   <add id="interrupt_subscriber_interface_exec_context">
   Threaded Context for all methods.
   </add>
*/
#define INTERRUPT_SUBSCRIBER_INTERFACE "interrupt_subscriber"
SIM_INTERFACE(interrupt_subscriber) {
        void (*notify)(
                conf_object_t *NOTNULL obj,
                apic_delivery_mode_t delivery_mode,
                bool level_assert,
                apic_trigger_mode_t trigger_mode,
                uint8 vector,
                interrupt_source_t source);
        void (*reset)(conf_object_t *NOTNULL obj);
};
// ADD INTERFACE interrupt_subscriber_interface

#ifdef __cplusplus
}
#endif

#endif // SIMICS_DEVS_INTERRUPT_SUBSCRIBER_H
