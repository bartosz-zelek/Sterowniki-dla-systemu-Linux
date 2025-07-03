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

#ifndef SIMICS_ARCH_RISC_V_CUSTOMIZE_H
#define SIMICS_ARCH_RISC_V_CUSTOMIZE_H

#include <simics/processor/types.h>
#include <simics/processor/generic-spr.h>
#include <simics/pywrap.h>
#include <simics/arch/risc-v.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="riscv_instruction_action_interface_t">

    The <iface>riscv_instruction_action</iface> interface helps with
    implementing semantics for user defined instruction.

    The <arg>cpu</arg> argument in all methods below is the processor object
    implementing this interface.

    <b>X registers</b>
    <fun>read_X_register</fun> return the current value of X register <tt>number</tt>.
    <fun>write_X_register</fun> updates the value of X register <tt>number</tt> to
    <tt>value</tt>.
    To help with disassembly <fun>name_X_register</fun> returns the name of the X
    register <tt>number</tt>.

    <b>Control and status registers, CSRs</b>
    These accesses are not seen as instruction accesses, all access checks are
    bypassed and no exception will be thrown.
    <fun>read_CSR</fun> returns current value of the CSR at <tt>address</tt>.
    <fun>write_CSR</fun> updates the value of the CSR at <tt>address</tt> to
    <tt>value</tt>. Not all bits of all CSRs is writable.

    <b>Memory accesses using logical address</b>
    A logical address is passed through MMU based on current mode. This means that
    an access can raise exception for page fault or illegal access.
    <fun>read_memory</fun> returns a value of <arg>size></arg> bytes zero extended to
    64 bits from memory at <tt>address</tt>/
    <fun>write_memory</fun> writes <arg>size</arg> bytes to memory at <arg>address</arg>.
    Both read_memory and write_memory can raise exception for unaligned data access if
    the core does not support unaligned accesses.

    <fun>load_memory_buf</fun> loads <tt>buf.len</tt> bytes from <arg>address</arg>
    to <arg>buf</arg>.
    <fun>store_memory_buf</fun> writes <tt>buf.len</tt> bytes from <arg>buf</arg> to
    <arg>address</arg>.
    These methods do not raise exception for unaligned accesses, instead large and/or
    unaligned accesses are broken down to multiple smaller aligned accesses.

    <b>Other</b>
    <fun>get_current_cpu_mode</fun> returns current cpu mode.
    <fun>raise_exception</fun> raises exception with <arg>code</arg> put in th xCAUSE CSR
    and <arg>tval</arg> put in the xTVAL CSR.

   <insert-until text="// ADD INTERFACE riscv_instruction_action_interface"/>
   </add>
   <add id="riscv_instruction_action_interface_exec_context">
   Threaded Context for all methods.
   Must be only called from within an instruction emulation callback registered
   using the cpu_instruction_decoder interface.
   </add> */

SIM_INTERFACE(riscv_instruction_action) {
        uint64 (*read_x_register)(conf_object_t *cpu, uint32 number);
        void (*write_x_register)(conf_object_t *cpu, uint32 number, uint64 value);
        const char * (*name_x_register)(conf_object_t *cpu, uint32 number);

        uint64 (*read_csr)(conf_object_t *cpu, uint32 address);
        void (*write_csr)(conf_object_t *cpu, uint32 address, uint64 value);

        // Logical address
        uint64 (*read_memory)(conf_object_t *cpu, uint64 address,
                              uint32 size);
        void (*write_memory)(conf_object_t *cpu, uint64 address,
                             uint32 size, uint64 value);

        void (*load_memory_buf)(conf_object_t *cpu, uint64 address, buffer_t buf);
        void (*store_memory_buf)(conf_object_t *cpu, uint64 address, bytes_t buf);

        riscv_cpu_mode_t (*get_current_cpu_mode)(conf_object_t *cpu);

        void (*raise_exception)(conf_object_t *cpu, uint64 code, uint64 tval);
};

#define RISCV_INSTRUCTION_ACTION_INTERFACE "riscv_instruction_action"
// ADD INTERFACE riscv_instruction_action_interface

/* <add-type id="riscv_csr_access_type_t def"></add-type> */
typedef enum {
        /* Access through csr/csri instruction */
        Riscv_CSR_Instruction_Access = Sim_Gen_Spr_Instruction_Access,

        /* Access through attribute */
        Riscv_Attribute_Access = Sim_Gen_Spr_Attribute_Access,

        /* Access through int_register_interface */
        Riscv_Int_Register_Access = Sim_Gen_Spr_Int_Register_Access,
} riscv_csr_access_type_t;

/* <add-type id="riscv_csr_access_cb_t def"></add-type> */
typedef uint64 (*riscv_csr_access_cb_t)(conf_object_t *obj,
                                        conf_object_t *cpu,
                                        uint32 csr_address,
                                        uint64 value,
                                        uint64 write_mask,
                                        riscv_csr_access_type_t type);

/* <add-type id="riscv_reset_cb_t def"></add-type> */
typedef uint64 (*riscv_reset_cb_t)(conf_object_t *obj,
                                   conf_object_t *cpu);


/* <add id="riscv_custom_csr_interface_t">

    The <iface>riscv_custom_csr</iface> interface lets other Simics objects
    implement custom CSR-registers and get callback for each access.

    For the methods below, <arg>cpu</arg> is the RISC-V CPU model that is
    extended and <arg>ext_obj</arg> is the extension object.

    The <fun>register_csr</fun> method registers a custom CSR at
    <arg>csr_address</arg>.
    The arguments <arg>name</arg> and <arg>description</arg> are for
    disassembly and int_register interface.
    The argument <arg>access</arg> is the function implementing the CSR access.
    Return true if CSR is successfully registered.

    <insert id="riscv_csr_access_type_t def"/>

    <insert id="riscv_csr_access_cb_t def"/>

    The <fun>register_reset</fun> method registers a reset callback for the
    extension. The <arg>reset_func</arg> callback will be called in connection
    with the core CPU reset flow.

    <insert id="riscv_reset_cb_t def"/>

   <insert-until text="// ADD INTERFACE riscv_custom_csr_interface"/>
   </add>
   <add id="riscv_custom_csr_interface_exec_context">
   Global Context for all methods.
   </add> */

SIM_INTERFACE(riscv_custom_csr) {

        bool (*register_csr)(conf_object_t *cpu,
                             conf_object_t *ext_obj,
                             uint32 csr_address,
                             const char *name,
                             const char *description,
                             riscv_csr_access_cb_t access);

        void (*register_reset)(conf_object_t *cpu,
                               conf_object_t *csr_obj,
                               riscv_reset_cb_t reset_func);
};

#define RISCV_CUSTOM_CSR_INTERFACE "riscv_custom_csr"
// ADD INTERFACE riscv_custom_csr_interface

#if defined(__cplusplus)
}
#endif

#endif /* SIMICS_ARCH_RISC_V_CUSTOMIZE_H */
