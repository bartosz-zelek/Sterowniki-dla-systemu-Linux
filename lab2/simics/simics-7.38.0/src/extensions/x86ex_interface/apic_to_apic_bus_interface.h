/*
  apic_to_apic_bus_interface.h - Extension of APIC_BUS_INTERFACE

  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef APIC_TO_APIC_BUS_INTERFACE_H
#define APIC_TO_APIC_BUS_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/types.h>
#include <simics/devs/apic.h>
#include <simics/devs/interrupt-ack.h>

#ifdef __cplusplus
extern "C" {
#endif
/* <add id="apic_to_apic_bus">
   This interfaces provided by APIC bus to APIC.

   The <b>ioapic_eoi</b> method is used send end of interrupt to the
   all IOAPICs from APIC with <b>id</b> via APIC bus.

   The <b>interrupt</b> method is used to send interrupt from LAPIC to other
   APICs via APIC bus. There are several variants of recipients: all,
   all exclude self (to exclude self <b>exclude_id</b> should be specified and
   value of <b>exclude</b> should be true), self.
   Also <b>interrupt</b> method is not available from Python.

   <insert-until text="// ADD INTERFACE apic_to_apic_bus"/>
   </add>
   <add id="apic_to_apic_bus_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(apic_to_apic_bus) {
        void (*ioapic_eoi)(conf_object_t *obj, int id);

#ifndef PYWRAP
        apic_bus_status_t   (*interrupt)(conf_object_t *obj,
                                         apic_destination_mode_t dest_mode,
                                         apic_delivery_mode_t delivery_mode,
                                         bool level_assert,
                                         apic_trigger_mode_t trigger_mode,
                                         uint8 vector,
                                         uint32 destination,
                                         bool exclude,
                                         uint32 exclude_id,
                                         interrupt_ack_fn_t ack_func,
                                         conf_object_t *ack_func_arg,
                                         conf_object_t *source);
#endif
};

#define APIC_TO_APIC_BUS_INTERFACE "apic_to_apic_bus"
// ADD INTERFACE apic_to_apic_bus

#ifdef __cplusplus
}
#endif

#endif /* ! APIC_TO_APIC_BUS_INTERFACE_H */
