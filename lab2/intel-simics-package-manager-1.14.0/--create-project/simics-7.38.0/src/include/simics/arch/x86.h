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

#ifndef SIMICS_ARCH_X86_H
#define SIMICS_ARCH_X86_H

#include <simics/base/memory-transaction.h>
#include <simics/processor/types.h>
#include <simics/processor/generic-spr.h>
#include <simics/pywrap.h>
#include <simics/arch/x86-memop.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="x86_access_type_interface_t">

   This interface is used to query information about the
   <type>x86_access_type_t</type> enum. The <fun>get_enum_name</fun> method
   returns the C enum name for the access type. <fun>get_short_name</fun>
   returns a corresponding short name. More than one access type may have the
   same short name.  The <fun>get_description</fun> returns a longer
   description of the access type.  The <fun>implicit</fun> function returns
   true if the access type is considered implicit or false if it is considered
   explicit. This information is used by the
   <iface>cpu_instrumentation_subscribe</iface> interface to separate accesses
   in the two different categories.

   <insert-until text="// ADD INTERFACE x86_access_type"/>

   </add>
   <add id="x86_access_type_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_access_type) {
        const char *(*get_enum_name)(conf_object_t *obj, x86_access_type_t at);
        const char *(*get_short_name)(conf_object_t *obj, x86_access_type_t at);
        const char *(*get_description)(conf_object_t *obj, x86_access_type_t at);
        bool (*implicit)(conf_object_t *obj, x86_access_type_t at);
};

#define X86_ACCESS_TYPE_INTERFACE "x86_access_type"
// ADD INTERFACE x86_access_type

/*
   <add id="devs api types">
   <name index="true">x86_sync_instruction_type_t</name>
   <insert id="x86_sync_instruction_type_t DOC"/>
   </add> */

/* <add id="x86_sync_instruction_type_t DOC">
   <ndx>x86_sync_instruction_type_t</ndx>
   <doc>
   <doc-item name="NAME">x86_sync_instruction_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="x86_sync_instruction_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
     Type of synchronisation instruction for x86.
     Used in the <tt>Core_Sync_Instruction</tt> hap.
   </doc-item>
   </doc>
   </add>
*/

/* <add-type id="x86_sync_instruction_type_t def"></add-type> */
typedef enum {
        X86_SFence = 1,
        X86_LFence = 2,
        X86_MFence = 3
} x86_sync_instruction_type_t;


/*
   <add id="arch_register_id_t def">
   <insert id="x86_register_id type"/>
   </add>

   <add id="arch_register_id_t desc">
   For x86 the <type>x86_register_id_t</type> can be used to refer to
   the different registers.
   </add>
*/

/* NOTE: do not change the numbers of RAX-RDI */

/* <add-type id="x86_register_id type"><ndx>x86_register_id_t</ndx>
   </add-type> */
typedef enum {
        X86_Reg_Id_Rax = 0,
        X86_Reg_Id_Rcx = 1,
        X86_Reg_Id_Rdx = 2,
        X86_Reg_Id_Rbx = 3,
        X86_Reg_Id_Rsp = 4,
        X86_Reg_Id_Rbp = 5,
        X86_Reg_Id_Rsi = 6,
        X86_Reg_Id_Rdi = 7,

        X86_Reg_Id_R8  = 8,
        X86_Reg_Id_R9  = 9,
        X86_Reg_Id_R10 = 10,
        X86_Reg_Id_R11 = 11,
        X86_Reg_Id_R12 = 12,
        X86_Reg_Id_R13 = 13,
        X86_Reg_Id_R14 = 14,
        X86_Reg_Id_R15 = 15,

        X86_Reg_Id_Xmm0  = 16,
        X86_Reg_Id_Xmm1  = 17,
        X86_Reg_Id_Xmm2  = 18,
        X86_Reg_Id_Xmm3  = 19,
        X86_Reg_Id_Xmm4  = 20,
        X86_Reg_Id_Xmm5  = 21,
        X86_Reg_Id_Xmm6  = 22,
        X86_Reg_Id_Xmm7  = 23,

        X86_Reg_Id_Xmm8  = 24,
        X86_Reg_Id_Xmm9  = 25,
        X86_Reg_Id_Xmm10 = 26,
        X86_Reg_Id_Xmm11 = 27,
        X86_Reg_Id_Xmm12 = 28,
        X86_Reg_Id_Xmm13 = 29,
        X86_Reg_Id_Xmm14 = 30,
        X86_Reg_Id_Xmm15 = 31,

        X86_Reg_Id_Mm0 = 32,
        X86_Reg_Id_Mm1 = 33,
        X86_Reg_Id_Mm2 = 34,
        X86_Reg_Id_Mm3 = 35,
        X86_Reg_Id_Mm4 = 36,
        X86_Reg_Id_Mm5 = 37,
        X86_Reg_Id_Mm6 = 38,
        X86_Reg_Id_Mm7 = 39,

        X86_Reg_Id_PC  = 40,    /* this is RIP */

        X86_Reg_Id_CF  = 41,    /* integer condition code flags */
        X86_Reg_Id_DST = 42,    /* DST field used to cache PF flag */
        X86_Reg_Id_AF  = 43,
        X86_Reg_Id_ZF  = 44,
        X86_Reg_Id_SF  = 45,
        X86_Reg_Id_OF  = 46,
        X86_Reg_Id_DF  = 47,

        X86_Reg_Id_EFLAGS = 48, /* the whole 32 bits eflags */

        X86_Reg_Id_C0 = 49,     /* floating point cc flags */
        X86_Reg_Id_C1 = 50,
        X86_Reg_Id_C2 = 51,
        X86_Reg_Id_C3 = 52,
        X86_Reg_Id_Top = 53,    /* floating point stack top */
        X86_Reg_Id_Not_Used,    /* dummy number that can be used */

        X86_Reg_Id_Local_Max    /* keep this one last */
} x86_register_id_t;

/* <add-type id="x86_memory_or_io_t"> </add-type> */
typedef enum {
        X86_Memory,
        X86_IO
} x86_memory_or_io_t;

#define X86_REG_ID_RAX_BIT (1ULL << X86_Reg_Id_Rax)
#define X86_REG_ID_RCX_BIT (1ULL << X86_Reg_Id_Rcx)
#define X86_REG_ID_RDX_BIT (1ULL << X86_Reg_Id_Rdx)
#define X86_REG_ID_RBX_BIT (1ULL << X86_Reg_Id_Rbx)
#define X86_REG_ID_RSP_BIT (1ULL << X86_Reg_Id_Rsp)
#define X86_REG_ID_RBP_BIT (1ULL << X86_Reg_Id_Rbp)
#define X86_REG_ID_RSI_BIT (1ULL << X86_Reg_Id_Rsi)
#define X86_REG_ID_RDI_BIT (1ULL << X86_Reg_Id_Rdi)

#define X86_REG_ID_XMM0_BIT (1ULL << X86_Reg_Id_Xmm0)
#define X86_REG_ID_XMM1_BIT (1ULL << X86_Reg_Id_Xmm1)
#define X86_REG_ID_XMM2_BIT (1ULL << X86_Reg_Id_Xmm2)
#define X86_REG_ID_XMM3_BIT (1ULL << X86_Reg_Id_Xmm3)
#define X86_REG_ID_XMM4_BIT (1ULL << X86_Reg_Id_Xmm4)
#define X86_REG_ID_XMM5_BIT (1ULL << X86_Reg_Id_Xmm5)
#define X86_REG_ID_XMM6_BIT (1ULL << X86_Reg_Id_Xmm6)
#define X86_REG_ID_XMM7_BIT (1ULL << X86_Reg_Id_Xmm7)

#define X86_REG_ID_R8_BIT  (1ULL << X86_Reg_Id_R8)
#define X86_REG_ID_R9_BIT  (1ULL << X86_Reg_Id_R9)
#define X86_REG_ID_R10_BIT (1ULL << X86_Reg_Id_R10)
#define X86_REG_ID_R11_BIT (1ULL << X86_Reg_Id_R11)
#define X86_REG_ID_R12_BIT (1ULL << X86_Reg_Id_R12)
#define X86_REG_ID_R13_BIT (1ULL << X86_Reg_Id_R13)
#define X86_REG_ID_R14_BIT (1ULL << X86_Reg_Id_R14)
#define X86_REG_ID_R15_BIT (1ULL << X86_Reg_Id_R15)

#define X86_REG_ID_XMM8_BIT  (1ULL << X86_Reg_Id_Xmm8)
#define X86_REG_ID_XMM9_BIT  (1ULL << X86_Reg_Id_Xmm9)
#define X86_REG_ID_XMM10_BIT (1ULL << X86_Reg_Id_Xmm10)
#define X86_REG_ID_XMM11_BIT (1ULL << X86_Reg_Id_Xmm11)
#define X86_REG_ID_XMM12_BIT (1ULL << X86_Reg_Id_Xmm12)
#define X86_REG_ID_XMM13_BIT (1ULL << X86_Reg_Id_Xmm13)
#define X86_REG_ID_XMM14_BIT (1ULL << X86_Reg_Id_Xmm14)
#define X86_REG_ID_XMM15_BIT (1ULL << X86_Reg_Id_Xmm15)

#define X86_REG_ID_MM0_BIT (1ULL << X86_Reg_Id_Mm0)
#define X86_REG_ID_MM1_BIT (1ULL << X86_Reg_Id_Mm1)
#define X86_REG_ID_MM2_BIT (1ULL << X86_Reg_Id_Mm2)
#define X86_REG_ID_MM3_BIT (1ULL << X86_Reg_Id_Mm3)
#define X86_REG_ID_MM4_BIT (1ULL << X86_Reg_Id_Mm4)
#define X86_REG_ID_MM5_BIT (1ULL << X86_Reg_Id_Mm5)
#define X86_REG_ID_MM6_BIT (1ULL << X86_Reg_Id_Mm6)
#define X86_REG_ID_MM7_BIT (1ULL << X86_Reg_Id_Mm7)

#define X86_REG_ID_PC_BIT (1ULL << X86_Reg_Id_PC)

#define X86_REG_ID_CF_BIT (1ULL << X86_Reg_Id_CF)
#define X86_REG_ID_DST_BIT (1ULL << X86_Reg_Id_DST)
#define X86_REG_ID_AF_BIT (1ULL << X86_Reg_Id_AF)
#define X86_REG_ID_ZF_BIT (1ULL << X86_Reg_Id_ZF)
#define X86_REG_ID_SF_BIT (1ULL << X86_Reg_Id_SF)
#define X86_REG_ID_OF_BIT (1ULL << X86_Reg_Id_OF)
#define X86_REG_ID_DF_BIT (1ULL << X86_Reg_Id_DF)

#define X86_REG_ID_EFLAGS_BIT (1ULL << X86_Reg_Id_EFLAGS)

#define X86_REG_ID_C0_BIT  (1ULL << X86_Reg_Id_C0)
#define X86_REG_ID_C1_BIT  (1ULL << X86_Reg_Id_C1)
#define X86_REG_ID_C2_BIT  (1ULL << X86_Reg_Id_C2)
#define X86_REG_ID_C3_BIT  (1ULL << X86_Reg_Id_C3)
#define X86_REG_ID_TOP_BIT (1ULL << X86_Reg_Id_Top)

#define SIM_DI_PREFIX_F0_BIT           (1 << 0)
#define SIM_DI_PREFIX_F2_BIT           (1 << 1)
#define SIM_DI_PREFIX_F3_BIT           (1 << 2)
#define SIM_DI_PREFIX_CS_BIT           (1 << 3)
#define SIM_DI_PREFIX_SS_BIT           (1 << 4)
#define SIM_DI_PREFIX_DS_BIT           (1 << 5)
#define SIM_DI_PREFIX_ES_BIT           (1 << 6)
#define SIM_DI_PREFIX_FS_BIT           (1 << 7)
#define SIM_DI_PREFIX_GS_BIT           (1 << 8)
#define SIM_DI_PREFIX_OPERAND_SIZE_BIT (1 << 9)
#define SIM_DI_PREFIX_66_BIT           SIM_DI_PREFIX_OPERAND_SIZE_BIT
#define SIM_DI_PREFIX_ADDRESS_SIZE_BIT (1 << 10)
#define SIM_DI_PREFIX_SSE_BIT          (1 << 11)
#define SIM_DI_PREFIX_REX_BIT          (1 << 12)

#define SIM_DI_PREFIX_REX_POS          16
#define SIM_DI_PREFIX_REX_POS_B        16
#define SIM_DI_PREFIX_REX_POS_X        17
#define SIM_DI_PREFIX_REX_POS_R        18
#define SIM_DI_PREFIX_REX_POS_W        19

EXPORTED x86_memory_transaction_t *SIM_x86_mem_trans_from_generic(
        generic_transaction_t *NOTNULL mop);

/* <add id="x86_tlb_interface_t">
   The x86 tlb interface is used for communication between an x86 cpu
   and its TLB. The TLB is implemented as a separate class for greater
   flexibility. The TLB object does no memory operations itself.

   <ndx>tagged_physical_address_t</ndx>
   <insert-until text="// ADD INTERFACE x86_tlb_interface"/>

   All functions in the interface get the <i>object</i> implementing
   the interface as their first parameter.

   <b>flush_all</b> is called when all TLB entries should be
   flushed. If <i>keep_global_entries</i> is set, then TLB entries
   with their global bit set should not be flushed.

   <b>flush_page</b> is invoked when pages containing <i>laddr</i> are
   to be removed from the TLB.

   <b>lookup</b> is used by the CPU when a memory access misses the
   STC. It must return true (non-zero) if and only if the memory
   operation specified by <i>mem_tr</i> hits the TLB and does not
   raise an exception. The <i>mode</i>, <i>linear_address</i> are valid when
   the method is invoked. The other fields passed through <i>mem_tr</i>
   are undefined. If the method returns true, the
   <i>s.physical_address</i>, <i>pat_type</i>, and <i>mtrr_type</i>
   fields of <i>mem_tr</i> must be updated by <b>lookup</b>.

   An access that misses in <b>lookup</b> but does not raise a fault
   is inserted into the TLB with <b>add</b>. The <i>page_size</i>
   encoding is 0 for 4 kb pages, 1 for 2 Mb pages, and 2 for 4 Mb
   pages.

   <b>itlb_lookup</b> is a simplified version of <b>lookup</b> used
   only for instruction TLB lookups. If the lookup is successful
   <i>valid</i> and <i>paddr</i> should be set, otherwise <i>valid</i>
   should be cleared.

   <b>set_pae_mode</b> is invoked when the cpu changes the
   PAE enable bit.

   It class implementing the interface must make sure that only
   addresses mapped in the TLB are present in the STCs.

   This interface may be changed or replaced with an architecture
   independent interface in future versions of Simics.

   </add>
   <add id="x86_tlb_interface_exec_context">
   Cell Context for all methods.
   </add> */
typedef struct {
        int                  valid;
        physical_address_t   paddr;
} tagged_physical_address_t;

SIM_INTERFACE(x86_tlb) {
        void (*flush_all)(conf_object_t *obj,
                            int keep_global_entries);
        void (*flush_page)(conf_object_t *obj,
                           linear_address_t laddr);
        int (*lookup)(conf_object_t *obj,
                      x86_memory_transaction_t *mem_tr);
        void (*add)(conf_object_t *obj,
                    processor_mode_t mode,
                    read_or_write_t read_or_write,
                    data_or_instr_t data_or_instr,
                    int global_page,
                    x86_memory_type_t pat_type,
                    x86_memory_type_t mtrr_type,
                    linear_address_t laddr,
                    physical_address_t paddr,
                    int page_size);
        tagged_physical_address_t (*itlb_lookup)(conf_object_t *obj,
                                                 linear_address_t laddr,
                                                 processor_mode_t mode);
        void (*set_pae_mode)(conf_object_t *obj, bool pae_mode);
};

#define X86_TLB_INTERFACE "x86_tlb"
// ADD INTERFACE x86_tlb_interface

typedef struct {
        access_t supervisor_access;
        access_t user_access;
        bool global_page;
        x86_memory_type_t pat_type;
        x86_memory_type_t mtrr_type;
        unsigned page_size_k;
} x86_tlb_attrs_t;

typedef struct {
        linear_address_t        linear_page_start;
        physical_address_t      physical_page_start;
        x86_tlb_attrs_t         attrs;
} x86_tlb_entry_t;

/* <add id="x86_tlb_v2_interface_t">
   The x86_tlb_v2 interface is used for communication between an x86 cpu
   and its TLB. The TLB is implemented as a separate class for greater
   flexibility. The TLB object does no memory operations itself.

   <fun>flush_all</fun> is called when all TLB entries should be flushed. If
   <arg>keep_global_entries</arg> is set, then TLB entries with their global
   bit set should not be flushed.

   <fun>flush_page</fun> is invoked when pages containing <arg>laddr</arg> are
   to be removed from the TLB.

   <fun>lookup</fun> is used by the CPU when a memory access misses the STC. It
   returns the matching TLB entry if the memory operation specified in
   <arg>mem_tr</arg> hits the TLB and does not raise an exception. Otherwise
   NULL is returned. The <arg>mode</arg>, <arg>linear_address</arg>, and
   <arg>type</arg> fields in <arg>mem_tr</arg> need to be valid when the method
   is invoked. The other fields passed through <arg>mem_tr</arg> are not to be
   used by the method and can carry any value. If the method returns true, the
   <arg>s.physical_address</arg>, <arg>pat_type</arg>, and <arg>mtrr_type</arg>
   fields of <arg>mem_tr</arg> must be updated by <fun>lookup</fun>.

   Pages are added to the TLB with <fun>add</fun>. The
   <arg>supervisor_access</arg> field in <arg>attrs</arg> argument specifies
   the allowed access types in supervisor mode and <arg>user_access</arg> in
   <arg>attrs</arg> specifies the allowed access types in user mode.

   <fun>itlb_lookup</fun> is a simplified version of <fun>lookup</fun> used
   only for instruction TLB lookups. If the lookup is successful
   <arg>valid</arg> and <arg>paddr</arg> should be set, otherwise
   <arg>valid</arg> should be cleared.

   The class implementing the interface must make sure that only addresses
   mapped in the TLB are present in the STCs.

   <insert-until text="// ADD INTERFACE x86_tlb_v2_interface"/>
   </add>
   <add id="x86_tlb_v2_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_tlb_v2) {
        void (*flush_all)(conf_object_t *obj,
                          int keep_global_entries);
        void (*flush_page)(conf_object_t *obj,
                           linear_address_t laddr);
        const x86_tlb_entry_t *(*lookup)(conf_object_t *obj,
                                         x86_memory_transaction_t *mem_tr);
        void (*add)(conf_object_t *obj,
                    linear_address_t laddr,
                    physical_address_t paddr,
                    x86_tlb_attrs_t attrs);
        tagged_physical_address_t (*itlb_lookup)(conf_object_t *obj,
                                                 linear_address_t laddr,
                                                 processor_mode_t mode);
};

#define X86_TLB_V2_INTERFACE "x86_tlb_v2"
// ADD INTERFACE x86_tlb_v2_interface

#define X86_TLB_PTE_WRITE           (1ull << 1)
#define X86_TLB_PTE_USER            (1ull << 2)
#define X86_TLB_PTE_DIRTY           (1ull << 6)
#define X86_TLB_PTE_GLOBAL          (1ull << 8)
#define X86_TLB_PTE_PAGE_MASK       (X86_TLB_PTE_WRITE | X86_TLB_PTE_USER |\
                                     X86_TLB_PTE_DIRTY | X86_TLB_PTE_GLOBAL)
#define X86_TLB_PTE_USER_SHIFT      9
#define X86_TLB_PTE_USER_READ       (1ull << X86_TLB_PTE_USER_SHIFT)
#define X86_TLB_PTE_USER_WRITE      (1ull << (X86_TLB_PTE_USER_SHIFT + 1))
#define X86_TLB_PTE_USER_EXECUTE    (1ull << (X86_TLB_PTE_USER_SHIFT + 2))
#define X86_TLB_PTE_USER_MASK       (X86_TLB_PTE_USER_READ |\
                                     X86_TLB_PTE_USER_WRITE |\
                                     X86_TLB_PTE_USER_EXECUTE)
#define X86_TLB_PTE_SVISOR_SHIFT    60
#define X86_TLB_PTE_SVISOR_READ     (1ull << X86_TLB_PTE_SVISOR_SHIFT)
#define X86_TLB_PTE_SVISOR_WRITE    (1ull << (X86_TLB_PTE_SVISOR_SHIFT + 1))
#define X86_TLB_PTE_SVISOR_EXECUTE  (1ull << (X86_TLB_PTE_SVISOR_SHIFT + 2))
#define X86_TLB_PTE_SVISOR_MASK     (X86_TLB_PTE_SVISOR_READ |\
                                     X86_TLB_PTE_SVISOR_WRITE |\
                                     X86_TLB_PTE_SVISOR_EXECUTE)
#define X86_TLB_PTE_MASK            (X86_TLB_PTE_USER_MASK |\
                                     X86_TLB_PTE_SVISOR_MASK)

/* <add-type id="x86_tlb_attrs_v3_t def"> </add-type> */
typedef struct {
        uint64 pte_attrs;
        x86_memory_type_t pat_type;
        x86_memory_type_t mtrr_type;
        unsigned page_size_k;
} x86_tlb_attrs_v3_t;

/* <add-type id="x86_tlb_entry_v3_t def"> </add-type> */
typedef struct {
        linear_address_t        linear_page_start;
        physical_address_t      physical_page_start;
        x86_tlb_attrs_v3_t      attrs;
} x86_tlb_entry_v3_t;

/* <add-type id="x86_tlb_inv_t def"> </add-type> */
typedef enum {
        X86_Tlb_Invalidate_Page = 0,
        X86_Tlb_Invalidate_Address_Space = 4,
        X86_Tlb_Invalidate_Address_Space_NonGlobal = 1,
        X86_Tlb_Invalidate_All = 2,
        X86_Tlb_Invalidate_All_NonGlobal = 3
} x86_tlb_inv_t;

/* <add id="x86_tlb_v3_interface_t">
   The x86_tlb_v3 interface is used for communication between an x86 cpu
   and its TLB. The TLB is implemented as a separate class for greater
   flexibility. The TLB object does no memory operations itself.

   Pages are added to the TLB with <fun>add</fun>. Process context identifier,
   linear and physical address for translation specified using arguments
   <arg>hint</arg>, <arg>laddr</arg> and <arg>paddr</arg> correspondingly.
   The fields in <arg>attrs</arg> argument used for specification of
   allowed access types and additional page attributes.

   <fun>lookup</fun> is used by the CPU when a memory access misses the STC.
   It returns the matching TLB entry if the memory operation specified in
   <arg>mem_tr</arg> hits the TLB and does not raise an exception. Otherwise
   NULL is returned. The <arg>hint</arg> argument specifies process context
   identifier. The <arg>mode</arg>, <arg>linear_address</arg>, and
   <arg>type</arg> fields in <arg>mem_tr</arg> need to be valid when the method
   is invoked. The other fields passed through <arg>mem_tr</arg> are not to be
   used by the method and can carry any value. If the method returns not NULL,
   the <arg>s.physical_address</arg>, <arg>pat_type</arg>, and
   <arg>mtrr_type</arg> fields of <arg>mem_tr</arg> must be updated by
   <fun>lookup</fun>.

   <fun>itlb_lookup</fun> is a simplified version of <fun>lookup</fun> used
   only for instruction TLB lookups. If the lookup is successful
   <arg>valid</arg> and <arg>paddr</arg> should be set, otherwise
   <arg>valid</arg> should be cleared.

   <fun>invalidate_page</fun> is used to remove from TLB page corresponding to
   given linear address specified by argument <arg>laddr</arg> in process
   context with identifier from argument <arg>hint</arg>.

   <fun>invalidate</fun> is invoked when value of CR4.PCIDE bit set and CPU
   executes INVPCID instruction. Direct and indirect operands of instruction
   transferred in <arg>hint</arg>, <arg>la</arg> and <arg>type</arg>
   arguments. TLB entries flushed according to INVPCID instruction description.
   New invalidation type X86_Tlb_Invalidate_Address_Space added to implement
   flush_all function from previous interfaces. The method also can be used to
   flush required TLB entries in any required cases.

   The class implementing the interface must make sure that only addresses
   mapped in the TLB are present in the STCs.

   <insert-until text="// ADD INTERFACE x86_tlb_v3_interface"/>
   </add>
   <add id="x86_tlb_v3_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_tlb_v3) {
        void (*add)(conf_object_t *obj,
                    uint64 hint,
                    linear_address_t laddr,
                    physical_address_t paddr,
                    x86_tlb_attrs_v3_t attrs);
        const x86_tlb_entry_v3_t *(*lookup)(conf_object_t *obj,
                                            uint64 hint,
                                            x86_memory_transaction_t *mem_tr);
        tagged_physical_address_t (*itlb_lookup)(conf_object_t *obj,
                                                 uint64 hint,
                                                 linear_address_t laddr,
                                                 processor_mode_t mode);
        void    (*invalidate_page)(conf_object_t *obj,
                                   uint64 hint,
                                   linear_address_t laddr);
        void    (*invalidate)(conf_object_t *obj,
                              uint32 type,
                              uint64 hint,
                              linear_address_t la);
};

#define X86_TLB_V3_INTERFACE "x86_tlb_v3"
// ADD INTERFACE x86_tlb_v3_interface

/* <add-type id="x86_pin_t"> </add-type> */
typedef enum {
        Pin_Init,
        Pin_Smi,
        Pin_Nmi,
        Pin_Ignne,
        Pin_Mcerr,
        Pin_Cmci
        /* Do not add new pins. Instead, define individual signal ports */
} x86_pin_t;

typedef struct {
        bool             valid;
        linear_address_t addr;
} tagged_linear_address_t;

/* <add id="x86_interface_t">

   The x86 interface exports functionality used by several other objects to
   interact with the processor. The <fun>set_pin_status</fun> function is used
   to set/clear certain processor pins.

   <ndx>x86_pin_t</ndx>
   <insert id="x86_pin_t"/>

   The <fun>start_up</fun> function is used by the local APIC to bring the
   processor <arg>obj</arg> out of the waiting for start-up IPI state, with a
   start address of <arg>start_address</arg>.

   The <fun>interrupt</fun> function tells the CPU that an interrupt is
   waiting. The interrupt vector number is given by the <arg>ack</arg> callback
   function. It is not allowed to call the <arg>ack</arg> callback during the
   <fun>interrupt</fun> call itself. The <arg>data</arg> object should
   implement the <iface>interrupt_cpu</iface> interface. Note that the
   <iface>interrupt_cpu</iface> interface defines the <arg>ack</arg> function,
   which should be the same as the <arg>ack</arg> argument. It is recommended
   that new implementations does not use rely on the <arg>ack</arg> argument,
   but rather looks up the callback through the <iface>interrupt_cpu</iface>
   interface from the object given by the <arg>data</arg> argument. The
   <fun>interrupt</fun> function returns 1 if the interrupt is accepted, and 0
   if it is not accepted because there is already an interrupt queued up to be
   processed. If 0 is returned, the caller should retry the call after one
   step. It should wait one step since the handling of an interrupt takes one
   step.

   If a checkpoint was taken when an interrupt was waiting, the acknowledge
   callback function can be recovered by looking up the
   <iface>interrupt_cpu</iface> interface at the APIC object given in the
   <arg>data</arg> parameter.

   If the interrupt was cancelled before it was acknowledged, the
   <fun>uninterrupt</fun> function is used. It is also called by the
   acknowledge callback. Thus, each invocation of <fun>interrupt</fun> is
   followed by exactly one call to <fun>uninterrupt</fun> at all times.  The
   <arg>ack</arg> parameter is unused and should be ignored. When the processor
   is reset, it forgets a waiting interrupt so it is not necessary to call
   <fun>uninterrupt</fun> during a reset.

   The functions in the <iface>interrupt_ack</iface> interface provides almost
   the same functionality as the <fun>interrupt</fun> and
   <fun>uninterrupt</fun> functions in this interface. The only difference is
   that the <fun>interrupt</fun> function in this interface returns 0 when the
   interrupt cannot be handled which allows the device to retry later.

   The <fun>has_pending_interrupt</fun> and <fun>has_waiting_interrupt</fun>
   calls should return information about interrupts in progress. An interrupt
   is said to be pending if it is acked by the processor and will be taken
   before execution of the next instruction. An interrupt is waiting if the
   logical interrupt line is high, but the interrupt has not yet been
   acked. These functions are used for sanity checks by the APIC. The APIC
   should keep track of posted interrupts by itself. These functions return 1
   if an interrupt is pending/waiting, and 0 otherwise.

   The <fun>logical_to_linear</fun> function returns the translated linear
   address from the given logical address and segment register number. The
   function uses the current content of control registers and segment registers
   to calculate the linear address. The tagged return value is marked invalid
   if no valid translation exists, for example if the passed logical address is
   beyond the segment limit of the passed segment or if it is
   non-canonical. The encoding of <arg>segment</arg> is 0 for ES, 1 for CS, 2
   for SS, 3 for DS, 4 for FS, and 5 for GS.

   The <fun>linear_to_physical</fun> function returns the physical address
   corresponding to the given linear <arg>address</arg>. The function uses the
   current content of control registers, TLBs and possibly page tables to
   calculate the physical address. A return value of -1 signals that no valid
   mapping exists.

   <fun>enter_acpi_c2_state</fun> is called from the chipset power module to
   request the CPU to enter an energy-saving state.

   <insert-until text="// ADD INTERFACE x86_interface"/>

   </add>
   <add id="x86_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86) {
        void (*set_pin_status)(conf_object_t *obj,
                               x86_pin_t pin, int status);
        void (*start_up)(conf_object_t *obj, uint32 start_address);
        int  (*interrupt)(conf_object_t *obj,
                          int (*ack)(conf_object_t *obj),
                          conf_object_t *data);
        void (*uninterrupt)(conf_object_t *obj,
                            int (*ack)(conf_object_t *obj));
        int  (*has_pending_interrupt)(conf_object_t *obj);
        int  (*has_waiting_interrupt)(conf_object_t *obj);
        tagged_linear_address_t (*logical_to_linear)(conf_object_t *obj,
                                                     int segment,
                                                     logical_address_t addr);
        physical_address_t (*linear_to_physical)(conf_object_t *obj,
                                                 data_or_instr_t d_or_i,
                                                 linear_address_t addr);
        void (*enter_acpi_c2_state)(conf_object_t *obj);
};

#define X86_INTERFACE "x86"
// ADD INTERFACE x86_interface

typedef struct {
	int taken;
	uint64 out_a;
	uint64 out_b;
	uint64 out_c;
	uint64 out_d;
} cpuid_ret_t;

/* ADD INTERFACE x86_cpuid_interface */

/* <add id="x86_cpuid_interface_t">
   <insert-until text="// ADD INTERFACE x86_cpuid_interface"/>

   The CPUID interface makes it possible to customize responses to CPUID
   requests. The <fun>cpuid</fun> method should set taken to nonzero if it
   managed to handle the request, zero otherwise. When taken is non-zero,
   then the returned values in out_a, out_b, out_c, and out_d will be
   written to the first four general purpose registers.
   Assigning external handlers to the leaf range reserved for MAGIC instruction
   implementation has no effect, because the MAGIC implementation always takes
   priority.
   </add>
   <add id="x86_cpuid_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(x86_cpuid) {
	cpuid_ret_t (*cpuid)(conf_object_t *obj, conf_object_t *cpu,
                             uint32 in_eax, uint32 in_ebx, uint32 in_ecx,
                             uint32 in_edx);
};
#define X86_CPUID_INTERFACE "x86_cpuid"
// ADD INTERFACE x86_cpuid_interface

typedef struct {
        uint64 a, b, c, d;
} cpuid_value_t;

/* <add id="x86_cpuid_query_interface_t">
   This interface is implemented by CPUs and can be used to query the values
   that would be returned by the CPUID instruction. The interface calculates
   the return value both from built in default values and from any handlers
   that would be installed using the <iface>x86_cpuid</iface> interface.

   This is a preliminary interface. Based on feedback, this interface can and
   will be changed without regard for the usual ABI compatibility rules.

   <insert-until text="// ADD INTERFACE x86_cpuid_query_interface"/>
   </add>

   <add id="x86_cpuid_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(x86_cpuid_query) {
        cpuid_value_t (*cpuid_query)(conf_object_t *obj, uint32 leaf,
                                     uint32 subleaf);
};
#define X86_CPUID_QUERY_INTERFACE "x86_cpuid_query"
// ADD INTERFACE x86_cpuid_query_interface

/* <add-type id="local_apic_interrupt_t"></add-type> */
typedef enum {
        Apic_Lvt_2e = 0x2e0,
        Apic_CMCI = 0x2f0,
        Apic_Performance_Counter = 0x340,
        Apic_Thermal_Sensor = 0x330
} local_apic_interrupt_t;

/* <add id="apic_cpu_interface_t">
   This interface is implemented by the local APIC device and is used by the
   processor and machine initialization code to interact with the local APIC.

   The <fun>tpr_r</fun> and <fun>tpr_w</fun> methods read and write the task
   priority register. The format of the <arg>tpr</arg> argument and the
   returned value from <fun>tpr_r</fun> is the same as for CR8, meaning
   TPR[bits 7:4], zero extended to 64 bits.

   Interrupts coming from the CPU itself are passed via the
   <fun>local_int</fun> function. The type of interrupt is one of the constants
   defined in <type>local_apic_interrupt_t</type>.

   <insert id="local_apic_interrupt_t"/>

   The <fun>init</fun> function is called by the CPU when it receives the INIT
   signal. The APIC should only update its internal state and not propagate
   this signal further. Both the CPU and the APIC should reset their pending
   interrupt flags when this function is called. The <fun>init</fun> function
   is not called at instantiation time.

   The <fun>power_on</fun> function is called at machine creation time, and the
   call initializes the APIC to the power on state. The <arg>bsp</arg> argument
   is true if the APIC is connected to the boot processor, and false
   otherwise. The initial APIC ID is set through the <arg>apic_id</arg>
   argument. The <fun>power_on</fun> function would typically be called from
   component code for the processor that includes the APIC.

   The <fun>enabled_r</fun> method returns bit 11 from the APIC BASE MSR, and
   is used by the processor to return the correct status for CPUID.

   <insert-until text="// ADD INTERFACE apic_cpu_interface"/>

   </add>
   <add id="apic_cpu_interface_exec_context">
   Cell Context for all methods.
   </add> */

SIM_INTERFACE(apic_cpu) {
        uint64 (*tpr_r)(conf_object_t *NOTNULL obj);
        void (*tpr_w)(conf_object_t *NOTNULL obj, uint64 tpr);
        void (*local_int)(conf_object_t *NOTNULL obj,
                          local_apic_interrupt_t int_type);
        void (*power_on)(conf_object_t *NOTNULL obj, bool bsp, int apic_id);
        void (*init)(conf_object_t *NOTNULL obj);
        bool (*enabled_r)(conf_object_t *NOTNULL obj);
};

#define APIC_CPU_INTERFACE "apic_cpu"
// ADD INTERFACE apic_cpu_interface

/* <add id="a20_interface_t">
   This interface is used between the A20 line handling device
   (typically the keyboard controller) and the x86 processor. The processor
   implements this interface and the keyboard controller calls it.

   <insert-until text="// ADD INTERFACE a20_interface"/>
   </add>
   <add id="a20_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(a20) {
        void (*set_a20_line)(conf_object_t *NOTNULL obj, int value);
        int (*get_a20_line)(conf_object_t *NOTNULL obj);
};

#define A20_INTERFACE "a20"
// ADD INTERFACE a20_interface

/* <add-type id="x86_msr_access_type_t def"></add-type> */
typedef enum {
        /* Access from a rdmsr/wrmsr instruction */
        Sim_X86_Msr_Instruction_Access = Sim_Gen_Spr_Instruction_Access,

        /* Access through attribute */
        Sim_X86_Msr_Attribute_Access = Sim_Gen_Spr_Attribute_Access,

        /* Access through int_register interface */
        Sim_X86_Msr_Int_Register_Access = Sim_Gen_Spr_Int_Register_Access,

        /* Access during VMX entry/exit */
        Sim_X86_Msr_VMX_Access,

        /* Architectural access without side effects within the CPU model, only
           reads or writes the register value. However, if it is sent to the
           platform as a non-inquiry access, that may cause side effects. */
        Sim_X86_Msr_Architectural_Access
} x86_msr_access_type_t;

/* <add-type id="x86_msr_ret_t def"></add-type> */
typedef enum {
        Sim_X86_Msr_Ok = Sim_Gen_Spr_Ok,        /* Access was OK */
        Sim_X86_Msr_GP_Fault,                   /* Raise #GP fault */
        Sim_X86_Msr_Not_Handled                 /* Pass on to next handler */
} x86_msr_ret_t;

/* <add-type id="x86_msr_getter_ret_t def"></add-type> */
typedef struct {
        x86_msr_ret_t status;
        uint64 value;
} x86_msr_getter_ret_t;

/* <add-type id="x86_msr_getter_func_t def"></add-type> */
typedef x86_msr_getter_ret_t (*x86_msr_getter_func_t)(
        conf_object_t *cpu,
        int64 number,
        x86_msr_access_type_t type,
        lang_void *user_data);

/* <add-type id="x86_msr_setter_func_t def"></add-type> */
typedef x86_msr_ret_t (*x86_msr_setter_func_t)(
        conf_object_t *cpu,
        int64 spr_number,
        uint64 value,
        x86_msr_access_type_t type,
        lang_void *user_data);

/* <add id="x86_msr_interface_t">
   <insert-until text="// ADD INTERFACE x86_msr_interface"/>

   The <fun>register_handlers</fun> function will register get and set
   functions that will be called every time a read or write access is made to
   the MSR with number <param>msr</param>.

   <insert id="x86_msr_getter_ret_t def"/>
   <insert id="x86_msr_ret_t def"/>
   <insert id="x86_msr_getter_func_t def"/>
   <insert id="x86_msr_setter_func_t def"/>

   The <param>type</param> parameter in the get and set functions is one of
   the following, depending on where from the access originated:

   <insert id="x86_msr_access_type_t def"/>

   The getter function returns the status just like the setter together with
   the read MSR value. A getter or setter may return the
   Sim_X86_MSR_Not_Handled return code, in which case the access will pass
   through to the earlier registered handler (or the default handler if there
   is no earlier registered handler).

   The function <fun>unregister_handlers</fun> will remove any
   non-default handlers for a particular MSR.

   The <fun>get</fun> and <fun>set</fun> functions get and set MSRs, using both
   model default MSR handlers and handlers installed through this interface.

   The function <fun>get_name</fun> takes <param>number</param>
   as parameter and returns the name of the MSR.

   The function <fun>get_number</fun> takes the <param>name</param>
   and returns the MSR number.

   The function <fun>get_all_valid_numbers</fun> returns a list of integers
   corresponding to all the valid MSRs, including both model default MSRs and
   MSRs installed through this interface.

   </add>
   <add id="x86_msr_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(x86_msr) {
        void (*register_handlers)(
                conf_object_t *cpu,
                int64 number,
                x86_msr_getter_func_t getter,
                lang_void *getter_data,
                x86_msr_setter_func_t setter,
                lang_void *setter_data,
                const char *name);
        void (*unregister_handlers)(
                conf_object_t *cpu,
                int64 number);
        x86_msr_getter_ret_t (*get)(
                conf_object_t *cpu,
                int64 number,
                x86_msr_access_type_t type);
        x86_msr_ret_t (*set)(
                conf_object_t *cpu,
                int64 number,
                uint64 value,
                x86_msr_access_type_t type);
        const char *(*get_name)(
                conf_object_t *cpu,
                int64 number);
        int64 (*get_number)(
                conf_object_t *cpu,
                const char *name);
        attr_value_t (*get_all_valid_numbers)(
                conf_object_t *cpu);
};
#define X86_MSR_INTERFACE "x86_msr"
// ADD INTERFACE x86_msr_interface


/* <add id="x86_cache_flush_interface_t">
   The x86 cache flush interface is implemented by objects
   which need to do some special handling when the cpu
   flushes the cache.

   The <b>flush</b> method is invoked when the cpu 
   executes an <b>invd</b> or a <b>wbinvd</b> instruction.
   The <i>writeback</i> parameter is set for the later instruction.

   <insert-until text="// ADD INTERFACE x86_cache_flush_interface"/>
   </add>
   <add id="x86_cache_flush_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_cache_flush) {
        void (*flush)(conf_object_t *obj, conf_object_t *cpu, bool writeback);
};
#define X86_CACHE_FLUSH_INTERFACE "x86_cache_flush"
// ADD INTERFACE x86_cache_flush_interface

/* Attribute format:
   type    bit 0..3
   s       bit 4
   dpl     bit 5..6
   p       bit 7
   avl     bit 12
   l       bit 13
   db      bit 14
   g       bit 15
   invalid bit 16 */
typedef struct {
        uint64        gpr[16];

        uint16        seg_sel[6];
        uint16        tr_sel;
        uint16        ldtr_sel;

        uint64        seg_base[6];
        uint64        tr_base;
        uint64        ldtr_base;

        uint32        seg_attr[6];
        uint32        tr_attr;
        uint32        ldtr_attr;

        uint32        seg_limit[6];
        uint32        tr_limit;
        uint32        ldtr_limit;

        uint64        cr0;
        uint64        cr2;
        uint64        cr3;
        uint64        cr4;
        uint64        cr8;

        uint64        ia32_efer;
        uint64        rip;
        uint64        rflags;

        uint64        dr[4];
        uint64        dr6;
        uint64        dr7;

        uint32        smbase;
        uint16        idtr_limit;
        uint16        gdtr_limit;
        uint64        idtr_base;        
        uint64        gdtr_base;

        bool          activity_hlt;
        bool          io_instr_restart;
        bool          block_nmi;
        uint8         vmx_mode;
        bool          ept_is_on;
        bool          reserved[3];

        uint64        io_rip;
        uint64        io_lin_addr;
        uint32        io_instr_info;
        uint32        reserved2;

        uint64        bndcfgs; /* MPX state */
        uint64        eptp;    /* valid only if EPT is on */

        uint64        pdpte[4];
        uint64        ssp;
        uint64        reserved3[6];
} smm_reg_state_t;

/* <add id="x86_smm_state_interface_t">
   The smm state interface is implemented by the cpu
   and is typically used by objects implementing the
   <iface>x86_smm</iface> interface.

   The <b>get_state</b> method saves the cpu state in the
   <arg>state</arg> struct. This method is intended to be
   used during SMM entry.

   The <b>set_state</b> method restores the cpu state 
   from the <arg>state</arg> struct. This method is intended
   to be used during SMM exit.

   The <b>smram_read</b> and <b>smram_write</b> methods
   can be used to access the SMRAM during SMM entry/exit.

   This interface is internal and may change without notice.
   <insert-until text="// ADD INTERFACE x86_smm_state_interface"/>
   </add>
   <add id="x86_smm_state_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_smm_state) {
        void (*get_state)(conf_object_t *cpu, smm_reg_state_t *state);
        void (*set_state)(conf_object_t *cpu, const smm_reg_state_t *state);

        uint64 (*smram_read)(conf_object_t *obj,
                             unsigned smram_offs, unsigned len);
        void (*smram_write)(conf_object_t *obj,
                            unsigned smram_offs, unsigned len, uint64 val);
};
#define X86_SMM_STATE_INTERFACE "x86_smm_state"
// ADD INTERFACE x86_smm_state_interface

/* <add id="x86_smm_interface_t">
   The smm interface is used to implement a custom SMRAM
   state save map.

   The <b>save_state</b> method is invoked when SMM mode
   is entered before any cpu state has been modified.
   The method should save the current cpu state
   to the SMRAM state save map.

   The <b>restore_state</b> method is invoked during execution
   of the <b>rsm</b> instruction. The method should
   restore the cpu state from the SMRAM state save map.

   The <iface>x86_smm_state</iface> interface implemented by
   the cpu can be used to facilitate cpu state changes and
   SMRAM access.

   This interface is internal and may change without notice.
   <insert-until text="// ADD INTERFACE x86_smm_interface"/>
   </add>
   <add id="x86_smm_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_smm) {
        void (*save_state)(conf_object_t *obj, conf_object_t *cpu);
        void (*restore_state)(conf_object_t *obj, conf_object_t *cpu);
};
#define X86_SMM_INTERFACE "x86_smm"
// ADD INTERFACE x86_smm_interface

/* <add-type id="x86_xmode_info_t"> </add-type> */
typedef struct {
        bool efer_lma;
        bool cs_l;
        bool cs_d;
        bool ss_b;
} x86_xmode_info_t;

/* x86_seg_reg_t describes a segment register and its hidden fields. If valid
   is false, the segment is unusable and attempts to use it will trigger a
   fault in non-64-bit mode. The meaning of each bit in attr is as follows:
   type    bit 0..3
   S       bit 4
   DPL     bit 5..6
   P       bit 7
   L       bit 13
   D/B     bit 14
   G       bit 15 */
typedef struct {
        bool valid;
        uint16 sel;
        uint64 base;
        uint32 attr;
        uint32 limit;
} x86_seg_reg_t;

typedef struct {
        uint64 base;
        uint16 limit;
} x86_system_seg_reg_t;

typedef enum {
        X86_Gdtr,
        X86_Idtr
} x86_system_seg_t;

typedef enum {
        X86_Cr0 = 0,
        X86_Cr2 = 2,
        X86_Cr3 = 3,
        X86_Cr4 = 4,
        X86_Cr8 = 8
} x86_cr_t;

typedef enum {
        X86_Dr0 = 0,
        X86_Dr1 = 1,
        X86_Dr2 = 2,
        X86_Dr3 = 3,
        X86_Dr6 = 6,
        X86_Dr7 = 7,
} x86_dr_t;

typedef enum {
        X86_Xcr0 = 0
} x86_xcr_t;

typedef enum {
        X86_Activity_Normal = 0,
        X86_Activity_Hlt = 1,
        X86_Activity_Shutdown = 2,
        X86_Activity_Wait_For_SIPI = 3,
        X86_Activity_Cx_State = 4,
        X86_Activity_MWait = 5,
        X86_Activity_Senter_Sleep_State = 6,
} x86_activity_t;

/* <add-type id="ymm_reg_t"> </add-type> */
typedef struct {
        uint64 llo64;
        uint64 lhi64;
        uint64 hlo64;
        uint64 hhi64;
} ymm_reg_t;

/* <add-type id="xmm_reg_t"> </add-type> */
typedef struct {
        uint64 lo64;
        uint64 hi64;
} xmm_reg_t;

/* <add-type id="x86_exec_mode_t"> </add-type> */
typedef enum {
        X86_Exec_Mode_Real,
        X86_Exec_Mode_V86,
        X86_Exec_Mode_Prot,
        X86_Exec_Mode_Compat,
        X86_Exec_Mode_64
} x86_exec_mode_t;

/* <add-type id="x86_detailed_exec_mode_t"> </add-type> */
typedef enum {
        X86_Detailed_Exec_Mode_Real_16,
        X86_Detailed_Exec_Mode_Real_32,
        X86_Detailed_Exec_Mode_V86,
        X86_Detailed_Exec_Mode_Protected_16,
        X86_Detailed_Exec_Mode_Protected_32,
        X86_Detailed_Exec_Mode_Protected_64,
        X86_Detailed_Exec_Mode_Compatibility_16,
        X86_Detailed_Exec_Mode_Compatibility_32,
} x86_detailed_exec_mode_t;

typedef struct {
        bool armed;
        uint64 address;
        uint64 extensions;
        uint64 hints;
} x86_monitor_info_t;

typedef struct {
        uint64 extensions;
        uint64 hints;
} x86_mwait_info_t;

typedef struct {
        bool pending;
        uint64 pending_dr6;
} x86_pending_debug_exc_t;

/* <add-type id="x86_fpu_reg_t"> </add-type> */
typedef struct {
        uint64 low;
        uint16 high;
} x86_fpu_reg_t;

/* <add-type id="x86_fpu_env_t"> </add-type> */
typedef struct {
        uint16 cw;
        uint16 sw;
        uint16 tag;
        uint16 opc;
        uint64 last_instr_ptr;
        uint64 last_operand_ptr;
        uint16 last_instr_sel;
        uint16 last_operand_sel;        
        uint32 pad;
} x86_fpu_env_t;

/* <add-type id="x86_seg_t"> </add-type> */
typedef enum {
        X86_Seg_ES,
        X86_Seg_CS,
        X86_Seg_SS,
        X86_Seg_DS,
        X86_Seg_FS,
        X86_Seg_GS,
        X86_Seg_LDTR,
        X86_Seg_TR,
        X86_Seg_None,
} x86_seg_t;

typedef enum {
        X86_Intstate_Not_Blocking = 0,
        X86_Intstate_Blocking_INT_Sti = 1,
        X86_Intstate_Blocking_INT_Mov_Ss = 2,
        X86_Intstate_Blocking_INIT = 4,
        X86_Intstate_Blocking_SMI = 8,
        X86_Intstate_Blocking_NMI = 16
} x86_intstate_t;

/* <add id="x86_reg_access_interface_t">
   The <iface>x86_reg_acc_access</iface> interface can be
   used to access x86 register state in an efficient manner.
   The interface is intended to be used from user decoder
   service routines.

   This interface is internal and may change without notice.
   <insert-until text="// ADD INTERFACE x86_reg_access_interface"/>
   </add>
   <add id="x86_reg_access_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_reg_access) {
        uint64 (*get_gpr)(conf_object_t *cpu, unsigned index);
        void (*set_gpr)(conf_object_t *cpu, unsigned index, uint64 val);

        uint64 (*get_rip)(conf_object_t *cpu);
        void (*set_rip)(conf_object_t *cpu, uint64 rip);

        uint64 (*get_rflags)(conf_object_t *cpu);
        void (*set_rflags)(conf_object_t *cpu, uint64 rflags);

        /* Possibly faster method for setting only cf,pf,af,zf,sf,of */
        void (*set_status_flags)(conf_object_t *cpu, uint16 flags);

        /* Regular segment registers */
        x86_seg_reg_t (*get_seg)(conf_object_t *cpu, x86_seg_t n);
        void (*set_seg)(conf_object_t *cpu, x86_seg_t n, x86_seg_reg_t seg);

        /* IDTR and GDTR */
        x86_system_seg_reg_t (*get_system_seg)(conf_object_t *cpu,
                                               x86_system_seg_t n);
        void (*set_system_seg)(conf_object_t *cpu, x86_system_seg_t n,
                               x86_system_seg_reg_t seg);

        /* Control registers */
        uint64 (*get_cr)(conf_object_t *cpu, x86_cr_t n);
        void (*set_cr)(conf_object_t *cpu, x86_cr_t n, uint64 val);
        uint64 (*get_efer)(conf_object_t *cpu);
        void (*set_efer)(conf_object_t *cpu, uint64 efer);
        uint64 (*get_xcr)(conf_object_t *cpu, x86_xcr_t n);
        void (*set_xcr)(conf_object_t *cpu, x86_xcr_t n, uint64 val);

        /* x87 FPU */
        x86_fpu_reg_t (*get_freg)(conf_object_t *cpu, unsigned index);
        void (*set_freg)(conf_object_t *cpu, unsigned index, x86_fpu_reg_t freg);
        x86_fpu_env_t (*get_fpu_env)(conf_object_t *cpu);
        void (*set_fpu_env)(conf_object_t *cpu, x86_fpu_env_t state);

        /* SIMD */
        xmm_reg_t (*get_xmm)(conf_object_t *cpu, unsigned index);
        void (*set_xmm)(conf_object_t *cpu, unsigned index, xmm_reg_t val);
        ymm_reg_t (*get_ymm)(conf_object_t *cpu, unsigned index);
        void (*set_ymm)(conf_object_t *cpu, unsigned index, ymm_reg_t val);
        uint32 (*get_mxcsr)(conf_object_t *cpu);
        void (*set_mxcsr)(conf_object_t *cpu, uint32 mxcsr);

        /* Debug registers */
        uint64 (*get_dr)(conf_object_t *cpu, x86_dr_t n);
        void (*set_dr)(conf_object_t *cpu, x86_dr_t n, uint64 val);

        /* SMM */
        bool (*get_in_smm)(conf_object_t *cpu);
        void (*set_in_smm)(conf_object_t *cpu, bool val);
        uint64 (*get_smm_base)(conf_object_t *cpu);
        void (*set_smm_base)(conf_object_t *cpu, uint64 val);

        /* Monitor/Mwait */
        x86_monitor_info_t (*get_monitor_info)(conf_object_t *cpu);
        void (*set_monitor_info)(conf_object_t *cpu, x86_monitor_info_t val);
        x86_mwait_info_t (*get_mwait_info)(conf_object_t *cpu);
        void (*set_mwait_info)(conf_object_t *cpu, x86_mwait_info_t val);

        /* Non-register state */
        x86_activity_t (*get_activity_state)(conf_object_t *cpu);
        void (*set_activity_state)(conf_object_t *cpu, x86_activity_t val);
        x86_intstate_t (*get_interruptibility_state)(conf_object_t *cpu);
        void (*set_interruptibility_state)(conf_object_t *cpu,
                                           x86_intstate_t val);

        /* A debug exception is pending if triggered by the last instruction,
           but not yet taken. */
        x86_pending_debug_exc_t (*get_pending_debug_exc)(conf_object_t *cpu);
        void (*set_pending_debug_exc)(conf_object_t *cpu,
                                      x86_pending_debug_exc_t val);

        /* Derived state for fast/easy access */
        x86_xmode_info_t (*get_xmode_info)(conf_object_t *cpu);
        x86_exec_mode_t (*get_exec_mode)(conf_object_t *cpu);

        /* This is configuration info. No need for a set method. */
        uint32 (*get_mxcsr_mask)(conf_object_t *cpu);

        /* Extension state dirty bit modification (for XSAVE) */
        uint64 (*get_ext_state_dirty_bits)(conf_object_t *cpu);
        void (*or_ext_state_dirty_bits)(conf_object_t *cpu, uint64 dirty_bits);
        void (*and_ext_state_dirty_bits)(conf_object_t *cpu, uint64 dirty_bits);

        /* PDPTE registers used in PAE paging mode */
        uint64 (*get_pdpte)(conf_object_t *cpu, int num);
        void (*set_pdpte)(conf_object_t *cpu, int num, uint64 val);

        uint32 (*get_vmx_mode)(conf_object_t *cpu);
};
#define X86_REG_ACCESS_INTERFACE "x86_reg_access"
// ADD INTERFACE x86_reg_access_interface

/* <add id="x86_exception_interface_t">
   The methods in this interface is used to raise exceptions
   from the instrumentation replace or user decoder functionality (it is illegal
   to invoke these methods from any other context). The
   methods perform longjmps and do not return.

   This interface is internal and may change without notice.

   <insert-until text="// ADD INTERFACE x86_exception_interface"/>
   </add>
   <add id="x86_exception_interface_exec_context">
   The methods in this interface may only be called from
   user decoder service routines. 
   </add> */
SIM_INTERFACE(x86_exception) {
        void (*DE_fault)(conf_object_t *cpu);

        void (*BR_fault)(conf_object_t *cpu);
        void (*UD_fault)(conf_object_t *cpu);
        void (*NM_fault)(conf_object_t *cpu);
        void (*DF_abort)(conf_object_t *cpu, const char *desc);

        void (*TS_fault)(conf_object_t *cpu, uint16 sel);
        void (*NP_fault)(conf_object_t *cpu, uint16 sel_vec, bool is_vec);

        void (*SS_fault)(conf_object_t *cpu, uint16 sel);
        void (*GP_fault)(conf_object_t *cpu, uint16 sel_vec, bool is_vec,
                         const char *desc);
        void (*PF_fault)(conf_object_t *cpu, linear_address_t laddr,
                         uint32 ecode);
        void (*MF_fault)(conf_object_t *cpu);
        void (*AC_fault)(conf_object_t *cpu);
        void (*XM_fault)(conf_object_t *cpu);
};
#define X86_EXCEPTION_INTERFACE "x86_exception"
// ADD INTERFACE x86_exception_interface

/* <add-type id="x86_exception_source"> 
   Defines different exception sources for a given exception/interrupt.
 *</add-type> */
typedef enum {
        X86_Exc_Hardware,
        X86_Exc_Software,
        X86_Exc_External,
        X86_Exc_Software_Debug,
        X86_Exc_Software_Priv,
        X86_Exc_Triple_Fault,
        X86_Exc_NMI,
        X86_Exc_Other_Event
} x86_exception_source_t;
        
/* <add-type id="x86_processor_mode_t"> </add-type> */
typedef enum {
        X86_Processor_Mode_Privileged,
        X86_Processor_Mode_User,
        X86_Processor_Mode_Current
} x86_processor_mode_t;

/* <add-type id="x86_alignment_t"> </add-type> */
typedef enum {
        X86_Alignment_Not_Required,
        X86_Alignment_Required
} x86_alignment_t;

/* <add-type id="x86_read_physical_ret_t"> </add-type> */
typedef struct {
        uint64 value;
        exception_type_t exception;
} x86_read_physical_ret_t;
        
/* <add id="x86_memory_access_interface_t">
   The <iface>x86_memory_access</iface> interface is used to access memory
   from user decoder service routines. If an exception condition
   occurs, a longjmp is taken. The interface methods may only be invoked from
   user decoder service routines.

   This interface is internal and may change without notice.
   <insert-until text="// ADD INTERFACE x86_memory_access_interface"/>
   </add>
   <add id="x86_memory_access_interface_exec_context">
   The methods in this interface may only be called from
   user decoder service routines. 
   </add> */
SIM_INTERFACE(x86_memory_access) {
        uint64 (*read_logical)(
                conf_object_t *cpu,
                logical_address_t offs,
                x86_seg_t seg,
                unsigned len,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#if !defined(PYWRAP)
        void (*read_logical_buf)(
                conf_object_t *cpu, 
                logical_address_t offs,
                x86_seg_t seg,
                unsigned len,
                void *p,
                x86_processor_mode_t mode,
                x86_access_type_t access_type,
                x86_alignment_t alignment);
#endif
        void (*write_logical)(
                conf_object_t *cpu,
                logical_address_t offs,
                x86_seg_t seg,
                unsigned len,
                uint64 val,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#if !defined(PYWRAP)
        void (*write_logical_buf)(
                conf_object_t *cpu,
                logical_address_t offs,
                x86_seg_t seg,
                unsigned len,
                const void *p,
                x86_processor_mode_t mode,
                x86_access_type_t access_type,
                x86_alignment_t alignment);
#endif
};
#define X86_MEMORY_ACCESS_INTERFACE "x86_memory_access"
// ADD INTERFACE x86_memory_access_interface

/* <add id="x86_memory_operation_interface_t"> 

   The <iface>x86_memory_operation</iface> interface is used to access memory
   from a user instruction. See the
   <iface>cpu_instrumentation_subscribe</iface> and
   <iface>cpu_instruction_decoder</iface> for more information. It is designed
   to be more efficient than the <iface>x86_memory_access</iface>. If an
   exception condition occurs, a longjmp is taken. The interface methods may
   only be invoked from user instruction.

   <insert-until text="// ADD INTERFACE x86_memory_operation"/>
   </add>
   <add id="x86_memory_operation_interface_exec_context">
   Cell Context for all methods, but must be called from an invoke
   function set by the <iface>instrumentation_replace</iface> interface.
   </add>
 */
SIM_INTERFACE(x86_memory_operation) {
        uint64 (*read_logical)(
                conf_object_t *cpu,
                x86_seg_t seg,
                logical_address_t offs,
                unsigned size,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#if !defined(PYWRAP)
        void (*read_logical_buf)(
                conf_object_t *cpu,
                uint8 *dst,
                x86_seg_t seg,
                logical_address_t offs,
                unsigned size,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#endif
        void (*write_logical)(
                conf_object_t *cpu,
                uint64 val,
                x86_seg_t seg,
                logical_address_t offs,
                unsigned size,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#if !defined(PYWRAP)
        void (*write_logical_buf)(
                conf_object_t *cpu,
                const uint8 *src,
                x86_seg_t seg,
                logical_address_t offs,
                unsigned size,
                x86_processor_mode_t mode,
                x86_access_type_t access_type);
#endif
        x86_read_physical_ret_t (*read_physical)(
                conf_object_t *cpu,
                physical_address_t address,
                unsigned size,
                x86_access_type_t access_type);
#if !defined(PYWRAP)
        exception_type_t (*read_physical_buf)(
                conf_object_t *cpu,
                uint8 *dst,
                physical_address_t address,
                unsigned size,
                x86_access_type_t access_type);
#endif
        exception_type_t (*write_physical)(
                conf_object_t *cpu,
                uint64 val,
                physical_address_t address,
                unsigned size,
                x86_access_type_t access_type);
#if !defined(PYWRAP)  
        exception_type_t (*write_physical_buf)(
                conf_object_t *cpu,
                const uint8 *src,
                physical_address_t address,
                unsigned size,
                x86_access_type_t access_type);
#endif
};

#define X86_MEMORY_OPERATION_INTERFACE "x86_memory_operation"
// ADD INTERFACE x86_memory_operation
        
/* <add id="x86_vmp_control_interface_t">
   The <iface>x86_vmp_control</iface> interface can be
   used to prevent VMP from being used.

   The <b>set_block_count</b> method sets the VMP block
   count. If the VMP block count is non-zero, VMP will not be
   used for simulation.

   The <b>get_block_count</b> method returns VMP block count.

   This interface is internal and may change without notice.
   <insert-until text="// ADD INTERFACE x86_vmp_control_interface"/>
   </add>
   <add id="x86_vmp_control_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(x86_vmp_control) {
        unsigned (*get_block_count)(conf_object_t *cpu);
        void (*set_block_count)(conf_object_t *cpu, unsigned cnt);
};
#define X86_VMP_CONTROL_INTERFACE "x86_vmp_control"
// ADD INTERFACE x86_vmp_control_interface

typedef enum {
        Vmp_Feature_Ept,
        Vmp_Feature_Tpr_Threshold,
        Vmp_Feature_Unrestricted_Guest,
        Vmp_Feature_Backoff,
        Vmp_Feature_Shadow_VMCS,
        Vmp_Feature_Nested_EPT,
        Vmp_Feature_Direct_Rdtsc,
        Vmp_Feature_Five_Level_Paging
} vmp_feature_t;

typedef enum {
        Vmp_Info_Cpu_Version,
        Vmp_Info_Vmxmon_Version
} vmp_info_t;

/* <add id="vmp_interface_t">

   The <iface>vmp</iface> interface is used to control enabling of VMP and VMP
   features from a user perspective. Models should use
   <iface>x86_vmp_control</iface> for potential blocking of VMP.

   This is an internal interface between VMP and the Simics Base package, and
   it may change at any time without notice.

   <insert-until text="// ADD INTERFACE vmp_interface"/>
   </add>
   <add id="vmp_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(vmp) {
        bool (*class_has_support)(conf_object_t *obj);
        bool (*host_support)(conf_object_t *obj);
        bool (*compatible_config)(conf_object_t *obj);
        bool (*enable)(conf_object_t *obj);
        void (*disable)(conf_object_t *obj);
        bool (*enabled)(conf_object_t *obj);
        void (*set_threshold)(conf_object_t *obj, int threshold);
        bool (*get_feature)(conf_object_t *obj, vmp_feature_t feature);
        bool (*set_feature)(conf_object_t *obj, vmp_feature_t feature, bool val);
        uint64 (*get_info)(conf_object_t *obj, vmp_info_t info);
};
#define VMP_INTERFACE "vmp"
// ADD INTERFACE vmp_interface

typedef enum {
        Xed_Iform,
} xed_data_type_t;

/* <add id="xed_access_interface_t">

   The <iface>xed_access</iface> interface is implemented by a processor model.
   The interface can be used to call Intel&reg; X86 Encoder Decoder (Intel\reg;
   XED) library built into the processor model.

   This interface is internal and may change at any time without notice.

   <insert-until text="// ADD INTERFACE xed_access_interface"/>
   </add>
   <add id="xed_access_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(xed_access) {
        int (*get_last)(conf_object_t *obj, xed_data_type_t type);
        int (*decode)(conf_object_t *obj, xed_data_type_t type, bytes_t bytes);
        const char *(*to_string)(conf_object_t *obj, xed_data_type_t type,
                                 int value);
};

#define XED_ACCESS_INTERFACE "xed_access"
// ADD INTERFACE xed_access_interface

/*
   <add id="x86_ept_interface_t">

   The interface provides insight into the Extended Page Table (EPT) address
   translation performed by target processors for virtual machine operations.
   It takes a result of the first level translation (the classical paging),
   called a guest physical address, and returns matching host physical address.

   The <fun>guest_physical_to_physical</fun> function translates a guest
   physical <param>address</param> to a host physical address. The
   <param>cpu_mode</param> argument sets the processor mode for the lookup.
   Access type is defined by <param>access</param>. The function returns a
   <em>physical_block_t</em> struct with <param>valid</param> bit and the
   <param>address</param>. The host physical address is valid when the valid
   flag is not 0. The function also returns <param>block_start</param> and
   <param>block_end</param>. The start and end of a block has the same linear
   mapping as the translated address. The range is inclusive, so block_end
   should be the address of the last byte of the block. This information can
   be used to figure out how often the function needs to be called for
   traversing larger address ranges.

   If the chosen processor's current mode does not use EPT translation,
   the function should use the identity mapping between guest and host physical
   addresses. An example of such situation would be an x86 processor outside
   of VMX non-root mode or with EPT controls disabled.

   To check if the processor's current VMCS state has EPT enabled,
   <fun>is_ept_active</fun> function should be used.

   <insert-until text="// ADD INTERFACE x86_ept_interface_t"/>
   </add>
   <add id="x86_ept_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(x86_ept) {
        physical_block_t (*guest_physical_to_physical)(
                conf_object_t *obj,
                generic_address_t address,
                x86_processor_mode_t cpu_mode,
                access_t access);
        bool (*is_ept_active)(conf_object_t* obj);
};

#define X86_EPT_INTERFACE "x86_ept"
// ADD INTERFACE x86_ept_interface_t

#if defined(__cplusplus)
}
#endif

#endif
