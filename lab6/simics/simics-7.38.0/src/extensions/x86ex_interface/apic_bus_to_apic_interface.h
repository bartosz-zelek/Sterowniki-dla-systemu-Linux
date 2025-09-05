/*
  apic_bus_to_apic_interface.h - Extension for APIC CPU interface

  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef APIC_BUS_TO_APIC_INTERFACE_H
#define APIC_BUS_TO_APIC_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/types.h>
#include <simics/devs/apic.h>
#include <simics/devs/interrupt-ack.h>
#include <simics/arch/x86.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: move apic_type_t enum into x86.h, exclude apic_bus_to_apic_interface.h
   from apic.h header */

/* <add-type id="apic_type_t"> </add-type> */
typedef enum {
        Apic_P6,
        Apic_P4,
        Apic_X2
} apic_type_t;

/* <add id="apic_bus_to_apic">
    This interface is provided by APIC to APIC bus.

    <b>get_arbitration_id</b> and <b>set_arbitration_id</b> methods
    are used to get/set value of the 4-bit software-transparent arbitration id register of the LAPIC.

    <b>get_apic_type</b> method is used to get type of the certain APIC. Also APIC has an attribute
    <b>apic_type</b> which indicate APIC's type. The type of APIC is one of the constants
    defined in <type>apic_type_t</type>.

    <insert id="apic_type_t"/>

    There are 3 types of the APIC:
    1) "P6" - this type of the APIC is used in system based on P6 family and Pentium&reg; processors, obsolete.
    2) "P4" - this type of the APIC is used in system based Pentium&reg; 4 and Intel&reg; Xeon&reg; processors.
    Also "P4" APICs is an extension of the APIC architecture found in the P6 family processors.
    Architecture of the APIC with type "P4" called the xAPIC architecture.
    See more information in IA SDM Vol3 ch. 10.3.
    3) "X2" - this type of the APIC provides an extension of the xAPIC architecture
    ("P4" APIC type) - x2APIC. The x2APIC architecture provides backward compatibility to the
    xAPIC architecture and forward extendability for future Intel platform innovations.
    These extensions and modifications are supported by a new mode of execution (x2APIC mode)
    are detailed in IA SDM Vol. 3 Section 10.12.

    <b>remote_read</b> method is used to do remote memory transaction.
    Remote transactions are supported only in the 82489DX and local APIC for Pentium&reg; processors.
    This method is obsolete for modern processors.

    <b>get_apic_id</b> method is used to get local APIC ID.

    <b>get_arbitration_priority</b> method is used to get arbitration priority of the
    local APIC. Each local APIC is given an arbitration priority of from 0 to 15.
    The arbitration priority is available only for system based on P6 family processors.
    This method is obsolete for modern processors.

    <b>get_apr</b> method is used to get value of the read only Arbitration Priority Register (APR)
    of the APIC. The APR is available only for system based on P6 family processors,
    so it is obsolete method for modern processors.

    <b>has_physical_destination</b> and <b>has_logical_destination</b> methods are used to
    find out which destination mode has IPI: physical or logical.

    <b>is_focus</b> method is used to find out is to be checked processor is <b>focus processor</b>.
    A processor is said to be the <b>focus of an interrupt</b> if it is currently servicing that
    interrupt or if it has a pending request for that interrupt.
    This arbitration scheme was used only in P6 family and Pentium&reg; processors, so it is obsolete method
    for modern processors.

    The <b>start_up</b> function is used by the local APIC to bring the
    processor <arg>obj</arg> out of the waiting for start-up IPI state, with a
    start address of <arg>start_address</arg>.

    <b>is_interrupt_slot_available</b> method is used to do check for the free interrupt slots
    in the local APIC.
    Only the local APICs having free interrupt slots participate in the lowest priority arbitration.
    The interrupt slot based arbitration scheme was used only in system based on the P6 family and
    Pentium&reg; processors and it is not used in modern processors (obsolete method).

    <b>set_pin_status</b> method is used to specify interrupt input pin polarity of the processor's
    LINT0 and LINT1 pins: (0) active high or (1) active low.

    <b>get_ack_in_progress</b> method indicates that acknowledging of the  external interrupt is
    currently processing.

    <b>do_lower_ext_int</b> method is used to clear posted and requested interrupts in the processor.

    <b>get_processor_priority</b> method is used to get value of the Processor Priority Register (PPR)
    of the APIC.

    <b>get_lvt_lint0</b> method is used to get value of the LVT entry called LINT0 of the APIC.

    <b>interrupt</b> method is used to send interrupt from APIC bus to certain APIC. Also <b>interrupt</b>
    method is not available from Python.
   <insert-until text="// ADD INTERFACE apic_bus_to_apic"/>
   </add>
   <add id="apic_bus_to_apic_interface_exec_context">
   Cell Context for all methods.
   </add> */

SIM_INTERFACE(apic_bus_to_apic) {
        uint32      (*get_arbitration_id)(conf_object_t *obj);
        void        (*set_arbitration_id)(conf_object_t *obj, uint32 val);

        uint32      (*get_arbitration_priority)(conf_object_t *obj);
        uint32      (*get_processor_priority)(conf_object_t *obj);

        uint32      (*get_apr)(conf_object_t *obj);
        uint32      (*get_lvt_lint0)(conf_object_t *obj);

        apic_type_t (*get_apic_type)(conf_object_t *obj);
        uint32      (*get_apic_id)(conf_object_t *obj);

        bool        (*get_ack_in_progress)(conf_object_t *obj);

        void        (*set_pin_status)(conf_object_t *obj, x86_pin_t pin, int status);

        bool        (*has_physical_destination)(conf_object_t *obj,
                                                uint32 destination,
                                                bool exclude,
                                                uint32 exclude_id);
        bool        (*has_logical_destination)(conf_object_t *obj, uint32 destination);

        bool        (*is_focus)(conf_object_t *obj, uint8 vector);
        bool        (*is_interrupt_slot_available)(conf_object_t *obj, uint8 vector);
        void        (*remote_read)(conf_object_t *obj, uint8 vector);
        void        (*do_lower_ext_int)(conf_object_t *obj);
        void        (*start_up)(conf_object_t *obj, uint32 start_address);

#ifndef PYWRAP
        void        (*interrupt)(conf_object_t *obj,
                                 apic_delivery_mode_t delivery_mode,
                                 bool level_assert,
                                 apic_trigger_mode_t trigger_mode,
                                 uint8 vector,
                                 interrupt_ack_fn_t ack_func,
                                 conf_object_t *ack_func_arg);

#endif
};

#define APIC_BUS_TO_APIC_INTERFACE "apic_bus_to_apic"
// ADD INTERFACE apic_bus_to_apic

#ifdef __cplusplus
}
#endif

#endif /* ! APIC_BUS_TO_APIC_INTERFACE_H */
