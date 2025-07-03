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

#ifndef SIMICS_DEVS_FIREWIRE_H
#define SIMICS_DEVS_FIREWIRE_H

#include <simics/pywrap.h>
#include <simics/util/dbuffer.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="firewire_ack_code_t"></add-type> */
typedef enum {
        /* Values defined in the FireWire specification */
        Firewire_Ack_Complete = 1,
        Firewire_Ack_Pending = 2,
        Firewire_Ack_Busy_X = 4,
        Firewire_Ack_Busy_A = 5,
        Firewire_Ack_Busy_B = 6,
        Firewire_Ack_Tardy = 0xb,
        Firewire_Ack_Conflict_Error = 0xc,
        Firewire_Ack_Data_Error = 0xd,
        Firewire_Ack_Type_Error = 0xe,
        Firewire_Ack_Address_Error = 0xf,
        
        /* Values not defined in FireWire but used in Simics */
        Firewire_Ack_No_Destination = 0x10, /* no destination found */
        Firewire_Ack_No_Ack = 0x11 /* no ack signal sent for packet */
} firewire_ack_code_t;

/* <add-type id="firewire_response_code_t"></add-type> */
typedef enum {
        Firewire_Response_Complete      = 0,
        /*Firewire_Response_Conflict    = 4,*/
        Firewire_Response_Data_Error    = 5,
        Firewire_Response_Type_Error    = 6,
        Firewire_Response_Address_Error = 7
} firewire_response_code_t;

/* <add-type id="firewire_transaction_code_t"></add-type> */
typedef enum {
        Firewire_Write_Quadlet         = 0x0,
        Firewire_Write_Block           = 0x1,
        Firewire_Write_Response        = 0x2,
        Firewire_Read_Quadlet          = 0x4,
        Firewire_Read_Block            = 0x5,
        Firewire_Read_Quadlet_Response = 0x6,
        Firewire_Read_Block_Response   = 0x7,
        Firewire_Cycle_Start           = 0x8,
        Firewire_Lock                  = 0x9,
        Firewire_Streaming             = 0xa,
        Firewire_Lock_Response         = 0xb
} firewire_transaction_code_t;

/* <add-type id="firewire_async_lock_code_t"></add-type> */
typedef enum {
        Firewire_Lock_Maskswap    = 1,
        Firewire_Lock_Compareswap = 2,
        Firewire_Lock_Fetchadd    = 3,
        Firewire_Lock_Littleadd   = 4,
        Firewire_Lock_Boundedadd  = 5,
        Firewire_Lock_Wrapadd     = 6,
        Firewire_Lock_Vendor      = 7
} firewire_async_lock_code_t;

/* <add-type id="firewire_iso_tag_t"></add-type> */
typedef enum {
        Firewire_Tag_Unformatted = 0
        /* All others currently reserved */
} firewire_iso_tag_t;

/* <add-type id="firewire_port_status_t"></add-type> */
typedef enum {
        Firewire_Port_No_Port = 0,
        Firewire_Port_Not_Connected = 1,
        Firewire_Port_Parent = 2,
        Firewire_Port_Child = 3
} firewire_port_status_t;

/* <add id="firewire_bus_interface_t">

   <insert-until text="// ADD INTERFACE firewire_bus_interface"/>

   This interface must be implemented by all firewire buses.

   <fun>connect_device</fun> is called when a device wants to connect to the
   bus. The bus should return 0 upon success.

   <fun>disconnect_device</fun> is called when a device wants to disconnect
   from the bus. The bus should return 0 upon success.
   
   <fun>set_device_bus_id</fun> sets the bus id for the device. This needs to
   be called by a device when its Node_IDS[bus_id] field is updated. The
   new bus id should be placed in bits 15-6 in the bus_id argument.

   <fun>set_id_mask</fun> can be called by a device to specify a mask that
   should be applied to the device ID when routing transfers. This can be
   used to e.g. accept transfers to multiple bus-numbers.

   <fun>transfer</fun> sends a packet. The bus will
   route the transfer to the correct device(s). The <param>source</param>
   parameter should be set to the device which sent the transfer. The bus uses
   this parameter to avoid sending packets to their sender. If the
   <param>crc_calculated</param> argument
   is non-zero the packet's crc fields are filled in.The return code is one
   of:

   <insert id="firewire_ack_code_t"/>

   <fun>register_channel</fun> can be called to tell the bus that the device
   want to receive isochronous transfers to channel <param>channel</param>.
   channel must be in the range [0, 63).
   A device can register several channels. It is an error to subscribe a
   device to a channel it is already subscribed to.
   Returns 0 on success.

   <fun>unregister_channel</fun> tells the bus the device is no longer
   interested in transfers to channel <param>channel</param>. channel must be
   in the range [0, 63). It is an error to unsubscribe a device from a channel
   if it isn't subscribing to it. Returns 0 on success.

   <fun>reset</fun> can be called to cause a bus reset. After the bus reset,
   all the devices may have received new IDs. 

   </add>
   <add id="firewire_bus_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(firewire_bus) {
        int (*connect_device)(conf_object_t *NOTNULL bus,
                              conf_object_t *NOTNULL dev);
        int (*disconnect_device)(conf_object_t *NOTNULL bus,
                                 conf_object_t *NOTNULL dev);
        void (*set_device_bus_id)(conf_object_t *NOTNULL bus,
                                  conf_object_t *NOTNULL dev,
                                  uint16 bus_id);

        void (*set_id_mask)(conf_object_t *NOTNULL bus,
                            conf_object_t *NOTNULL dev,
                            uint16 id_mask);

        firewire_ack_code_t (*transfer)(conf_object_t *NOTNULL bus,
                                        conf_object_t *NOTNULL source,
                                        dbuffer_t *packet, int crc_calculated);

        int (*register_channel)(conf_object_t *NOTNULL bus,
                                conf_object_t *NOTNULL dev,
                                uint32 channel);
        int (*unregister_channel)(conf_object_t *NOTNULL bus,
                                  conf_object_t *NOTNULL dev,
                                  uint32 channel);

        void (*reset)(conf_object_t *NOTNULL bus);
};
#define FIREWIRE_BUS_INTERFACE "firewire_bus"
// ADD INTERFACE firewire_bus_interface

/* <add-type id="uint32_array_t">
     An array of unsigned 32-bit integers.
   </add-type> */
typedef struct {
        size_t len;
        uint32 *data;
} uint32_array_t;

/* <add id="firewire_device_interface_t">

   <insert-until text="// ADD INTERFACE firewire_device_interface"/>

   This interface must be implemented by all firewire devices.

   <fun>transfer</fun> is called when the device receives a packet.
   If the crc_calculated argument
   is non-zero the packet's crc fields are filled in.The return code is one
   of:

   <insert id="firewire_ack_code_t"/>

   <fun>reset</fun> is called when the bus is reset. <param>id</param> is
   the new ID of the device (top 10 bits represent the bus number, and the low
   6 bits the node ID). <param>root_id</param> is the node ID for the root
   node. <param>self_ids</param> is the list of Self-ID packets
   during the Self-ID phase of the reset.

   <insert id="uint32_array_t"/>

   <fun>get_self_id_template</fun> is an accessor function called by the bus
   or other self-ID provider to provide the static parts of the Self-ID for
   this device. The ID and port parts will be overwritten by the Self-ID
   provider.

   <fun>get_rhb</fun> gets the current status of the root hold off bit in the
   device.

   <fun>get_port_count</fun> returns the number of ports the device has. The
   largest allowed value is 16 and the lowest 1. This should remain constant
   during the lifetime of the device and always return a valid value.

   <fun>get_port_mask</fun> returns a bitmask. Each bit is one if that port is
   enabled and zero if it is disabled. The least significant bit describes port
   0 and the most significant bit port 15. The bits for ports which do not
   exist on the device should be set to zero.

   </add>
   <add id="firewire_device_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(firewire_device) {
        firewire_ack_code_t (*transfer)(conf_object_t *NOTNULL dev,
                                        dbuffer_t *packet, int crc_calculated);

        void (*reset)(conf_object_t *NOTNULL dev, uint16 id,
                      uint8 root_id,
                      uint32_array_t self_ids);
        uint32 (*get_self_id_template)(
                conf_object_t *NOTNULL dev);

        int (*get_rhb)(conf_object_t *NOTNULL dev);
        uint8 (*get_port_count)(conf_object_t *NOTNULL dev);
        uint16 (*get_port_mask)(conf_object_t *NOTNULL dev);
};
#define FIREWIRE_DEVICE_INTERFACE "firewire_device"
// ADD INTERFACE firewire_device_interface

#if defined(__cplusplus)
}
#endif

#endif
