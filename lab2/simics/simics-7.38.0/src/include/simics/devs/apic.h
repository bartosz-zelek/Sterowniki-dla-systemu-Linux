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

#ifndef SIMICS_DEVS_APIC_H
#define SIMICS_DEVS_APIC_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="apic_destination_mode_t"> </add-type> */
typedef enum {
        Apic_Destination_Mode_Physical = 0,
        Apic_Destination_Mode_Logical = 1
} apic_destination_mode_t;

/* <add-type id="apic_delivery_mode_t"> </add-type> */
typedef enum {
        Apic_Delivery_Mode_Fixed = 0,
        Apic_Delivery_Mode_Lowest_Priority = 1,
        Apic_Delivery_Mode_SMI = 2,
        Apic_Delivery_Mode_Remote_Read = 3,
        Apic_Delivery_Mode_NMI = 4,
        Apic_Delivery_Mode_INIT = 5,
        Apic_Delivery_Mode_Start_Up = 6,
        Apic_Delivery_Mode_Ext_INT = 7
} apic_delivery_mode_t;

/* <add-type id="apic_trigger_mode_t"> </add-type> */
typedef enum {
        Apic_Trigger_Mode_Edge = 0,
        Apic_Trigger_Mode_Level = 1
} apic_trigger_mode_t;

/* <add-type id="apic_delivery_status_t"> </add-type> */
typedef enum {
        Apic_Delivery_Status_Idle = 0,
        Apic_Delivery_Status_Send_Pending = 1
} apic_delivery_status_t;

/* <add-type id="apic_rr_status_t"> </add-type> */
typedef enum {
        Apic_RR_Invalid = 0,
        Apic_RR_Pending = 1,
        Apic_RR_Valid = 2
} apic_rr_status_t;

/* <add-type id="apic_bus_status_t"> </add-type> */
typedef enum {
        Apic_Bus_Accepted = 0,
        Apic_Bus_Retry = 1,
        Apic_Bus_No_Target = 2,
        Apic_Bus_Invalid = 3
} apic_bus_status_t;

/* <add id="apic_bus_interface_t">

   This interface is implemented by all apic buses, and used by the IO-APICs to
   send a message over the bus. 

   Messages with delivery mode <tt>Apic_Delivery_Mode_Ext_INT</tt> needs to be
   acknowledged. They are acknowledged at the object pointed to by the apic's
   "pic" attribute via the <iface>interrupt_cpu</iface> interface.

   <insert-until text="// end of interface"/>

   <insert id="apic_destination_mode_t"/>
   <insert id="apic_delivery_mode_t"/>
   <insert id="apic_trigger_mode_t"/>
   <insert id="apic_bus_status_t"/>

   See the architecture software developer's manual for more information about
   the parameters. For IPIs, the sender is responsible for filtering out
   reserved vectors (vectors 0 through 15) and flagging the appropriate error
   on the sending side. For I/O-APIC initiated interrupts, reserved vectors can
   be sent and will flag errors in the receiving APICs.

   </add>
   <add id="apic_bus_interface_exec_context">
   Cell Context for all methods.
   </add> */

SIM_INTERFACE(apic_bus) {
        apic_bus_status_t (*interrupt)(conf_object_t *obj,
                                       apic_destination_mode_t dest_mode,
                                       apic_delivery_mode_t delivery_mode,
                                       int level_assert,
                                       apic_trigger_mode_t trigger_mode,
                                       uint8 vector,
                                       uint8 destination);
};

#define APIC_BUS_INTERFACE "apic_bus"
// end of interface

#if defined(__cplusplus)
}
#endif

#endif
