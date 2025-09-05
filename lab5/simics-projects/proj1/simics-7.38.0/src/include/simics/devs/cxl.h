/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_DEVS_CXL_H
#define SIMICS_DEVS_CXL_H

#include <simics/base/types.h>
#include <simics/base/memory-transaction.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add-type id="cxl_mbox_cmd_rc_t def">
   </add-type>
*/
typedef enum {
        CXL_MBOX_CMD_RC_SUCCESS = 0x0000,
        CXL_MBOX_CMD_RC_BACKGROUND_COMMAND_STARTED = 0x0001,
        CXL_MBOX_CMD_RC_INVALID_INPUT = 0x0002,
        CXL_MBOX_CMD_RC_UNSUPPORTED = 0x0003,
        CXL_MBOX_CMD_RC_INTERNAL_ERROR = 0x0004,
        CXL_MBOX_CMD_RC_RETRY_REQUIRED = 0x0005,
        CXL_MBOX_CMD_RC_BUSY = 0x0006,
        CXL_MBOX_CMD_RC_MEDIA_DISABLED = 0x0007,
        CXL_MBOX_CMD_RC_FW_TRANSFER_IN_PROGRESS = 0x0008,
        CXL_MBOX_CMD_RC_FW_TRANSFER_OUT_OF_ORDER = 0x0009,
        CXL_MBOX_CMD_RC_FW_AUTHENTICATION_FAILED = 0x000A,
        CXL_MBOX_CMD_RC_INVALID_SLOT = 0x000B,
        CXL_MBOX_CMD_RC_ACTIVATION_FAILED_FW_ROLLED_BACK = 0x000C,
        CXL_MBOX_CMD_RC_ACTIVATION_FAILED_COLD_RESET_REQUIRED = 0x000D,
        CXL_MBOX_CMD_RC_INVALID_HANDLE = 0x000E,
        CXL_MBOX_CMD_RC_INVALID_PHYSICAL_ADDRESS = 0x000F,
        CXL_MBOX_CMD_RC_INJECT_POISON_LIMIT_REACHED = 0x0010,
        CXL_MBOX_CMD_RC_PERMANENT_MEDIA_FAILURE = 0x0011,
        CXL_MBOX_CMD_RC_ABORTED = 0x0012,
        CXL_MBOX_CMD_RC_INVALID_SECURITY_STATE = 0x0013,
        CXL_MBOX_CMD_RC_INCORRECT_PASSPHRASE = 0x0014,
        CXL_MBOX_CMD_RC_UNSUPPORTED_MAILBOX = 0x0015,
        CXL_MBOX_CMD_RC_INVALID_PAYLOAD_LENGTH = 0x0016
} cxl_mbox_cmd_rc_t;

/*
   <add-type id="cxl_mbox_op_t def">
   </add-type>
*/
typedef enum {
        CXL_MBOX_OP_INVALID = 0x0000,
        CXL_MBOX_OP_GET_EVENT_RECORD = 0x0100,
        CXL_MBOX_OP_CLEAR_EVENT_RECORD = 0x0101,
        CXL_MBOX_OP_GET_EVT_INT_POLICY = 0x0102,
        CXL_MBOX_OP_SET_EVT_INT_POLICY = 0x0103,
        CXL_MBOX_OP_GET_FW_INFO = 0x0200,
        CXL_MBOX_OP_TRANSFER_FW = 0x0201,
        CXL_MBOX_OP_ACTIVATE_FW = 0x0202,
        CXL_MBOX_OP_GET_TIMESTAMP = 0x0300,
        CXL_MBOX_OP_SET_TIMESTAMP = 0x0301,
        CXL_MBOX_OP_GET_SUPPORTED_LOGS = 0x0400,
        CXL_MBOX_OP_GET_LOG = 0x0401,
        CXL_MBOX_OP_IDENTIFY = 0x4000,
        CXL_MBOX_OP_GET_PARTITION_INFO = 0x4100,
        CXL_MBOX_OP_SET_PARTITION_INFO = 0x4101,
        CXL_MBOX_OP_GET_LSA = 0x4102,
        CXL_MBOX_OP_SET_LSA = 0x4103,
        CXL_MBOX_OP_GET_HEALTH_INFO = 0x4200,
        CXL_MBOX_OP_GET_ALERT_CONFIG = 0x4201,
        CXL_MBOX_OP_SET_ALERT_CONFIG = 0x4202,
        CXL_MBOX_OP_GET_SHUTDOWN_STATE = 0x4203,
        CXL_MBOX_OP_SET_SHUTDOWN_STATE = 0x4204,
        CXL_MBOX_OP_GET_POISON = 0x4300,
        CXL_MBOX_OP_INJECT_POISON = 0x4301,
        CXL_MBOX_OP_CLEAR_POISON = 0x4302,
        CXL_MBOX_OP_GET_SCAN_MEDIA_CAPS = 0x4303,
        CXL_MBOX_OP_SCAN_MEDIA = 0x4304,
        CXL_MBOX_OP_GET_SCAN_MEDIA = 0x4305,
        CXL_MBOX_OP_SANITIZE = 0x4400,
        CXL_MBOX_OP_SECURE_ERASE = 0x4401,
        CXL_MBOX_OP_GET_SECURITY_STATE = 0x4500,
        CXL_MBOX_OP_SET_PASSPHRASE = 0x4501,
        CXL_MBOX_OP_DISABLE_PASSPHRASE = 0x4502,
        CXL_MBOX_OP_UNLOCK = 0x4503,
        CXL_MBOX_OP_FREEZE_SECURITY = 0x4504,
        CXL_MBOX_OP_PASSPHRASE_SECURE_ERASE = 0x4505,
        CXL_MBOX_OP_SECURITY_SEND = 0x4600,
        CXL_MBOX_OP_SECURITY_RECEIVE = 0x4601,
        CXL_MBOX_OP_GET_SLD_QOS_CONTROL = 0x4700,
        CXL_MBOX_OP_SET_SLD_QOS_CONTROL = 0x4701,
        CXL_MBOX_OP_GET_SLD_QOS_STATUS = 0x4702,
        CXL_MBOX_OP_EVENT_NOTIFICATION = 0x5000,
        CXL_MBOX_OP_IDENTIFY_SWITCH_DEVICE = 0x5100,
        CXL_MBOX_OP_GET_PHYSICAL_PORT_STATE = 0x5101,
        CXL_MBOX_OP_PHYSICAL_PORT_CONTROL = 0x5102,
        CXL_MBOX_OP_SEND_PPB_CXL_IO_CONFIGURATION_REQUEST_PHYSICAL = 0x5103,
        CXL_MBOX_OP_GET_VIRTUAL_CXL_SWITCH_INFO = 0x5200,
        CXL_MBOX_OP_BIND_VPPB = 0x5201,
        CXL_MBOX_OP_UNBIND_VPPB = 0x5202,
        CXL_MBOX_OP_GENERATE_AER_EVENT = 0x5203,
        CXL_MBOX_OP_TUNNEL_MANAGEMENT_COMMAND = 0x5300,
        CXL_MBOX_OP_SEND_PPB_CXL_IO_CONFIGURATION_REQUEST_MLD = 0x5301,
        CXL_MBOX_OP_SEND_PPB_CXL_IO_MEMORY_REQUEST = 0x5302,
        CXL_MBOX_OP_GET_LD_INFO = 0x5400,
        CXL_MBOX_OP_GET_LD_ALLOCATIONS = 0x5401,
        CXL_MBOX_OP_SET_LD_ALLOCATIONS = 0x5402,
        CXL_MBOX_OP_GET_QOS_CONTROL = 0x5403,
        CXL_MBOX_OP_SET_QOS_CONTROL = 0x5404,
        CXL_MBOX_OP_GET_QOS_STATUS = 0x5405,
        CXL_MBOX_OP_GET_QOS_ALLOCATED_BW = 0x5406,
        CXL_MBOX_OP_SET_QOS_ALLOCATED_BW = 0x5407,
        CXL_MBOX_OP_GET_QOS_BW_LIMIT = 0x5408,
        CXL_MBOX_OP_SET_QOS_BW_LIMIT = 0x5409,
} cxl_mbox_op_t;

/* <add id="cxl_map_interface_t">

   This interface is used to claim ranges in CXL address spaces.

   Functions <fun>add_map</fun> and <fun>del_map</fun> are used to add and
   remove maps, <arg>map_obj</arg> will be mapped in the address space
   indicated by <arg>type</arg> according to the information in <arg>info</arg>.

   Note: This interface is considered tech-preview and may change without
   notice.

   <insert-until text="// ADD INTERFACE cxl_map_interface"/>
   </add>

   <add id="cxl_map_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        CXL_Type_Not_Set,
        CXL_Type_Io,
        CXL_Type_Mem,
        CXL_Type_Cache,
        CXL_Type_Other,
} cxl_type_t;

SIM_INTERFACE(cxl_map)
{
        void (*add_map)(conf_object_t *obj, conf_object_t *map_obj,
                        map_info_t info, cxl_type_t type);
        void (*del_map)(conf_object_t *obj, conf_object_t *map_obj,
                        physical_address_t base, cxl_type_t type);
};

#define CXL_MAP_INTERFACE "cxl_map"
// ADD INTERFACE cxl_map_interface

/* <add id="cxl_non_device_decoder_handling_interface_t">

   Functions <fun>enable_decoder</fun> and <fun>disable_decoder</fun> are used
   to enable and disable HDM decoders on a cxl-hdm-port device.

   <insert-until text="// ADD INTERFACE cxl_non_device_decoder_handling_interface"/>
   </add>

   <add id="cxl_non_device_decoder_handling_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(cxl_non_device_decoder_handling)
{
        int (*enable_decoder)(conf_object_t *obj, uint8 index, uint8 ig,
                              uint8 iw, uint64 base, uint64 size,
                              uint64 target_list);
        int (*disable_decoder)(conf_object_t *obj, uint8 decoder_index);
};

#define CXL_NON_DEVICE_DECODER_HANDLING_INTERFACE "cxl_non_device_decoder_handling"
// ADD INTERFACE cxl_non_device_decoder_handling_interface

/* <add id="cxl_mem_downstream_port_managing_interface_t">

   Functions <fun>enable_decoder</fun> and <fun>disable_decoder</fun> are used
   to enable and disable HDM decoders on a cxl-hdm-port device.

   <insert-until text="// ADD INTERFACE cxl_mem_downstream_port_managing_interface"/>
   </add>

   <add id="cxl_mem_downstream_port_managing_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(cxl_mem_downstream_port_managing)
{
        bool (*register_port_mem_obj)(conf_object_t *obj, uint8 port_number,
                                      conf_object_t *port_mem_obj);
        void (*unregister_port_mem_obj)(conf_object_t *obj, uint8 port_number);
};

#define CXL_MEM_DOWNSTREAM_PORT_MANAGING_INTERFACE "cxl_mem_downstream_port_managing"
// ADD INTERFACE cxl_mem_downstream_port_managing_interface

#define ATOM_TYPE_cxl_type cxl_type_t
SIM_ACCESSIBLE_ATOM(cxl_type);

#if defined(__cplusplus)
}
#endif

#endif
