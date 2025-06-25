/*
  © 2025 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_ARCH_X86_MEMOP_H
#define SIMICS_ARCH_X86_MEMOP_H

#include <simics/base/memory-transaction.h>
#include <simics/processor/types.h>
#include <simics/processor/generic-spr.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_X86_ACCESS_TYPES(OP)                                                \
  OP(0,Othe,X86_Other,                 "Access that is not categorized (rare)") \
  OP(0,Vani,X86_Vanilla,               "Normal access such as a MOV to/from")   \
  OP(1,Inst,X86_Instruction,           "Instruction fetch")                     \
  OP(0,Clfl,X86_Clflush,               "Cache line flush from CLFLUSH")         \
  OP(0,Fenv,X86_Fpu_Env,               "Floating point environment (FLDENV,"    \
                                       " FNSTENV part of FRSTOR, FNSAVE)")      \
  OP(0,Fste,X86_Fpu_State,             "Register part of FRSTOR and FNSAVE")    \
  OP(1,Idt ,X86_Idt,                   "Interrupt descriptor table")            \
  OP(1,Gdt ,X86_Gdt,                   "Global descriptor table")               \
  OP(1,Ldt ,X86_Ldt,                   "Local descriptor table")                \
  OP(1,Tseg,X86_Task_Segment,          "Task segment")                          \
  OP(1,Tswi,X86_Task_Switch,           "Task save/restore during a task switch")\
  OP(1,CPar,X86_Far_Call_Parameter,    "Parameter copy in far call")            \
  OP(1,Stac,X86_Stack,                 "Stack accesses during complex control"  \
                                       " flow (exception handling etc)")        \
  OP(1,Pml4,X86_Pml4,                  "Page map level 4 table")                \
  OP(1,Pdp ,X86_Pdp,                   "Page directory pointer table")          \
  OP(1,Pd  ,X86_Pd,                    "Page directory table")                  \
  OP(1,Pt  ,X86_Pt,                    "Page table")                            \
  OP(0,Sse ,X86_Sse,                   "16-byte operations for SSE registers")  \
  OP(0,Fpu ,X86_Fpu,                   "10-byte and 16-byte operations"         \
                                       " to/from X87 registers")                \
  OP(0,AccS,X86_Access_Simple,         "Device accesses (DMA)")                 \
  OP(1,UCP ,X86_Microcode_Update,      "Data read when performing a processor"  \
                                       " microcode update")                     \
  OP(0,NTmp,X86_Non_Temporal,          "Non temporal store (example: MOVNTI)")  \
  OP(0,Pr3 ,X86_Prefetch_3DNow,        "Prefetch with PREFETCH (3DNow!)")       \
  OP(0,Prw3,X86_Prefetchw_3DNow,       "Prefetch with PREFETCHW (3DNow!)")      \
  OP(0,PrT0,X86_Prefetch_T0,           "Prefetch with PREFETCHT0 (SSE)")        \
  OP(0,PrT1,X86_Prefetch_T1,           "Prefetch with PREFETCHT1 (SSE)")        \
  OP(0,PrT2,X86_Prefetch_T2,           "Prefetch with PREFETCHT2 (SSE)")        \
  OP(0,PrNT,X86_Prefetch_NTA,          "Prefetch with PREFETCHNTA (SSE)")       \
  OP(0,Lall,X86_Loadall,               "State read by the loadall instruction") \
  OP(1,AInf,X86_Atomic_Info,           "Transaction sent with size 0 to"        \
                                       " finish an atomic transaction")         \
  OP(0,X16B,X86_Cmpxchg16b,            "CMPXCHG16B instruction")                \
  OP(1,SMM ,X86_Smm_State,             "SMM state structure")                   \
  OP(1,VMCS,X86_Vmcs,                  "Virtual-machine control data structure")\
  OP(1,VMX ,X86_Vmx_IO_Bitmap,         "VMX operation I/O bitmap")              \
  OP(1,VMX ,X86_Vmx_Vapic,             "VMX operation Virtual-APIC")            \
  OP(1,VMX ,X86_Vmx_Msr,               "VMX operation MSR area")                \
  OP(1,VMX ,X86_Vmx_Msr_Bitmaps,       "VMX operation MSR bitmaps")             \
  OP(1,Pml4e,X86_Pml4e,                "EPT PML4 table")                        \
  OP(1,Pdpte,X86_Pdpte,                "EPT page-directory-pointer table")      \
  OP(1,Pde ,X86_Pde,                   "EPT page directory")                    \
  OP(1,Pte ,X86_Pte,                   "EPT page table")                        \
  OP(0,IEpt,X86_Invept_Descriptor,     "INVEPT descriptor")                     \
  OP(1,Sstac,X86_Shadow_Stack,         "Shadow stack accesses during shadow"    \
                                       " stack or control flow operations")     \
  OP(0,Ivpid,X86_Invvpid_Descriptor,   "INVVPID descriptor")                    \
  OP(1,Ipt ,X86_Processor_Trace,       "Intel Processor Trace")                 \
  OP(1,Pml5,X86_Pml5,                  "Page map level 5 table")                \
  OP(1,Pml5e,X86_Pml5e,                "EPT PML5 table")                        \
  OP(1,PdpPa,X86_Pdp_Pae,              "Page directory pointer table in PAE"    \
                                       " mode")                                 \
  OP(1,VMX ,X86_Vmx_EPTP_List,         "VMX operation with EPTP-list")          \
  OP(1,VMX ,X86_Vmx_VE_Info,           "VMX operation with #VE info page")      \
  OP(1,Sppt,X86_Sppt,                  "SPP tables")                            \
  OP(1,PmlLe,X86_Pml_Log,              "EPT PML log")                           \
  OP(1,Pebs,X86_Pebs,                  "Precise event based sampling")          \
  OP(1,VmxPt,X86_Vmx_Pasid_Translation,"Translation between guest and host"     \
                                       " Process Address Space IDs")            \
  OP(0,Stac,X86_Stack_Explicit,        "Explicit stack accesses")               \
  OP(0,MttMetadata,X86_Mtt_Metadata,   "MTT metadata table access")             \
  OP(0,PrOth,X86_Prefetch_Other,       "Uncategorized prefetches")              \
  OP(0,Last,X86_Access_Type_Last,      "Always last in public types")

#define X86_ACCESS_TYPE_ENUM(implicit, short, enumval, str) \
        enumval,

/* <add-type id="x86_access_type_t">
   See online help for expanded output of this type:
   api-help x86_access_type_t
   </add-type> */
typedef enum x86_access_type {
        FOR_X86_ACCESS_TYPES(X86_ACCESS_TYPE_ENUM)
} x86_access_type_t;

/* <add-type id="x86_memory_type_t"> </add-type> */
typedef enum {
        X86_None,
        X86_Strong_Uncacheable,    /* UC */
        X86_Uncacheable,           /* UC- */
        X86_Write_Combining,       /* WC */
        X86_Write_Through,         /* WT */
        X86_Write_Back,            /* WB */
        X86_Write_Protected        /* WP */
} x86_memory_type_t;

/* <add id="devs api types">
   <name index="true">x86_memory_transaction_t</name>
   <insert id="x86_memory_transaction_t DOC"/>
   </add> */

/* <add id="x86_memory_transaction_t DOC">
   <ndx>x86_memory_transaction_t</ndx>
   <name index="true">x86_memory_transaction_t</name>
   <doc>
   <doc-item name="NAME">x86_memory_transaction_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="x86_memory_transaction_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">

   The <i>s</i> field contains generic information about memory
   operations (see <tt>generic_transaction_t</tt>).
   
   The <i>mode</i> is the current mode (user or supervisor) of the cpu.

   The <i>linear_address</i> contains the address for transactions
   with linear addresses.

   The <i>access_linear</i> flag is set for all transactions with
   linear addresses.

   The <i>io</i> flag is set on port accesses (from IN and OUT instructions).
   It is cleared for regular memory accesses, and also for memory mapped I/O.

   The <i>fault_as_if_write</i> flag indicates that an access should set
   the page fault access bits as a write even if the access is a read.

   The <i>access_type</i> field contains the type of the transaction.
   <ndx>x86_access_type_t</ndx>
   <smaller>
   <insert id="x86_access_type_t"/>
   </smaller>

   The effective memory type for the access is contained in
   <i>effective_type</i>. The MMU calculates the effective memory type and uses
   the <i>pat_type</i> and <i>mtrr_type</i> members as temporary storage and
   input to that calculation. The <i>pat_type</i> and <i>mtrr_type</i> members
   should not be used by code outside of the MMU.

   <ndx>x86_memory_type_t</ndx>
   <smaller>
   <insert id="x86_memory_type_t"/>
   </smaller>
   </doc-item>
   </doc>

   </add> */

/* <add id="x86_memory_transaction_t def">
   <insert-until text="// JDOCU INSERT-UNTIL x86_memory_transaction_t"/>
   </add-type> 
*/
typedef struct x86_memory_transaction {
        generic_transaction_t s;                /* Superclass */
        linear_address_t      linear_address;   
        physical_address_t    guest_physical_address;
        uint16                segnum;           /* segment number */
        uint16                access_linear:1;  /* Linear access */
        uint16                io:1;             /* I/O (port) access */
        uint16                fault_as_if_write:1;
        uint16                guest_phys_valid:1;
        processor_mode_t      mode;
        x86_access_type_t     access_type;
        x86_memory_type_t     pat_type;
        x86_memory_type_t     mtrr_type;
        x86_memory_type_t     effective_type;
        int                   sequence_number; /* used for -stall */
} x86_memory_transaction_t;
// JDOCU INSERT-UNTIL x86_memory_transaction_t

#if defined(__cplusplus)
}
#endif

#endif
