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

#ifndef SIMICS_DEVS_PCI_H
#define SIMICS_DEVS_PCI_H

#include <simics/base/types.h>
#include <simics/base/memory-transaction.h>
#include <simics/base/transaction.h>
#include <simics/pywrap.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define INTERNAL_FIELD(f) _internal_ ## f

/* <add id="devs api types">
   <name index="true">pci_memory_transaction_t</name>
   <insert id="pci_memory_transaction_t DOC"/>
   </add> */

/* <add id="pci_memory_transaction_t DOC">
   <ndx>pci_memory_transaction_t</ndx>
   <doc>
   <doc-item name="NAME">pci_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="pci_memory_transaction_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   
   The <type>pci_memory_transaction_t</type> is used for memory accesses
   initiated by PCI devices.

   <note>All struct fields are internal and should never be used
   directly.</note>

   A <type>generic_transaction_t</type> can be converted to a
   <type>pci_memory_transaction_t</type> via the
   <fun>SIM_pci_mem_trans_from_generic()</fun> function. Never explicitly cast
   one struct to the other, always use the Simics API functions.</doc-item>
   <doc-item name="SEE ALSO">
     <fun>SIM_pci_mem_trans_from_generic</fun>,
     <type>generic_transaction_t</type>
   </doc-item>
   </doc></add>

   <add-type id="pci_memory_transaction_t def"></add-type>
*/
typedef struct pci_memory_transaction {
        generic_transaction_t INTERNAL_FIELD(s);
        uint32 INTERNAL_FIELD(original_size);

        int INTERNAL_FIELD(bus_address);

        int INTERNAL_FIELD(bus_number);
        int INTERNAL_FIELD(device_number);
        int INTERNAL_FIELD(function_number);

        uint32 INTERNAL_FIELD(tlp_prefix);
} pci_memory_transaction_t;

#undef INTERNAL_FIELD

// The documentation for these EXPORTED methods are in mop.c and in the API RM        
EXPORTED pci_memory_transaction_t *SIM_pci_mem_trans_from_generic(
        generic_transaction_t *NOTNULL mop);

EXPORTED uint16 VT_get_pci_mem_op_requester_id(
        const pci_memory_transaction_t *NOTNULL mop);

EXPORTED uint32 VT_get_pci_mem_op_tlp_prefix(
        const pci_memory_transaction_t *NOTNULL mop);

EXPORTED void VT_set_pci_mem_op_tlp_prefix(
        pci_memory_transaction_t *NOTNULL mop,
        uint32 tlp_prefix);


#define DEPRECATED_FUNC(f) _deprecated_ ## f

/* <add id="pci_device_interface_t">
   This interface is implemented by all PCI devices (including bridges).

   <note>The <var>interrupt_acknowledge</var> and <var>special_cycle</var>
   functions are deprecated. The functions can be left
   unimplemented (NULL).</note>

   <insert-until text="// ADD INTERFACE pci_device_interface"/>
   </add>

   <add id="pci_device_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_device) {
        void (*bus_reset)(conf_object_t *obj);

        /* Deprecated; interrupt_acknowledge, special_cycle */
        int (*DEPRECATED_FUNC(interrupt_acknowledge))(conf_object_t *obj);
        void (*DEPRECATED_FUNC(special_cycle))(conf_object_t *obj,
                                               uint32 value);

        /* System Error */
        void (*system_error)(conf_object_t *obj);

        /* peer-to-peer interrupt mechanism */
        void (*interrupt_raised)(conf_object_t *obj, int pin);
        void (*interrupt_lowered)(conf_object_t *obj, int pin);
};

#define PCI_DEVICE_INTERFACE "pci_device"
// ADD INTERFACE pci_device_interface

#undef DEPRECATED_FUNC

/* <add id="pci_bus_interface_t">
   This interface is implemented by all PCI buses and its functions are
   accessed by PCI devices. For further details, see <cite>Model Builder
   User's Guide</cite>, chapter "Modeling PCI Devices".

   <note>The <var>memory_access</var> function is deprecated and must not be
   called (as it is not implemented by Simics pci-bus object). The
   <var>interrupt_acknowledge</var>, <var>special_cycle</var>,
   <var>add_default</var> and <var>remove_default</var> functions are also
   deprecated, but they are still implemented by Simics pci-bus and thus
   callable.</note>

   <insert-until text="// ADD INTERFACE pci_bus_interface"/>
   </add>
   <add id="pci_bus_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_bus) {
        /* Deprecated; memory_access */
        exception_type_t (*memory_access)(
                conf_object_t *obj, generic_transaction_t *NOTNULL mem_op);
        void (*raise_interrupt)(conf_object_t *obj, conf_object_t *NOTNULL dev,
                                int pin);
        void (*lower_interrupt)(conf_object_t *obj, conf_object_t *NOTNULL dev,
                                int pin);
        /* Deprecated; interrupt_acknowledge */
        int (*interrupt_acknowledge)(conf_object_t *obj);
        int (*add_map)(conf_object_t *obj, conf_object_t *dev,
                       addr_space_t space, conf_object_t *target,
                       map_info_t info);
        int (*remove_map)(conf_object_t *obj, conf_object_t *dev,
                          addr_space_t space, int function);
	void (*set_bus_number)(conf_object_t *obj, int bus_id);
	void (*set_sub_bus_number)(conf_object_t *obj, int bus_id);
        /* Deprecated; add_default, remove_default */
        void (*add_default)(conf_object_t *obj, conf_object_t *dev,
                            addr_space_t space, conf_object_t *target,
                            map_info_t info);
        void (*remove_default)(conf_object_t *obj, addr_space_t space);
        void (*bus_reset)(conf_object_t *obj);
        /* Deprecated; special_cycle */
        void (*special_cycle)(conf_object_t *obj, uint32 value);
        void (*system_error)(conf_object_t *obj);

        int (*get_bus_address)(conf_object_t *obj, conf_object_t *NOTNULL dev);

        void (*set_device_status)(conf_object_t *obj, int device, int function,
                                  int enabled);

        /* Memory spaces */
        conf_object_t *(*configuration_space)(conf_object_t *obj);
        conf_object_t *(*io_space)(conf_object_t *obj);
        conf_object_t *(*memory_space)(conf_object_t *obj);
};
#define PCI_BUS_INTERFACE		"pci_bus"
// ADD INTERFACE pci_bus_interface

/*
 <add id="pci_upstream_interface_t">
   This interface is implemented by PCI(e) buses and
   bridges to handle upstream transactions (primarily
   memory or IO).

   A PCI(e) bus typically forwards all transactions to
   an upstream bridge.

   PCI(e) bridges either forward a transaction to its primary
   side bus or initiate a downstream operation
   on the secondary bus using the <iface>pci_downstream</iface>
   interface.

   It should be noted that a PCI bridge using
   this interface will see peer-to-peer traffic
   from its secondary interface; this is a difference
   compared to real hardware where such transactions
   never involve the bridge. To behave like real
   hardware, PCI bridges should send all peer-to-peer
   traffic downstream.
   <insert-until text="// ADD INTERFACE pci_upstream_interface"/>
   </add>

   <add id="pci_upstream_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(pci_upstream) {
        exception_type_t (*operation)(conf_object_t *obj,
                                      generic_transaction_t *mem_op,
                                      addr_space_t space);
};
#define PCI_UPSTREAM_INTERFACE   "pci_upstream"
// ADD INTERFACE pci_upstream_interface

/*
 <add id="pci_upstream_operation_interface_t">

   This interface is implemented by PCI(e) buses to handle upstream
   transactions (primarily memory or IO) from PCI(e) devices.

   The <var>initiator</var> should be the PCI device object itself. There are
   currently no requirements on this object in terms of interfaces. It is
   intended for future use and could be left NULL.

   The 16-bit Requester ID is provided by the <var>rid</var> parameter. This is
   the ID that uniquely identifies the initiator in a PCIe hierarchy, also more
   commonly known as the BDF (Bus/Device/Function). This ID can be left blank
   (0) by non-PCIe devices if the BDF is not known.

   Return value is of type <type>exception_type_t</type> and maps, just like
   all other PCI interfaces, onto PCI concepts accordingly:
   <dl>
     <dt>Sim_PE_No_Exception</dt><dd>OK</dd>
     <dt>Sim_PE_IO_Not_Taken</dt><dd>Master Abort</dd>
     <dt>Sim_PE_IO_Error</dt><dd>Target Abort</dd>
   </dl>
   Other return values are currently unexpected.

   <insert-until text="// ADD INTERFACE pci_upstream_operation_interface"/>
   </add>

   <add id="pci_upstream_operation_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(pci_upstream_operation) {
        exception_type_t (*read)(conf_object_t *obj,
                                 conf_object_t *initiator,
                                 uint16 rid,
                                 addr_space_t space,
                                 physical_address_t address,
                                 buffer_t buffer);
        exception_type_t (*write)(conf_object_t *obj,
                                  conf_object_t *initiator,
                                  uint16 rid,
                                  addr_space_t space,
                                  physical_address_t address,
                                  bytes_t buffer);
};
#define PCI_UPSTREAM_OPERATION_INTERFACE   "pci_upstream_operation"
// ADD INTERFACE pci_upstream_operation_interface

/*
  <add id="pci_downstream_interface_t">
   The <iface>pci_downstream</iface> interface is implemented by
   PCI(e) buses to handle downstream transactions.
   <insert-until text="// ADD INTERFACE pci_downstream_interface"/>
   </add>

   <add id="pci_downstream_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(pci_downstream) {
        exception_type_t (*operation)(conf_object_t *obj,
                                      generic_transaction_t *mem_op,
                                      addr_space_t space);
};
#define PCI_DOWNSTREAM_INTERFACE   "pci_downstream"
// ADD INTERFACE pci_downstream_interface

/* <add id="pci_bridge_interface_t">
   This interface is implemented by all PCI bridges.
   For interrupts, the initiating device is specified with a pci-bus object and
   PCI device number pair, and the pin represents PCI interrupt lines (A, B, C,
   or D) as numbers in the range of 0 to 3.

   <insert-until text="// ADD INTERFACE pci_bridge_interface"/>
   </add>
   <add id="pci_bridge_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_bridge) {
        void (*system_error)(conf_object_t *obj);
        void (*raise_interrupt)(conf_object_t *obj, conf_object_t *pci_bus,
                                int device, int pin);
        void (*lower_interrupt)(conf_object_t *obj, conf_object_t *pci_bus,
                                int device, int pin);
};

#define PCI_BRIDGE_INTERFACE		"pci_bridge"
// ADD INTERFACE pci_bridge_interface


/* <add id="pci_interrupt_interface_t">
   This interface should only be used when a device other than the bridge
   handles PCI interrupts on the PCI bus. The initiating device is specified
   with a PCI device number, and the pin represents PCI interrupt lines
   (A, B, C, or D) as numbers in the range of 0 to 3.

   <insert-until text="// ADD INTERFACE pci_interrupt_interface"/>

   </add>
   <add id="pci_interrupt_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_interrupt) {
        void (*raise_interrupt)(conf_object_t *obj, conf_object_t *pci_bus,
                                int device, int pin);
        void (*lower_interrupt)(conf_object_t *obj, conf_object_t *pci_bus,
                                int device, int pin);
};

#define PCI_INTERRUPT_INTERFACE		"pci_interrupt"
// ADD INTERFACE pci_interrupt_interface

#define DEVICE_CONF_FUNC      0xff

/*
   <add-type id="pci_interrupt_pin_t def">
   </add-type>
*/
typedef enum {
        /* As encoded in the PCIe spec; Interrupt Pin register */
        PCI_INTERRUPT_INTA     = 1,
        PCI_INTERRUPT_INTB     = 2,
        PCI_INTERRUPT_INTC     = 3,
        PCI_INTERRUPT_INTD     = 4
} pci_interrupt_pin_t;

/*
   <add-type id="pcie_message_type_t def">
   </add-type>
*/
typedef enum {
        /* Address Translation */
        PCIE_ATS_Invalidate = 0x01,
        PCIE_ATS_Invalidate_Completion = 0x02,
        PCIE_PRS_Request    = 0x04,
        PCIE_PRS_Response   = 0x05,

        PCIE_Latency_Tolerance_Reporting = 0x10,
        PCIE_Optimized_Buffer_Flush_Fill = 0x12,

        /* INTx emulation */
        PCIE_Msg_Assert_INTA       = 0x20,
        PCIE_Msg_Assert_INTB       = 0x21,
        PCIE_Msg_Assert_INTC       = 0x22,
        PCIE_Msg_Assert_INTD       = 0x23,
        PCIE_Msg_Deassert_INTA     = 0x24,
        PCIE_Msg_Deassert_INTB     = 0x25,
        PCIE_Msg_Deassert_INTC     = 0x26,
        PCIE_Msg_Deassert_INTD     = 0x27,

        /* Power Management */
        PCIE_PM_Active_State_Nak   = 0x14,
        PCIE_PM_PME                = 0x18,
        PCIE_PM_Turn_Off           = 0x19,
        PCIE_PM_PME_TO_Ack         = 0x1B,

        /* Error Messages */
        PCIE_ERR_COR               = 0x30,
        PCIE_ERR_NONFATAL          = 0x31,
        PCIE_ERR_FATAL             = 0x33,

        /* Locked Transaction */
        PCIE_Unlock                = 0x00,

        /* Slot Power Limit */
        PCIE_Set_Slot_Power_Limit  = 0x50,

        PCIE_Precision_Time_Measurement = 0x52,

        /* Hot Plug Messages */
        PCIE_HP_Power_Indicator_On        = 0x45,
        PCIE_HP_Power_Indicator_Blink     = 0x47,
        PCIE_HP_Power_Indicator_Off       = 0x44,
        PCIE_HP_Attention_Button_Pressed  = 0x48,
        PCIE_HP_Attention_Indicator_On    = 0x41,
        PCIE_HP_Attention_Indicator_Blink = 0x43,
        PCIE_HP_Attention_Indicator_Off   = 0x40,

        PCIE_Vendor_Defined_Type_0 = 0x7e,
        PCIE_Vendor_Defined_Type_1 = 0x7f,

        PCIE_Locked_Transaction    = 0x00,  // legacy name for PCIE_Unlock

        /* Data Link Layer (virtual) Messages

           NOTE: these messages only exist on Simics simulator, as they are
           normally part of the Data Link Layer which is below the level of
           abstraction for Simics PCIe models

           According to PCIe rev 2.0, when a target receives a message that it
           does not recognize or support, except for the "Vendor Defined Type
           1" message, it should treat the request as an "Unsupported Request"
           and report it accordingly (see sec 2.3.1 for reference).
    
           Hence models that comply with rev 2.0 must be updated to either
           1) handle these messages or 2) ignore these messages.

           Ideally we would like to use a new pcie_link interface to transmit
           this information - see bug 17849 for more info. */
        PCIE_DLL_Link_Down = -1,
        PCIE_DLL_Link_Up = -2,
} pcie_message_type_t;

/*
   <add-type id="pcie_ecs_t def">
   </add-type>
*/
typedef enum {
        /* ERR_COR Subclass as encoded in the PCIe spec. */
        PCIE_ECS_Legacy = 0x00,
        PCIE_ECS_SIG_SFW = 0x01,
        PCIE_ECS_SIG_OS = 0x02,
        PCIE_ECS_Extended = 0x03
} pcie_ecs_t;

/* <add id="pci_multi_function_device_interface_t">
   This interface is intended for PCI devices that want to have multiple
   functions implemented within the same device instance, as compared to the
   scheme of having each PCI function defined in a separate instance.

   <fun>supported_functions</fun> is called from the pci-bus to find out how
   many functions are supported by this device. The expected return value is a
   list of tuples, where each tuple contains the PCI function number followed
   by one or two elements controlling what to map into the bus'
   config-space. The list can consist of any number of tuples using any and all
   of the supported formats:

   <ul>

   <li>(fun, str); maps the named port-interface on the object implementing the
   current interface. For example, the name of the bank that provides the
   configuration registers for this function.</li>

   <li>(fun, obj); maps the object expecting configuration access is handled by
   io_memory implemented directly on the object.</li>

   <li>(fun, obj, str); maps the named port-interface on the provided
   object.</li>

   </ul>

   The list can contain a mix of formats, each entry corresponds to a unique
   mapping of a function.

   For ARI enabled PCIe devices, the device number is assumed to be zero thus
   it is an error to try to claim a function number greater than seven while
   connecting with a device number greater than zero.

   <insert-until text="// ADD INTERFACE pci_multi_function_device_interface"/>
   </add>

   <add id="pci_multi_function_device_interface_exec_context">
   Global Context for all methods.
   </add> */
SIM_INTERFACE(pci_multi_function_device) {
        attr_value_t (*supported_functions)(conf_object_t *obj);
};
#define PCI_MULTI_FUNCTION_DEVICE_INTERFACE "pci_multi_function_device"
// ADD INTERFACE pci_multi_function_device_interface

/* <add id="pcie_device_interface_t">

   This interface must be implemented by all PCIe devices that can receive
   downstream transactions.

   <fun>connected</fun> and <fun>disconnected</fun> are used to indicate that
   the device is (dis)connected to <arg>port_obj</arg> with device id
   <arg>device_id</arg> and may use the <iface>pcie_map</iface> interface to
   add/remove functions and claim/release other resources. These functions
   should only be called once for each (dis)connected physical device no matter
   how many physical or virtual functions it has. The <arg>device_id</arg>
   contains the device number (D part of B:D:F) and should use 0 as the function
   number (F part of B:D:F).

   <fun>hot_reset</fun> is used to indicate that a Hot Reset has been signaled
   on the PCIe link to which the device is connected. It is up to the device to
   reset functions and other resources mapped using the <iface>pcie_map</iface>
   interface.

   Note: This interface is considered tech-preview and may change without
   notice.

   <insert-until text="// ADD INTERFACE pcie_device_interface"/>
   </add>

   <add id="pcie_device_interface_exec_context">
   Global Context for all methods.
   </add>
*/

SIM_INTERFACE(pcie_device) {
        void (*connected)(conf_object_t *obj, conf_object_t *port_obj,
                          uint16 device_id);
        void (*disconnected)(conf_object_t *obj, conf_object_t *port_obj,
                             uint16 device_id);
        void (*hot_reset)(conf_object_t *obj);
};

#define PCIE_DEVICE_INTERFACE "pcie_device"
// ADD INTERFACE pcie_device_interface

/*
 <add id="pcie_map_interface_t">

   This interface is used to claim ranges in PCIe address spaces and to manage
   virtual functions.

   Functions <fun>add_map</fun> and <fun>del_map</fun> are used to add and
   remove maps, <arg>map_obj</arg> will be mapped in the address space
   indicated by <arg>type</arg> according to the information in <arg>info</arg>.

   Functions <fun>add_function</fun>, <fun>del_function</fun>,
   <fun>enable_function</fun> and <fun>disable_function</fun> are used to add,
   delete, enable and disable functions, both virtual and physical. The
   supplied <arg>map_obj</arg> must be a valid map-target, it will receive both
   Config and Message transactions. For Message transactions, the device id (not
   including bus number) will be in bits 55:48 of the 64-bit address. For
   Config transactions, the device id is not part of the address. The supplied
   <arg>device_and_function</arg> passed to these methods should contain the
   device and function number (D:F part of B:D:F).

   The function <fun>get_device_id</fun> returns the current device id of
   <arg>dev_obj</arg>, as a 16 bit number. Note that the bus number part is
   saved immediately after the RC/Switch sets it, even if no successful
   Config-Write has been made.

   Note: This interface is considered tech-preview and may change without
   notice.

   <insert-until text="// ADD INTERFACE pcie_map_interface"/>
   </add>

   <add id="pcie_map_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        PCIE_Type_Not_Set,
        PCIE_Type_Mem,
        PCIE_Type_IO,
        PCIE_Type_Cfg,
        PCIE_Type_Msg,
        PCIE_Type_Other,
} pcie_type_t;

SIM_INTERFACE(pcie_map) {
        void (*add_map)(conf_object_t *obj, conf_object_t *map_obj,
                        map_info_t info, pcie_type_t type);
        void (*del_map)(conf_object_t *obj, conf_object_t *map_obj,
                        physical_address_t base, pcie_type_t type);
        void (*add_function)(conf_object_t *obj, conf_object_t *map_obj,
                             uint16 device_and_function);
        void (*del_function)(conf_object_t *obj, conf_object_t *map_obj,
                             uint16 device_and_function);
        void (*enable_function)(conf_object_t *obj, uint16 device_and_function);
        void (*disable_function)(conf_object_t *obj,
                                 uint16 device_and_function);
        uint16 (*get_device_id)(conf_object_t *obj, conf_object_t *dev_obj);
};

#define PCIE_MAP_INTERFACE "pcie_map"
// ADD INTERFACE pcie_map_interface

/*
 <add id="pcie_port_control_interface_t">

   This interface is used to configure instances of the
   <class>pcie-downstream-port</class>

   The <fun>set_secondary_bus_number</fun> method should be called whenever the
   Secondary Bus Number register in the Type 1 Configuration Header of this
   downstream port changes.

   The <fun>hot_reset</fun> method sends a hot reset to all downstream devices

   Note: This interface is considered tech-preview and may change without
   notice.

   <insert-until text="// ADD INTERFACE pcie_port_control_interface"/>
   </add>

   <add id="pcie_port_control_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(pcie_port_control) {
        void (*set_secondary_bus_number)(conf_object_t *obj, uint64 value);
        void (*hot_reset)(conf_object_t *obj);
};

#define PCIE_PORT_CONTROL_INTERFACE "pcie_port_control"
// ADD INTERFACE pcie_port_control_interface


/*
   <add-type id="pcie_hotplug_pd_t def">
   </add-type>
*/
typedef enum {
        PCIE_HP_PD_Adapter_Not_Present = 0,
        PCIE_HP_PD_Adapter_Present = 1
} pcie_hotplug_pd_t;

/*
   <add-type id="pcie_hotplug_mrl_t def">
   </add-type>
*/
typedef enum {
        PCIE_HP_MRL_Closed = 0,
        PCIE_HP_MRL_Open = 1
} pcie_hotplug_mrl_t;

/*
 <add id="pcie_hotplug_events_interface_t">

   This interface should be implemented by Hot-Plug capable Downstream Ports.
   Such devices should be either a Downstream Port of a PCI Express Switch or a
   Root Port of PCI Express Root Complex.

   The device that implements this interface should on the invocation of any
   of the methods in this interface should do the following:

   1. If the applicable Hot-Plug capability has been enabled, set the new value
      of the corresponding Hot-Plug event in the applicable register.
   2. If the value changed from its previous value, set the changed register
      value for the applicable Hot-Plug event.
   3. If the value changed from its previous value, notify software using an
      interrupt if notifications are enabled for the applicable Hot-Plug event
      and an interrupt mechanism has been configured by the software (MSI/MSI-X
      or PCI-compatible INTx Emulation).

   The <fun>presence_change()</fun> method is called by the pcie-downstream-port
   (if its upstream target implements this interface) when it goes from having
   no adapter present to having an adapter present and vice versa.

   <insert-until text="// ADD INTERFACE pcie_hotplug_events_interface"/>
   </add>

   <add id="pcie_hotplug_events_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(pcie_hotplug_events) {
        void (*presence_change)(conf_object_t *obj, pcie_hotplug_pd_t state);
        void (*power_fault)(conf_object_t *obj);
        void (*attention_button_pressed)(conf_object_t *obj);
        void (*mrl_sensor)(conf_object_t *obj, pcie_hotplug_mrl_t state);
        void (*data_link_layer)(conf_object_t *obj, bool is_active);
};

#define PCIE_HOTPLUG_EVENTS_INTERFACE "pcie_hotplug_events"
// ADD INTERFACE pcie_hotplug_events_interface

/*
 <add id="pcie_link_training_interface_t">

   Trigger link training on <param>obj</param>. <param>device_id</param>
   provides the device id of device connected to <param>obj</param> which will
   be the link training target.

   <insert-until text="// ADD INTERFACE pcie_link_training_interface"/>
   </add>

   <add id="pcie_link_training_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(pcie_link_training) {
        bool (*trigger)(conf_object_t *obj, uint16 device_id);
};

#define PCIE_LINK_TRAINING_INTERFACE "pcie_link_training"
// ADD INTERFACE pcie_link_training_interface

#define ATOM_TYPE_pcie_type pcie_type_t
SIM_ACCESSIBLE_ATOM(pcie_type);

#define ATOM_TYPE_pcie_requester_id uint16
SIM_ACCESSIBLE_ATOM(pcie_requester_id);

#define ATOM_TYPE_pcie_device_id uint16
SIM_ACCESSIBLE_ATOM(pcie_device_id);

#define ATOM_TYPE_pcie_msg_type pcie_message_type_t
SIM_ACCESSIBLE_ATOM(pcie_msg_type);

#define ATOM_TYPE_pcie_ecs pcie_ecs_t
SIM_ACCESSIBLE_ATOM(pcie_ecs);

typedef enum {
        PCIE_Msg_Route_Not_Set,
        PCIE_Msg_Route_Upstream,   /* To RC */
        PCIE_Msg_Route_Address,    /* IO and Memory */
        PCIE_Msg_Route_ID,         /* Messages */
        PCIE_Msg_Route_Broadcast,  /* From RC */
        PCIE_Msg_Route_Terminate,  /* INTx */
        PCIE_Msg_Route_Gather,     /* PME_Turn_Off_Ack */
} pcie_msg_route_t;
#define ATOM_TYPE_pcie_msg_route pcie_msg_route_t
SIM_ACCESSIBLE_ATOM(pcie_msg_route);

/*
<add id="pcie_pasid_info_t DOC">
  <ndx>pcie_pasid_info_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_pasid_info_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type contain the PASID Information in the pcie_pasid transaction atom.
    </doc-item>
  </doc>
</add>
*/
typedef union {
    struct {
        uint32 pasid:20;    // PASID
        uint32 exe:1;       // Execute Requested
        uint32 priv:1;      // Privileged Mode Requested
    } field;
    uint32 u32;
} pcie_pasid_info_t;
#define ATOM_TYPE_pcie_pasid uint32
SIM_ACCESSIBLE_ATOM(pcie_pasid);

typedef enum {
        PCIE_AT_Not_Set,
        PCIE_AT_Untranslated,
        PCIE_AT_Translation_Request,
        PCIE_AT_Translated,
} pcie_at_t;
#define ATOM_TYPE_pcie_at pcie_at_t
SIM_ACCESSIBLE_ATOM(pcie_at);

typedef enum {
        PCIE_Error_Not_Set,
        PCIE_Error_Unsupported_Request,
        PCIE_Error_Completer_Abort,
        PCIE_Error_Master_Abort,
        PCIE_Error_No_Error,
        PCIE_Error_Configuration_Request_Retry_Status,
} pcie_error_t;

typedef struct {
        pcie_error_t val;
} pcie_error_ret_t;

#define ATOM_TYPE_pcie_error_ret pcie_error_ret_t *
SIM_ACCESSIBLE_ATOM(pcie_error_ret);

typedef struct {
        uint64 val;
} pcie_byte_count_ret_t;

#define ATOM_TYPE_pcie_byte_count_ret pcie_byte_count_ret_t *
SIM_ACCESSIBLE_ATOM(pcie_byte_count_ret);

#define ATOM_TYPE_pcie_destination_segment uint8
SIM_ACCESSIBLE_ATOM(pcie_destination_segment);


#if defined(SIMICS_6_API) || defined(SHOW_OBSOLETE_API)
/* <add id="pcie_adapter_compat_interface_t">
   Deprecated as part of the old PCIe library. Replaced by the new PCIe library.

   This interface is used by PCI buses that want to be compatible
   with PCIe devices created with the new PCIe library. This interface is
   implemented by the legacy-pcie-upstream-adapter.

   <insert-until text="// ADD INTERFACE pcie_adapter_compat_interface"/>
   </add>
   <add id="pcie_adapter_compat_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pcie_adapter_compat) {
        void (*set_secondary_bus_number)(conf_object_t *obj,
                                         int secondary_bus_number);
};
#define PCIE_ADAPTER_COMPAT_INTERFACE		"pcie_adapter_compat"


// ADD INTERFACE pcie_adapter_compat_interface

#define DEVICE_PCIE_CONF_FUNC 0xfe

/* <add id="pci_express_interface_t">
   Deprecated as part of the old PCIe library. Replaced by the new PCIe library.

   <note>This interface is deprecated. The <iface>transaction</iface> interface
   implemented by the pcie-bus (upstream and downstream ports) can be used to
   send any type of message with any type of routing by adding  related
   <var>pcie_type</var> and <var>pcie_msg_route</var> ATOMs to
   the transaction.</note>

   This interface can be implemented by any PCI Express device, switch or
   endpoint. It is also implemented by the pci-bus, which will pass it on
   to the other end; e.g. if the endpoint sends a message, the pci-bus will
   pass it on to the bridge (downport), and if the downport sends it, it will
   be broadcasted to all devices on the bus.

   Please note that message routing is not supported. The implications of this
   limitation is that a switch does not know if the message is intended for the
   switch itself or for some device connected to it. In addition, since the bus
   does not replace the <param>src</param> parameter when forwarding the
   message, and the bus does not support port mappings, it is impossible for a
   bus-to-bus bridge (up-port/down-port) to know from which bus the message
   originates making it impossible to forward the message to the other side.

   Another way of looking at this limitation is to say that the only message
   routing currently supported is the implicit routing that terminates at the
   receiver (Type[2:0] = 0b100), so bus-to-bus bridges should not try to
   forward the message to the other side.

   <param>src</param> is the object sending the message. <param>type</param> is
   one of:

   <insert id="pcie_message_type_t def"/>

   The contents of <param>payload</param> depends on <param>type</param>.

   <insert-until text="// ADD INTERFACE pci_express_interface"/>

   </add>
   <add id="pci_express_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_express) {
        int (*send_message)(conf_object_t *dst, conf_object_t *src,
                            pcie_message_type_t type, byte_string_t payload);
};
#define PCI_EXPRESS_INTERFACE "pci_express"
// ADD INTERFACE pci_express_interface

/* <add id="pci_express_hotplug_interface_t">
   Deprecated as part of the old PCIe library. Replaced by the new PCIe library.

   This interface is intended for PCI Express switches that need to monitor the
   status of their downports. The interface should be implemented by the bridge
   object.

   <fun>presence_change</fun> is called from the pci-bus when an device is
   added or removed from the bus. <param>is_present</param> is set to 1 when
   the device is added, and 0 when it is removed.

   <fun>inject_power_fault</fun> can be used to simulate a power fault on
   the downport. It is never called automatically.

   <fun>press_attention_button</fun> can be called to simulate the attention
   button being pressed. The switch can respond to this by e.g. raising
   an interrupt and setting appropriate status bits. It is never called
   automatically.

   <fun>set_mrl_state</fun> is similar to <fun>attention_button_press</fun>
   but controls the Manually operated Retention Latch. Set
   <param>locked</param> to 1 to simulate it being locked/closed, and 0 to
   simulate it being unlocked/opened.

   Finally, <fun>get_mrl_state</fun> returns the state of the Manually
   operated Retention Latch.

   <insert-until text="// ADD INTERFACE pci_express_hotplug_interface"/>

   </add>
   <add id="pci_express_hotplug_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(pci_express_hotplug) {
        /* This is sent when a device is added or removed from the bus. */
        void (*presence_change)(conf_object_t *dst, conf_object_t *NOTNULL src,
                                int is_present);
        void (*inject_power_fault)(conf_object_t *obj);
        void (*press_attention_button)(conf_object_t *obj);
        void (*set_mrl_state)(conf_object_t *obj, int locked);
        int  (*get_mrl_state)(conf_object_t *obj);
};
#define PCI_EXPRESS_HOTPLUG_INTERFACE "pci_express_hotplug"
// ADD INTERFACE pci_express_hotplug_interface

#endif /* 6 or SHOW_OBSOLETE_API */

#define ATOM_TYPE_pcie_requester_segment uint8
SIM_ACCESSIBLE_ATOM(pcie_requester_segment);

#define ATOM_TYPE_pcie_ats_translation_request_cxl_src bool
SIM_ACCESSIBLE_ATOM(pcie_ats_translation_request_cxl_src);

#define ATOM_TYPE_pcie_ats_translation_request_no_write bool
SIM_ACCESSIBLE_ATOM(pcie_ats_translation_request_no_write);

typedef union {
        struct {
                uint64 r:1;      // Read access
                uint64 w:1;      // Write access
                uint64 u:1;      // Untranslated Access Only
                uint64 exe:1;    // Execute Permitted
                uint64 priv:1;   // Privileged Mode Access
                uint64 global:1; // Global mapping for all PASIDs
                uint64 ama:3;    // ATS memory attributes, implementation defined
                uint64 cxl_io:1;
                uint64 n:1;      // non-snooped accesses
                uint64 s:1;      // size of translation
                uint64 translated_addr: 52;
        } field;
        uint64 u64;
} pcie_ats_translation_completion_entry_t;

typedef union {
        struct {
                uint64 global:1;  // All PASIDs
                uint64 rsvd:10;
                uint64 s:1;       // The range being invalidated is greater than 4096 bytes
                uint64 untranslated_addr: 52;
        } field;
        uint64 u64;
} pcie_ats_invalidate_request_payload_t;

#define ATOM_TYPE_pcie_ats_invalidate_request_itag uint8
SIM_ACCESSIBLE_ATOM(pcie_ats_invalidate_request_itag);

#define ATOM_TYPE_pcie_ats_invalidate_completion_itag_vector uint32
SIM_ACCESSIBLE_ATOM(pcie_ats_invalidate_completion_itag_vector);

#define ATOM_TYPE_pcie_ats_invalidate_completion_count uint8
SIM_ACCESSIBLE_ATOM(pcie_ats_invalidate_completion_count);


/*
<add id="pcie_prs_page_request_t DOC">
  <ndx>pcie_prs_page_request_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_prs_page_request_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used contain PCIe PRS page request data.
    </doc-item>
  </doc>
</add>
*/
typedef union {
        struct {
                uint64 r:1;     // Read access
                uint64 w:1;     // Write access
                uint64 l:1;     // Last
                uint64 prgi:9;  // Page request group
                uint64 page_addr: 52;
        } field;
        uint64 u64;
} pcie_prs_page_request_t;

#define ATOM_TYPE_pcie_prs_page_request uint64
SIM_ACCESSIBLE_ATOM(pcie_prs_page_request);

/*
<add id="pcie_prs_response_code_t DOC">
  <ndx>pcie_prs_response_code_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_prs_page_code_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used to indicate PCIe PRS Response Message Code.
    </doc-item>
  </doc>
</add>
*/
typedef enum {
        PCIE_PRS_Response_Success = 0,
        PCIE_PRS_Response_Invalid_Request = 1,
        PCIE_PRS_Response_Failure = 15,
} pcie_prs_response_code_t;

/*
<add id="pcie_prs_page_group_response_t DOC">
  <ndx>pcie_prs_page_group_response_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_prs_page_group_response_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used contain PCIe PRS page response data.
    </doc-item>
  </doc>
</add>
*/
typedef union {
        struct {
                uint16 prgi:9;  // Page request group
                uint16 rsvd:3;
                uint16 response_code:4;  // Response code
        } field;
        uint16 u16;
} pcie_prs_page_group_response_t;

#define ATOM_TYPE_pcie_prs_page_group_response uint16
SIM_ACCESSIBLE_ATOM(pcie_prs_page_group_response);

#define ATOM_TYPE_pcie_prs_stop_marker bool
SIM_ACCESSIBLE_ATOM(pcie_prs_stop_marker);

/*
<add id="pcie_link_speed_t DOC">
  <ndx>pcie_link_speed_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_link_speed_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used to indicate PCIe link speeds.
    </doc-item>
  </doc>
</add>
*/
typedef enum {
        PCIE_Link_Speed_Undefined = 0,
        PCIE_Link_Speed_2_5       = 1,
        PCIE_Link_Speed_5         = 2,
        PCIE_Link_Speed_8         = 4,
        PCIE_Link_Speed_16        = 8,
        PCIE_Link_Speed_32        = 16,
        PCIE_Link_Speed_64        = 32,
} pcie_link_speed_t;

/*
<add id="pcie_link_width_t DOC">
  <ndx>pcie_link_width_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_link_width_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used to indicate PCIe link widths.
    </doc-item>
  </doc>
</add>
*/
typedef enum {
        PCIE_Link_Width_Undefined  = 0,
        PCIE_Link_Width_x1         = 1,
        PCIE_Link_Width_x2         = 2,
        PCIE_Link_Width_x4         = 4,
        PCIE_Link_Width_x8         = 8,
        PCIE_Link_Width_x16        = 16,
} pcie_link_width_t;


/*
<add id="pcie_link_negotiation_t DOC">
  <ndx>pcie_link_negotiation_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_link_negotiation_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used to send desired (maximum) link attributes to a peer who
      then sets the negotiated link speed and width based on the supported
      values of both peers.
    </doc-item>
  </doc>
</add>
*/
typedef struct {
        pcie_link_speed_t maximum_link_speed;
        pcie_link_width_t maximum_link_width;
        pcie_link_speed_t negotiated_link_speed;
        pcie_link_width_t negotiated_link_width;
} pcie_link_negotiation_t;
/* </add-type> */

#define ATOM_TYPE_pcie_link_negotiation pcie_link_negotiation_t *
SIM_ACCESSIBLE_ATOM(pcie_link_negotiation);

/*
<add id="pcie_ide_secured_t DOC">
  <ndx>pcie_ide_secured_t</ndx>
  <doc>
    <doc-item name="NAME">pcie_ide_secured_t</doc-item>
    <doc-item name="DESCRIPTION">
      This type is used contain PCIe IDE Secured TLP.
    </doc-item>
  </doc>
</add>
*/
typedef struct {
    bool t;  // Trusted Execution
    bool k;  // Key set
    bool m;  // MAC included
    bool p;  // PCRC included
    uint8 sub_stream;
    uint8 stream_id;
    uint8 pr_sent_counter;
} pcie_ide_secured_t;

#define ATOM_TYPE_pcie_ide_secured pcie_ide_secured_t *
SIM_ACCESSIBLE_ATOM(pcie_ide_secured);

#if defined(__cplusplus)
}
#endif

#endif
