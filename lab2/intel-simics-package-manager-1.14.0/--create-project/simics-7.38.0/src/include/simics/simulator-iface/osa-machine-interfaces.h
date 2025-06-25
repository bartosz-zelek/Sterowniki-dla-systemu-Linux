/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_MACHINE_INTERFACES_H
#define SIMICS_SIMULATOR_IFACE_OSA_MACHINE_INTERFACES_H

#include <simics/device-api.h>
#include <simics/simulator-api.h>
#include "osa-types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="osa_machine_notification_interface_t">
   <name>osa_machine_notification_interface_t</name>
   <ndx>osa_machine_notification_interface_t</ndx>

   <ndx>notify_mode_change!osa_machine_notification interface method</ndx>
   <fun>notify_mode_change</fun> registers a callback function that will be
   called when processor <arg>cpu</arg> changes processor mode. The
   callback function will be called with the processor that changed mode
   <arg>cpu</arg>, the mode previous to the change <arg>old_mode</arg> and
   the mode after the change <arg>new_mode</arg>.

   <ndx>notify_exception!osa_machine_notification interface method</ndx>
   <fun>notify_exception</fun> registers a callback function that will be
   called when processor <arg>cpu</arg> takes an exception with exception
   number <arg>exc_num</arg>. The callback function <arg>cb</arg> will be
   called with the processor <arg>cpu</arg> causing the exception and
   exception number <arg>exc_num</arg> of that exception.

   <ndx>notify_control_reg!osa_machine_notification interface method</ndx>
   <fun>notify_control_reg</fun> registers a callback function that will be
   called when a control register, with register number <arg>reg_num</arg>, in
   processor <arg>cpu</arg> is updated. The callback function <arg>cb</arg>
   will be called with the processor <arg>cpu</arg>, register number
   <arg>reg_num</arg> and the written register <arg>value</arg> (see
   Core_Control_Register_Write documentation for more details) as
   arguments. The register number for a certain register can be retrieved with
   the <fun>get_register_number</fun> function.

   <ndx>notify_control_reg_read!osa_machine_notification interface method</ndx>
   <fun>notify_control_reg_read</fun> registers a callback function that will
   be called when a control register, with register number <arg>reg_num</arg>,
   in processor <arg>cpu</arg> is read. The callback function <arg>cb</arg>
   will be called with the processor <arg>cpu</arg> and register number
   <arg>reg_num</arg> as arguments. The register number for a certain register
   can be retrieved with the <fun>get_register_number</fun> function.

   <ndx>notify_exec_breakpoint!osa_machine_notification interface method</ndx>
   <fun>notify_exec_breakpoint</fun>,
   <ndx>notify_read_breakpoint!osa_machine_notification interface method</ndx>
   <fun>notify_read_breakpoint</fun> and
   <ndx>notify_write_breakpoint!osa_machine_notification interface method</ndx>
   <fun>notify_write_breakpoint</fun> plant breakpoints of length
   <arg>len</arg> for processor <arg>cpu</arg> on <arg>address</arg>.
   The breakpoint is of type execution, read, write respectively. The
   <arg>virt</arg> argument specifies if <arg>address</arg> is a virtual
   or physical address. The callback function <arg>cb</arg> is called when
   the breakpoint is hit.
   The arguments of the callback functions are the processor that the
   breakpoint hit on <arg>cpu</arg> and the <arg>address</arg>
   (virtual or physical depending on what the breakpoint was registered as)
   that was hit.
   Callbacks functions for <fun>notify_read_breakpoint</fun> and
   <fun>notify_write_breakpoint</fun> also gets the access size <arg>len</arg>
   of the read or write.
   The callback function for <fun>notify_write_breakpoint</fun> additionally
   has the previous value <arg>old_val</arg> at the address written and the new
   value <arg>new_val</arg> that is being written passed as arguments. Reading
   the actual memory from the callback will result in reading the new value that
   has been written as the callback is called after the write is done.
   On x86 virtual breakpoints use linear addresses (as opposed to logical
   addresses).

   For all functions, the <arg>tracker</arg> argument should be the tracker
   calling this interface. This makes it possible for a hypervisor tracker to
   handle guests differently.

   All methods that register a notification callback take <arg>data</arg> as an
   argument which will be passed on to callback function. These methods return
   a cancel ID to be used with the <fun>cancel</fun> method to cancel the
   callback. A returned value of 0 means that an error occurred and no callback
   was registered, in which case the caller is responsible for freeing the
   callback <arg>data</arg>.

   <ndx>cancel!osa_machine_notification interface method</ndx>
   <fun>cancel</fun> cancels the callback function with ID <arg>cancel_id</arg>
   and will free the callback <arg>data</arg> associated with the
   notification. This ID will have been returned from the function that
   registered the callback.

   <insert-until text="// ADD INTERFACE osa_machine_notification_interface"/>
   </add>

   <add id="osa_machine_notification_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef enum {
        OSA_Read_One_Byte = 1,
        OSA_Read_Two_Byte = 2,
        OSA_Read_Four_Byte = 4,
        OSA_Read_Eight_Byte = 8,
} osa_read_len_t;

SIM_INTERFACE(osa_machine_notification) {
        cancel_id_t (*notify_mode_change)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu,
                void (*cb)(cbdata_call_t data, conf_object_t *cpu,
                           processor_mode_t old_mode,
                           processor_mode_t new_mode),
                cbdata_register_t data);
        cancel_id_t (*notify_exception)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, int exc_num,
                void (*cb)(cbdata_call_t data, conf_object_t *cpu, int exc_num),
                cbdata_register_t data);
        cancel_id_t (*notify_control_reg)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, int reg_num,
                void (*cb)(cbdata_call_t data, conf_object_t *cpu, int reg_num,
                           uint64 value),
                cbdata_register_t data);
        cancel_id_t (*notify_exec_breakpoint)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, uint64 address, uint64 len,
                bool virt,
                void (*cb)(cbdata_call_t data, conf_object_t *cpu,
                           uint64 address),
                cbdata_register_t data);
        cancel_id_t (*notify_read_breakpoint)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, uint64 address, unsigned len,
                bool virt,
                void (*cb)(cbdata_call_t data, conf_object_t *NOTNULL cpu,
                           uint64 address, unsigned len),
                cbdata_register_t data);
        cancel_id_t (*notify_write_breakpoint)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, uint64 address, unsigned len,
                bool virt,
                void (*cb)(cbdata_call_t data, conf_object_t *NOTNULL cpu,
                           uint64 address, unsigned len, uint64 old_val,
                           uint64 new_val),
                cbdata_register_t data);
        void (*cancel)(conf_object_t *NOTNULL obj,
                       conf_object_t *NOTNULL tracker, cancel_id_t cancel_id);
        cancel_id_t (*notify_control_reg_read)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, int reg_num,
                void (*cb)(cbdata_call_t data, conf_object_t *cpu, int reg_num),
                cbdata_register_t data);
};

#define OSA_MACHINE_NOTIFICATION_INTERFACE "osa_machine_notification"
// ADD INTERFACE osa_machine_notification_interface

/* <add id="osa_machine_query_interface_t">
   <name>osa_machine_query_interface_t</name>
   <ndx>osa_machine_query_interface_t</ndx>

   <ndx>read_register!osa_machine_query interface method</ndx>
   <fun>read_register</fun> reads the register with number <arg>reg</arg>
   (number can be retrieved with <fun>get_register_number</fun>) from processor
   <arg>cpu</arg> and returns the value of that register.

   <ndx>get_register_number!osa_machine_query interface method</ndx>
   <fun>get_register_number</fun> returns the register number of the register
   with name <arg>reg</arg> from processor <arg>cpu</arg> or -1 upon error.

   <ndx>read_phys_memory!osa_machine_query interface method</ndx>
   <fun>read_phys_memory</fun> reads <arg>len</arg> bytes of memory from
   physical address <arg>addr</arg> of processor <arg>cpu</arg>. The returned
   value is an uint64 with the value if the read succeeded, otherwise nil. The
   <arg>len</arg> argument should use one of the lengths declared in the
   <type>osa_read_len_t</type> enum.

   <ndx>read_phys_bytes!osa_machine_query interface method</ndx>
   <fun>read_phys_bytes</fun> reads <arg>len</arg> bytes of memory from
   physical address <arg>addr</arg>. The length to read can be up to 1024
   bytes. The returned value is of data type containing the bytes read upon
   success or nil otherwise.

   <ndx>virtual_to_physical!osa_machine_query interface method</ndx>
   <fun>virtual_to_physical</fun> translates the virtual address
   <arg>vaddr</arg> of processor <arg>cpu</arg> to a physical address as
   translation would be for a data read. The returned value is the physical
   address as an uint64 upon success, otherwise nil. For x86 this uses linear
   to physical translation (as opposed to the logical to physical variant).

   <ndx>cpu_mode!osa_machine_query interface method</ndx>
   <fun>cpu_mode</fun> returns the current processor mode of <arg>cpu</arg>.

   <ndx>get_all_processors!osa_machine_query interface method</ndx>
   <fun>get_all_processors</fun> returns all available processors. For example,
   when detecting parameters, a tracker should use its known processors if the
   system is enabled, otherwise it can get them via get_all_processors. For
   hypervisor configurations, the tracker framework must be enabled in order to
   detect parameters for a guest.

   <ndx>get_exception_number!osa_machine_query interface method</ndx>
   <fun>get_exception_number</fun> returns the exception number of the
   exception with name <arg>name</arg> from processor <arg>cpu</arg>.
   Returns -1 if no exception with the given name exists.

   For all functions, the <arg>tracker</arg> argument should be the tracker
   calling this interface. This makes it possible for a hypervisor tracker to
   handle guests differently.

   <insert-until text="// ADD INTERFACE osa_machine_query_interface"/>
   </add>

   <add id="osa_machine_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(osa_machine_query) {
        uint64 (*read_register)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, int reg);
        int (*get_register_number)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, const char *reg);
        attr_value_t (*read_phys_memory)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, physical_address_t addr,
                osa_read_len_t len);
        attr_value_t (*read_phys_bytes)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, physical_address_t paddr,
                unsigned len);
        attr_value_t (*virtual_to_physical)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, uint64 vaddr);
        processor_mode_t (*cpu_mode)(conf_object_t *NOTNULL obj,
                                     conf_object_t *NOTNULL tracker,
                                     conf_object_t *NOTNULL cpu);
        attr_value_t (*get_all_processors)(conf_object_t *NOTNULL obj,
                                           conf_object_t *NOTNULL tracker);
        int (*get_exception_number)(
                conf_object_t *NOTNULL obj, conf_object_t *NOTNULL tracker,
                conf_object_t *NOTNULL cpu, const char *name);
};

#define OSA_MACHINE_QUERY_INTERFACE "osa_machine_query"
// ADD INTERFACE osa_machine_query_interface

#if defined(__cplusplus)
}
#endif

#endif  /* ! SIMICS_SIMULATOR_IFACE_OSA_MACHINE_INTERFACES_H */

