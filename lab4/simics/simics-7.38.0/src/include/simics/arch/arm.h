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

#ifndef SIMICS_ARCH_ARM_H
#define SIMICS_ARCH_ARM_H

#ifndef TURBO_SIMICS
#include <simics/base/transaction.h>
#endif
#include <simics/processor/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="devs api types">
   <name index="true">arm_mem_instr_origin_t</name>
   <insert id="arm_mem_instr_origin_t DOC"/>
   </add> */

/* <add id="arm_mem_instr_origin_t DOC">
     <ndx>arm_mem_instr_origin_t</ndx>
     <doc>
       <doc-item name="NAME">arm_mem_instr_origin_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
         <insert id="arm_mem_instr_origin_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         List of special memory operations that can be send by a ARM processor.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_mem_instr_origin_t def"></add-type> */
typedef enum {
        /* Normal load or store instructions */
        Instr_Normal_Arm = 0,

        /* Unprivileged memory access instructions. */
        Instr_Unprivileged_Load,
        Instr_Unprivileged_Store,

        /* Other loads/stores or cache affecting instructions */
        Instr_ldrex,
        Instr_strex,
        Instr_ldxp,
        Instr_stxp,

        /* Address translation instruction */
        Instr_At,

        /* Atomic read-modify-write instructions */
        Instr_Atomic,

        /* Cache maintenance instructions */
        Instr_Cache_Maintenance,

        /* Number of different of enum values, not a value in itself. */
        Instr_Count
} arm_mem_instr_origin_t;

#ifndef TURBO_SIMICS

/* <add id="devs api types">
   <name index="true">arm_memory_transaction_t</name>
   <insert id="arm_memory_transaction_t DOC"/>
   </add> */

/* <add id="arm_memory_transaction_t DOC">
   <ndx>arm_memory_transaction_t</ndx>
   <name index="true">arm_memory_transaction_t</name>
   <doc>
   <doc-item name="NAME">arm_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="arm_memory_transaction_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   This is the ARM specific memory transaction data structure.
   The generic data is stored in the <var>s</var> field.

   The <var>mode</var> field specifies the processor mode the MMU should assume
   when processing the transaction. This is the same as the current mode of the
   processor except for unprivileged load and store instructions when it is
   always <const>Sim_CPU_Mode_User</const>.

   The <var>rotate</var> field is non-zero if this transaction is from one of
   the AArch32 instructions for which an unaligned address is interpreted as an
   aligned load with the value rotated so that the addressed byte becomes the
   least significant byte if neither <reg>SCTLR.U</reg> nor <reg>SCTLR.A</reg>
   is set.

   The <var>instr_origin</var> field specifies the type of instruction that
   initiated this memory transaction.

   </doc-item>
   </doc>
   </add>

   <add-type id="arm_memory_transaction_t def"></add-type>
*/
typedef struct arm_memory_transaction {
        generic_transaction_t s;

        processor_mode_t mode;
        int rotate;
        arm_mem_instr_origin_t instr_origin;
} arm_memory_transaction_t;

EXPORTED arm_memory_transaction_t *SIM_arm_mem_trans_from_generic(
        generic_transaction_t *NOTNULL mop);

/* <add id="arm_interface_t">
   This interface is implemented by ARM processors to provide various
   functionality that is specific for this class of processors.

   The <fun>read_register_mode</fun> and <fun>write_register_mode</fun>
   functions are used to access banked copies of the registers. They are used
   just like the <iface>int_register</iface> interface <fun>read</fun> and
   <fun>write</fun> functions, except that they take an extra parameter
   <param>mode</param> that specifies which register bank should be used.
   <param>mode</param> should be the mode bits in the cpsr corresponding to
   the mode shifted right to bits 0-4.

   <insert-until text="// END INTERFACE arm_iface"/>
   </add>
   <add id="arm_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(arm) {
        uint64 (*read_register_mode)(conf_object_t *processor_obj,
                                     int reg_num, int mode);
        void (*write_register_mode)(conf_object_t *processor_obj,
                                    int reg_num, int mode, uint64 value);
};

#define ARM_INTERFACE "arm"

// END INTERFACE arm_iface

/* Interrupt numbers used in the simple_interrupt interface in non-M-profile
   cores. */
#define ARM_INT_IRQ  0
#define ARM_INT_FIQ  1
#define ARM_INT_VIRQ 2
#define ARM_INT_VFIQ 3
#define ARM_INT_SEI  4
#define ARM_INT_REI  5
#define ARM_INT_VSEI 6

/* Interrupt numbers used in the simple_interrupt interface in M-profile
   cores. 0x0-0x1ff are used for NVIC interrupt inputs.*/
#define ARM_INT_NMI     0x200
#define ARM_INT_SYSTICK 0x201

/* <add id="arm_coprocessor_interface_t">

   A coprocessor for the ARM has to provide the
   arm_coprocessor_interface. This interface
   defines the functions that will be called when
   the coprocessor instructions (cdp, ldc, mcr, mrc, mrrc, mcrr, stc)
   are executed.

   The read_register_64_bit and write_register_64_bit are used for
   mrrc and mccr instructions which read and write 64 bit values
   in to two registers.

   The interface also defines a flag, finished, which indicates
   whether a memory transfer operation is finished or not.

   The function reset is called when the cpu is reset,
   and allows the coprocessor to also do a reset,
   the argument hard_reset indicates whether the reset
   was soft (0) or hard (1).

   <insert-until text="// ADD INTERFACE arm_coprocessor_interface"/>
   </add>

   <add id="arm_coprocessor_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(arm_coprocessor) {
        void (*process_data)(conf_object_t *NOTNULL obj,
                             uint32 CRd,
                             uint32 opcode_1,
                             uint32 CRn,
                             uint32 CRm,
                             uint32 opcode_2,
                             int type);
        void (*load_coprocessor)(conf_object_t *NOTNULL obj,
                                 uint32 CRd,
                                 uint32 N,
                                 uint32 Options,
                                 uint32 value,
                                 int type);

        uint32 (*read_register)(conf_object_t *NOTNULL obj,
                                uint32 opcode_1,
                                uint32 CRn,
                                uint32 CRm,
                                uint32 opcode_2,
                                int type);
        void (*write_register)(conf_object_t *NOTNULL obj,
                               uint32 value,
                               uint32 opcode_1,
                               uint32 CRn,
                               uint32 CRm,
                               uint32 opcode_2,
                               int type);
        uint64 (*read_register_64_bit)(conf_object_t *NOTNULL obj,
                                       uint32 opcode_1,
                                       uint32 CRm,
                                       int type);
        void (*write_register_64_bit)(conf_object_t *NOTNULL obj,
                                      uint64 value,
                                      uint32 opcode_1,
                                      uint32 CRm,
                                      int type);
        uint32 (*store_coprocessor)(conf_object_t *NOTNULL obj,
                               uint32 CRd,
                               uint32 N,
                               uint32 Options,
                               int type);
        void (*reset)(conf_object_t *NOTNULL obj, int hard_reset);
};

#define ARM_COPROCESSOR_INTERFACE "arm_coprocessor"
// ADD INTERFACE arm_coprocessor_interface

/* <add id="arm_avic_interface_t">

    The <iface>ARM AVIC</iface> interface makes it possible for an ARM
    processor to get the interrupt vector address from an AVIC device
    connected to the processor core. Both processor and AVIC must
    enable the AVIC interface to support this feature.

    The processor calls <fun>get_interrupt_address</fun> function to
    get the interrupt vector address. The AVIC returns an
    <em>arm_avic_t</em> struct with a <i>valid</i> field and an
    <i>address</i> field, the <i>address</i> field is only valid when
    the <i>valid</i> is not <tt>0</tt>.

   <insert-until text="// ADD INTERFACE arm_avic_interface"/>
   </add>
   <add id="arm_avic_interface_exec_context">
   Cell Context for all methods.
   </add> */
typedef struct arm_avic {
        int valid;
        uint32 address;
} arm_avic_t;

SIM_INTERFACE(arm_avic) {
        arm_avic_t (*get_interrupt_address)(conf_object_t *obj);
};

#define ARM_AVIC_INTERFACE "arm_avic"
// ADD INTERFACE arm_avic_interface


/* <add id="arm_trustzone_interface_t">

   This interface is implemented by ARM processors that supports the arm ARM
   TrustZone feature. The <fun>get_security_mode</fun> function returns the
   current state of the processor, whereas <fun>mem_op_security_mode</fun>
   extracts the mode of a memory operation in progress.

   The <fun>get_security_mode</fun> functions corresponds to the expression
   <tt>(cpsr.mode != Monitor &amp;&amp; scr.ns) ? Arm_Trustzone_Non_Secure :
   Arm_Trustzone_Secure</tt>. The <fun>mem_op_security_mode</fun> function
   always returns <tt>Arm_Trustzone_Non_Secure</tt> when the processor is in
   non-secure mode, in secure mode it returns the <tt>ns</tt> bit in the
   first-level page table entry for the actual area being accessed.

   <insert-until text="// END INTERFACE trustzone_arm"/>
   </add>
   <add id="arm_trustzone_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
typedef enum {
        Arm_Trustzone_Secure = 0,
        Arm_Trustzone_Non_Secure = 1
} arm_trustzone_mode_t;

SIM_INTERFACE(arm_trustzone) {
        arm_trustzone_mode_t (*get_security_mode)(conf_object_t *NOTNULL obj);
        arm_trustzone_mode_t (*mem_op_security_mode)(
                conf_object_t *NOTNULL obj,
                generic_transaction_t *NOTNULL memop);
};

#define ARM_TRUSTZONE_INTERFACE "arm_trustzone"
// END INTERFACE trustzone_arm

// ADD INTERFACE arm_external_debug_interface
#define ARM_EXTERNAL_DEBUG_INTERFACE "arm_external_debug"

/* <add id="arm_external_debug_interface_t">
   This <iface>arm_external_debug</iface> interface is used for
   external debug feature.

   The <fun>handle_semihosting</fun> function is called whenever the aarch64
   instruction hlt 0xf000 is executed where semihosting is enabled for use.

   The <fun>read_reg</fun> function is called reading the registers in external
   debug device.

   The <fun>write_reg</fun> function is called writing the registers in
   external debug device.
   </add>

   <add id="arm_external_debug_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(arm_external_debug) {
        void (*handle_semihosting)(conf_object_t *obj);
        uint64 (*read_reg)(conf_object_t *obj, int offset);
        void (*write_reg)(conf_object_t *obj, int offset, uint64 v);
};
// END INTERFACE arm_external_debug

// ADD INTERFACE arm_gic_interface
#define ARM_GIC_INTERFACE "arm_gic"

/* <add id="arm_gic_interface_t"> This <iface>arm_gic</iface> interface is used
   accessing registers in a device implementing the Generic Interrupt
   Controller architecture..

   The <fun>read_register</fun> function is called reading the registers in GIC
   device.

   The <fun>write_register</fun> function is called writing the registers in
   GIC device.

   The <fun>cpu_state_changed</fun> function is called to notify the GIC device
   that the cpu has changed state. This function is only called when the cpu
   changes state with an interrupt pending. </add>

   <add id="arm_gic_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef struct arm_cpu_state {
        bool fmo_set; /* scr_el3.irq */
        bool imo_set;
        bool irq_set; /* scr_el3.irq */
        bool fiq_set; /* scr_el3.fiq */
        bool ns;
        int el;
        uint32 mpidr;
} arm_cpu_state_t;

typedef struct gic_reg_info {
        int op1;
        int crn;
        int crm;
        int op2;
        arm_cpu_state_t cpu_state;
} gic_reg_info_t;

SIM_INTERFACE(arm_gic) {
        uint64 (*read_register)(conf_object_t *obj, gic_reg_info_t reg_info,
                                bool inquiry);
        void (*write_register)(conf_object_t *obj, gic_reg_info_t reg_info,
                               uint64 value);
        void (*cpu_state_changed)(conf_object_t *obj, arm_cpu_state_t cpu_state);
};
// END INTERFACE arm_gic

// ADD INTERFACE arm_gic_cpu_state
#define ARM_GIC_CPU_STATE_INTERFACE "arm_gic_cpu_state"

/* <add id="arm_gic_cpu_state_interface_t"> This <iface>arm_gic_cpu_state</iface>
   interface is used providing cpu states for a device implementing the Generic
   Interrupt Controller architecture.

   The <fun>get_cpu_state_info</fun> function is called to get current state of
   the a CPU, e.g. exception level, secure mode. </add>

   <add id="arm_gic_cpu_state_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(arm_gic_cpu_state) {
        arm_cpu_state_t (*get_cpu_state)(conf_object_t *obj);
};
// END INTERFACE arm_gic_cpu_state

/* <add id="devs api types">
     <name index="true">arm_mem_type_t</name>
     <insert id="arm_mem_type_t DOC"/>
   </add> */

/* <add id="arm_mem_type_t DOC">
     <ndx>arm_mem_type_t</ndx>
     <doc>
       <doc-item name="NAME">arm_mem_type_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_mem_type_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         Arm memory types. Corresponds to the MemType pseudo code
         enumeration in the Armv8 A-profile Architecture Reference
         Manual.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_mem_type_t def"></add-type> */
typedef enum {
        Arm_MemType_Normal,
        Arm_MemType_Device
} arm_mem_type_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_device_type_t</name>
     <insert id="arm_device_type_t DOC"/>
   </add> */

/* <add id="arm_device_type_t DOC">
     <ndx>arm_device_type_t</ndx>
     <doc>
       <doc-item name="NAME">arm_device_type_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_device_type_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         Arm device memory types. Corresponds to the DeviceType pseudo code
         enumeration in the Armv8 A-profile Architecture Reference Manual.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_device_type_t def"></add-type> */
typedef enum {
        Arm_DeviceType_nGnRnE = 0x0,
        Arm_DeviceType_nGnRE  = 0x1,
        Arm_DeviceType_nGRE   = 0x2,
        Arm_DeviceType_GRE    = 0x3,
        Arm_DeviceType_Unknown
} arm_device_type_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_mem_attr_t</name>
     <insert id="arm_mem_attr_t DOC"/>
   </add> */

/* <add id="arm_mem_attr_t DOC">
     <ndx>arm_mem_attr_t</ndx>
     <doc>
       <doc-item name="NAME">arm_mem_attr_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_mem_attr_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         Memory cacheability. Corresponds to the MemAttr pseudo code constants
         in the Armv8 A-profile Architecture Reference Manual.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_mem_attr_t def"></add-type> */
typedef enum {
        Arm_MemAttr_NC = 0x0, // Non-cacheable
        Arm_MemAttr_WT = 0x2, // Write-through
        Arm_MemAttr_WB = 0x3, // Write-back
        Arm_MemAttr_Unknown
} arm_mem_attr_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_mem_hint_t</name>
     <insert id="arm_mem_hint_t DOC"/>
   </add> */

/* <add id="arm_mem_hint_t DOC">
     <ndx>arm_mem_hint_t</ndx>
     <doc>
       <doc-item name="NAME">arm_mem_hint_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_mem_hint_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         Cache allocation hint. Corresponds to the MemHint pseudo code
         constants in the Armv8 A-profile Architecture Reference Manual.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_mem_hint_t def"></add-type> */
typedef enum {
        Arm_MemHint_No  = 0x0, // No Read-Allocate, No Write-Allocate
        Arm_MemHint_WA  = 0x1, // No Read-Allocate, Write-Allocate
        Arm_MemHint_RA  = 0x2, // Read-Allocate, No Write-Allocate
        Arm_MemHint_RWA = 0x3, // Read-Allocate, Write-Allocate
        Arm_MemHint_Unknown
} arm_mem_hint_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_mem_transient_t</name>
     <insert id="arm_mem_transient_t DOC"/>
   </add> */

/* <add id="arm_mem_transient_t DOC">
     <ndx>arm_mem_transient_t</ndx>
     <doc>
       <doc-item name="NAME">arm_mem_transient_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_mem_transient_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         Transcience hint. Corresponds to the boolean used for transience by
         the pseudo code in the Armv8 A-profile Architecture Reference Manual.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_mem_transient_t def"></add-type> */
typedef enum {
        Arm_Transient_True,
        Arm_Transient_False,
        Arm_Transient_Unknown
} arm_mem_transient_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_memory_attributes_encoding_t</name>
     <insert id="arm_memory_attributes_encoding_t DOC"/>
   </add> */

/* <add id="arm_memory_attributes_encoding_t DOC">
     <ndx>arm_memory_attributes_encoding_t</ndx>
     <doc>
       <doc-item name="NAME">arm_memory_attributes_encoding_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_memory_attributes_encoding_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         This type should be used to encode or decode the uint64 value
         contained in an arm_memory_attributes atom. The comment beside each
         field is the type that should be used to interpret the field value.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_memory_attributes_encoding_t def"></add-type> */
typedef union {
        struct {
                uint64 memory_type:2;            // arm_mem_type_t
                uint64 device_type:3;            // arm_device_type_t
                uint64 inner_cacheability:3;     // arm_mem_attr_t
                uint64 inner_allocation_hint:3;  // arm_mem_hint_t
                uint64 inner_transcience_hint:2; // arm_mem_transient_t
                uint64 outer_cacheability:3;     // arm_mem_attr_t
                uint64 outer_allocation_hint:3;  // arm_mem_hint_t
                uint64 outer_transcience_hint:2; // arm_mem_transient_t
                uint64 shareable:1;              // bool
                uint64 outer_shareable:1;        // bool
        } u;
        uint64 u64;
} arm_memory_attributes_encoding_t;
/* </add-type> */

/* <add id="devs api types">
     <name index="true">arm_smmu_attributes_t</name>
     <insert id="arm_smmu_attributes_t DOC"/>
   </add> */

/* <add id="arm_smmu_attributes_t DOC">
     <ndx>arm_smmu_attributes_t</ndx>
     <doc>
       <doc-item name="NAME">arm_smmu_attributes_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
           <insert id="arm_smmu_attributes_t def"/>
         </smaller>
       </doc-item>
       <doc-item name="DESCRIPTION">
         This type should be used to encode or decode the uint64 value
         contained in an arm_smmu_attributes atom. The comment beside each
         field is the type that should be used to interpret the field value.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_smmu_attributes_t def"></add-type> */
typedef union {
        struct {
                uint64 sid:32;    // IMPLEMENTATION DEFINED size, between 0 and 32 bits
                uint64 ssid:20;   // IMPLEMENTATION DEFINED size, between 0 and 20 bits
                uint64 secsid:1;  // bool
                uint64 ssidv:1;   // bool
                uint64 atst:1;    // bool
        } u;
       uint64 u64;
} arm_smmu_attributes_t;
/* </add-type> */

/* Determines whether a memory access is privileged. */
#define ATOM_TYPE_arm_privileged bool
SIM_ACCESSIBLE_ATOM(arm_privileged);

/* Determines whether a memory access is non-secure. */
#define ATOM_TYPE_arm_nonsecure bool
SIM_ACCESSIBLE_ATOM(arm_nonsecure);

/* Memory attributes of a memory access. */
#define ATOM_TYPE_arm_memory_attributes uint64
SIM_ACCESSIBLE_ATOM(arm_memory_attributes);

/* MMU attributes of a memory access. */
#define ATOM_TYPE_arm_smmu_attributes uint64
SIM_ACCESSIBLE_ATOM(arm_smmu_attributes);

/* Non-Secure Access IDentity of a memory access. */
#define ATOM_TYPE_arm_nsaid uint64
SIM_ACCESSIBLE_ATOM(arm_nsaid);

/* DeviceID of a memory access. */
#define ATOM_TYPE_arm_device_id uint64
SIM_ACCESSIBLE_ATOM(arm_device_id);

/* <add id="arm_cpu_group_exclusive_interface_t">

   This interface is only intended to be used between Arm CPU objects. It
   contains the functions needed for cross-CPU communication related to
   exclusive memory accesses.

   <fun>mark_exclusive</fun> notifies the CPU that another CPU has marked the
   address range as exclusive. The CPU must then probe all CPUs in the CPU
   group for exclusive address ranges using <fun>probe_exclusive</fun> and
   possibly invalidate them using <fun>clear_exclusive</fun> before accessing
   the address range.

   <fun>clear_and_probe_exclusive</fun> notifies the CPU to invalidate any
   exclusive address ranges that it has overlapping the specified clear address
   range. Also returns true if the CPU still has any exclusive address ranges
   overlapping the specified probe address range.

   <insert-until text="// END INTERFACE arm_cpu_group_exclusive"/>
   </add>

   <add id="arm_cpu_group_exclusive_interface_exec_context">
   Threaded Context for all methods.
   </add>
*/

SIM_INTERFACE(arm_cpu_group_exclusive) {
        void (*mark_exclusive)(
                conf_object_t *obj,
                physical_address_t address,
                physical_address_t size);
        bool (*clear_and_probe_exclusive)(
                conf_object_t *obj,
                physical_address_t clear_address,
                physical_address_t clear_size,
                physical_address_t probe_address,
                physical_address_t probe_size);
};

#define ARM_CPU_GROUP_EXCLUSIVE_INTERFACE "arm_cpu_group_exclusive"

// END INTERFACE arm_cpu_group_exclusive

/* <add id="arm_cpu_group_event_interface_t">

   This interface is only intended to be used between Arm CPU objects. It
   contains the functions needed for cross-CPU communication related to
   event signalling.

   <fun>signal_event</fun> notifies the CPU to set the event register.

   <insert-until text="// END INTERFACE arm_cpu_group_event"/>
   </add>

   <add id="arm_cpu_group_event_interface_exec_context">
   Threaded Context for all methods.
   </add>
*/

SIM_INTERFACE(arm_cpu_group_event) {
        void (*signal_event)(conf_object_t *obj);
};

#define ARM_CPU_GROUP_EVENT_INTERFACE "arm_cpu_group_event"

// END INTERFACE arm_cpu_group_event

/* <add id="devs api types">
     <name index="true">arm_translation_regime_t</name>
     <insert id="arm_translation_regime_t DOC"/>
   </add> */

/* <add id="arm_translation_regime_t DOC">
     <ndx>arm_translation_regime_t</ndx>
     <doc>
       <doc-item name="NAME">arm_translation_regime_t</doc-item>
       <doc-item name="SYNOPSIS">
         <smaller>
         <insert id="arm_translation_regime_t def"/>
         </smaller>,
       </doc-item>
       <doc-item name="DESCRIPTION">
         Arm MMU translation regimes. Named after the AArch64 translation
         regimes, but also used for the AArch32 ones.
       </doc-item>
     </doc>
   </add> */

/* <add-type id="arm_translation_regime_t def"></add-type> */
typedef enum {
        Arm_TR_EL3,  /* EL3         */
        Arm_TR_EL2,  /* EL2   PL2   */
        Arm_TR_EL20, /* EL2&0       */
        Arm_TR_EL10, /* EL1&0 PL1&0 */
} arm_translation_regime_t;
/* </add-type> */

/* <add id="arm_cpu_group_tlb_interface_t">

   This interface is only intended to be used between Arm CPU objects. It
   contains the functions needed for cross-CPU communication related to
   TLB invalidation.

   <fun>invalidate_tlb</fun> notifies the CPU to invalidate TLB entries related
   to the translation regime. If <arg>by_virtual_address</arg> is true only
   entries containing the specified virtual address should be invalidated,
   otherwise all entries should be invalidated.

   <insert-until text="// END INTERFACE arm_cpu_group_tlb"/>
   </add>

   <add id="arm_cpu_group_tlb_interface_exec_context">
   Threaded Context for all methods.
   </add>
*/

SIM_INTERFACE(arm_cpu_group_tlb) {
        void (*invalidate_tlb)(
                conf_object_t *obj,
                arm_translation_regime_t translation_regime,
                bool by_virtual_address,
                logical_address_t virtual_address);
};

#define ARM_CPU_GROUP_TLB_INTERFACE "arm_cpu_group_tlb"

// END INTERFACE arm_cpu_group_tlb

#endif /* TURBO_SIMICS */

#if defined(__cplusplus)
}
#endif

#endif
