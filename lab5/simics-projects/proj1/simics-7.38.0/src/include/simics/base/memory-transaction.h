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

#ifndef SIMICS_BASE_MEMORY_TRANSACTION_H
#define SIMICS_BASE_MEMORY_TRANSACTION_H

#ifndef TURBO_SIMICS

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/base/memory.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef PYWRAP
typedef struct transaction transaction_t;
typedef struct pci_memory_transaction pci_memory_transaction_t;
#endif

/* Bits in memory transaction types */
enum {
        Sim_Trn_Instr    = 1,
        Sim_Trn_Write    = 2,
        Sim_Trn_Control  = 4,   /* Not a memory transaction. Signifies that
                                   this is not an exchange of data between CPU
                                   and memory. */
        Sim_Trn_Prefetch = 8    /* no defined semantics really, may go away */
};

/* <add id="device api types">
   <name index="true">mem_op_type_t</name>
   <insert id="mem_op_type_t DOC"/>
   </add> */

/* <add id="mem_op_type_t DOC">
   <ndx>mem_op_type_t</ndx>
   <name index="true">mem_op_type_t</name>
   <doc>
   <doc-item name="NAME">mem_op_type_t</doc-item>
   <doc-item name="SYNOPSIS">
   <smaller>
   <insert id="mem_op_type_t def"/>
   </smaller>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This enum is used to identify the type of a memory operation. The
   function <fun>SIM_get_mem_op_type()</fun> returns the type of a 
   <type>generic_transaction_t</type>, and <fun>SIM_set_mem_op_type()</fun>
   is used to set it.
   </doc-item>
   <doc-item name="SEE ALSO">
     <fun>SIM_get_mem_op_type</fun>, <fun>SIM_set_mem_op_type</fun>,
     <fun>SIM_get_mem_op_type_name</fun>
     <type>generic_transaction_t</type>,
   </doc-item>
   </doc>
   </add>

   <add-type id="mem_op_type_t def"></add-type>
 */
typedef enum {
        Sim_Trans_Load          = 0,
        Sim_Trans_Store         = Sim_Trn_Write,
        Sim_Trans_Instr_Fetch   = Sim_Trn_Instr,
        Sim_Trans_Prefetch      = Sim_Trn_Prefetch | Sim_Trn_Control,
        Sim_Trans_Cache         = Sim_Trn_Control
} mem_op_type_t; 

/* <add-type id="ini_type_t"></add-type> */
typedef enum {
        Sim_Initiator_Illegal         = 0x0,    /* catch uninitialized */
        Sim_Initiator_CPU             = 0x1000,
        Sim_Initiator_CPU_V9          = 0x1100,
        Sim_Initiator_CPU_X86         = 0x1200,
        Sim_Initiator_CPU_PPC         = 0x1300,
        Sim_Initiator_CPU_Alpha       = 0x1400,
        Sim_Initiator_CPU_MIPS        = 0x1600,
        Sim_Initiator_CPU_ARM         = 0x1700,
        Sim_Initiator_CPU_V8          = 0x1800,
        Sim_Initiator_CPU_H8          = 0x1900,
        Sim_Initiator_CPU_SH          = 0x1a00,
        Sim_Initiator_Device          = 0x2000,
        Sim_Initiator_PCI_Device      = 0x2010,
        Sim_Initiator_Cache           = 0x3000, /* The transaction is a cache
                                                   transaction as defined by
                                                   g-cache */
        Sim_Initiator_Other           = 0x4000, /* initiator == NULL */
        Sim_Initiator_User_Defined    = 0x5000, /* 0x5000 - 0x5fff are for user-
                                                   written objects that need to
                                                   tag mem-ops. */
} ini_type_t;

/* <add id="block_flag_t DOC">
   <ndx>block_flag_t</ndx>
   <name index="true">block_flag_t</name>
   <doc>
   <doc-item name="NAME">block_flag_t</doc-item>
   <doc-item name="SYNOPSIS">
   <insert id="block_flag_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   This enum are flags for blocking various internal caches (internal).
   </doc-item>
   </doc>
   </add>

   <add-type id="block_flag_t def"></add-type>
 */
typedef enum {
        Sim_Block_Memhier = 1,               /* want memhier or timing model
                                                to see future accesses */
        Sim_Block_MMU = 2,                   /* want future MMU visibility */
        Sim_Block_MMU_IOSTC = 4,             /* want future MMU visibility
                                                for accesses that skip the
                                                IOSTC lookup */
        Sim_Block_Breakpoint = 8,            /* need breakpoint check */
        Sim_Block_Notmem = 16,               /* is not memory */
        Sim_Block_Endian = 32,               /* memory is byteswapped */
        Sim_Block_DSTC_Disabled = 64         /* DSTC is disabled for this
                                                access */
} block_flag_t;

/*
   Used to communicate information about memory operations. Not all info is
   complete/correct in all uses. For an example of how a memory hierarchy
   can use a memory transaction, see example memory hierarchy.
   This structure should be considered opaque to the user in its entirety;
   use accessor functions only.

   The transaction parameters are divided into a generic part shown
   below and an architecture specific part. The generic struct is
   included as the first component of the memory_transaction_t struct,
   where the architecture-specific parameters are declared.

   Normally the logical address and most control flags are set before
   the mmu is called. The host_address is a host pointer to the start
   of the simulated memory the transaction loads from or stores to.

   real_address points to the destination for a load or source of a
   store operation.

   The ini_ptr points to the object initiating the operation, this is
   either a processor or a device depending on the ini_type field.
   ini_type_t

   The mmu sets the physical_address and possibly set some more control
   flags. The memory hierarchy only reads the memory transaction to
   decide what stall time to return.

   The block_STC bit functions as a "veto" flag for various
   modules. Any code can set this to 1, but it should not be set to
   zero. When set to one, the next access to the same STC line will be
   passed to the memory hierarchy.

   The may_stall bit is set to zero for a few types of references where
   the simulator kernel cannot deal with stalling. A memory hierarchy must
   return stall time zero when the flag is clear. A memory hierarchy
   that wants to stall anyway should accumulate stall time, and
   trigger the total stall time when a stall-capable access comes
   along (which is not guaranteed to happen but most probably
   will). For atomic read/write
   instructions (swap) any access may stall. If an instruction is stalled
   the entire instruction will be executed after the stalltime is up.
   The may_stall bit may be cleared by the mmu.

   The inverse_endian flag may be changed by the MMU. If set, the
   data involved in the access will be transferred in the opposite order.
   If not set, the bytes will be transferred in the same order they occur in
   the memory space.
   A memory object should not set this bit to a hard value, only toggle it.
   It may have been set by Simics for implementation reasons.

   The page_cross field is used to indicate when Simics has
   split a memory transaction in two.  It is normally 0, but when a
   transaction crosses an MMU page boundary, it will be converted into
   two separate transactions, one for each accessed page.  The
   addresses and sizes are adjusted to confine them to their
   respective pages.  The first of these transaction will have
   page_cross set to 1, and the second will have it set to 2.

   The atomic is read only. It may be read by any module.
   The atomic flag is set for any data memory transaction caused 
   by an atomic instruction; i.e., an instruction whose memory references
   must be performed without any intervening memory references
   (from other processors). The atomic sequence is a read followed
   by a write.

   The use_page_cache is used internally. Do not change its
   value.

   The user_ptr may be used to pass information between user defined
   modules that use memory transactions. Simics does not use this field.

   The type field should not be used directly; use the
   SIM_mem_op_is_xxx()
   predicate functions, where xxx can be
   instruction, data, read, or
   write. SIM_set_mem_op_type() function is used to set
   the type of a memory operation (see the mem_op_type_t enum
   definition), and SIM_get_mem_op_type() is used to read it.

   id is a unique number for all currently outstanding memory
   transactions from a specific initiator.
*/

/* <add id="device api types">
   <name index="true">generic_transaction_t</name>
   <insert id="generic_transaction_t DOC"/>
   </add> */

/* <add id="generic_transaction_t DOC">
   <ndx>generic_transaction_t</ndx>
   <name index="true">generic_transaction_t</name>
   <doc>

   <doc-item name="NAME">generic_transaction_t</doc-item>

   <doc-item name="DESCRIPTION"> A <type>generic_transaction_t</type>
   represents a memory transaction. It should only be accessed via the accessor
   functions documented in
   <nref label="Memory Transactions">Device API Functions, Core, Memory
   Transactions</nref>.</doc-item>

   </doc>
   </add>
*/
typedef struct generic_transaction {
        logical_address_t   logical_address;
        physical_address_t  physical_address;
        unsigned int        size;             /* size in bytes */
        mem_op_type_t       type;             /* opaque */
        unsigned int        atomic:1;         /* trans is part of an atomic
                                                 sequence of mem ops */

        unsigned int        inquiry:1;        /* set to 1 to indicate inquiry
                                                 access */
        unsigned int        non_coherent:1;   /* set to 1 to use 
                                                 direct_memory_update_interface_t.
                                                 update_caching else 
                                                 update_permission */
        unsigned int        _deprecated_ignore:1;
        unsigned int        may_stall:1;      /* when set to 0, any stall time
                                                 returned is ignored */
        unsigned int        reissue:1;        /* If this is 1, the transaction
                                                 will be reissued if a stall
                                                 greater than 0 is returned */
        unsigned int        block_STC:1;      /* blocks stc (compat. only) */
        unsigned int        use_page_cache:1; /* internal - do not change */

        unsigned int        inverse_endian:1; /* data transfer is
                                                 byte-reversed */

        unsigned int        page_cross:2;     /* page crossing
                                                 0 - no crossing
                                                 1 - first access
                                                 2 - second access */

        unsigned int        mem_hier_on_fetch:1; /* set to have fetches sent
                                                    through mem hier */

        unsigned int        block_flags;      /* what we wish to block
                                                 from caching */

        ini_type_t          ini_type;         /* cpu, device, or other */
        conf_object_t      *ini_ptr;

        exception_type_t    exception;        /* set if memhier throws
                                                 exception, otherwise
                                                 Sim_PE_No_Exception */
#if !defined(PYWRAP)
        void               *user_ptr;         /* user pointer that Simics never
                                                 touches */


        uint8              *real_address;     /* data pointer for the
                                                 initiator */

        int                 space_count;      /* internal, # of memory space
                                                 transitions */
#endif
#if !defined(PYWRAP)
        struct transaction *transaction;
#else
        transaction_t      *transaction;      /* associated transaction_t */
#endif
#if !defined(PYWRAP)
        page_t             *reserved;         /* no longer used */
        unsigned int        reserved2;        /* no longer used */
#endif
} generic_transaction_t;

#if !defined(PYWRAP)
EXPORTED void SIM_c_get_mem_op_value_buf(
        const generic_transaction_t *NOTNULL mop, uint8 *NOTNULL dst);
EXPORTED void SIM_c_set_mem_op_value_buf(
        generic_transaction_t *NOTNULL mop, const uint8 *NOTNULL src);
#endif

EXPORTED attr_value_t SIM_get_mem_op_value_buf(
        const generic_transaction_t *NOTNULL mop);
EXPORTED void SIM_set_mem_op_value_buf(
        generic_transaction_t *NOTNULL mop, attr_value_t value);

/*
  <add-fun id="device api memtrans">
  <short>transaction data/instruction predicates</short>
  These functions check whether <tt>mem_op</tt> is a data or an instruction
  transaction. Currently, the only transactions that are instruction
  transactions are instruction fetches.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_data(const generic_transaction_t *NOTNULL mop)
{
        return !(mop->type & Sim_Trn_Instr);
}

/* <append-fun id="SIM_mem_op_is_data"/> */
FORCE_INLINE bool
SIM_mem_op_is_instruction(const generic_transaction_t *NOTNULL mop)
{
        return (mop->type & Sim_Trn_Instr) != 0;
}

/*
  <add-fun id="device api memtrans">
  <short>transaction read/write predicates</short>
  These functions check whether <tt>mem_op</tt> is a read or a write
  transaction.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_read(const generic_transaction_t *NOTNULL mop)
{
        return !(mop->type & Sim_Trn_Write);
}

/* <append-fun id="SIM_mem_op_is_read"/> */
FORCE_INLINE bool
SIM_mem_op_is_write(const generic_transaction_t *NOTNULL mop)
{
        return (mop->type & Sim_Trn_Write) != 0;
}

/*
  <add-fun id="device api memtrans">
  <short>transaction control predicates</short>
  Checks whether <tt>mem_op</tt> is a control transaction (one that does not
  actually transfer any data, such as cache control operations).
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_control(const generic_transaction_t *NOTNULL mop)
{
        return (mop->type & Sim_Trn_Control) != 0;
}

/*
  <add-fun id="device api memtrans">
  <short>transaction control predicates</short>
  Checks whether <tt>mem_op</tt> is prefetch transaction.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_prefetch(const generic_transaction_t *NOTNULL mop)
{
        return (mop->type & Sim_Trn_Prefetch) != 0;
}

/*
  <add-fun id="device api memtrans">
  <short>CPU initiated transaction</short>
  Checks whether <tt>mem_op</tt> is sent from a processor.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_from_cpu(const generic_transaction_t *NOTNULL mop)
{
        return (mop->ini_type & 0xf000) == Sim_Initiator_CPU;
}

/*
  <add-fun id="device api memtrans">
  <short>CPU initiated transaction</short>
  Checks whether <tt>mem_op</tt> is sent from a processor
  of a specific architecture.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_from_cpu_arch(const generic_transaction_t *NOTNULL mop,
                            ini_type_t arch)
{
        return (mop->ini_type & 0xff00) == arch;
}

/*
  <add-fun id="device api memtrans">
  <short>Device initiated transaction</short>
  Checks whether <tt>mem_op</tt> is sent from a device.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_from_device(const generic_transaction_t *NOTNULL mop)
{
        return (mop->ini_type & 0xf000) == Sim_Initiator_Device;
}

/*
  <add-fun id="device api memtrans">
  <short>Cache initiated transaction</short>
  Checks whether <tt>mem_op</tt> is sent from a cache timing model.
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE bool
SIM_mem_op_is_from_cache(const generic_transaction_t *NOTNULL mop)
{
        return (mop->ini_type & 0xf000) == Sim_Initiator_Cache;
}

/*
  <add-fun id="device api memtrans">
  <short>get type of transaction</short>
  This function returns the type of the memory transaction.
  <doc-item name="SEE ALSO">
    <type>generic_transaction_t</type>,
    <fun>SIM_set_mem_op_type</fun>
  </doc-item>
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE mem_op_type_t
SIM_get_mem_op_type(const generic_transaction_t *NOTNULL mop)
{
        return mop->type;
}

/*
  <add-fun id="device api memtrans">
  <short>set type of transaction</short>
  This function sets the type of the memory transaction.
  <doc-item name="SEE ALSO">
    <type>generic_transaction_t</type>,
    <fun>SIM_get_mem_op_type</fun>
  </doc-item>
  <di name="EXECUTION CONTEXT">Cell Context</di>
  </add-fun>
*/
FORCE_INLINE void
SIM_set_mem_op_type(generic_transaction_t *NOTNULL mop, mem_op_type_t type)
{
        mop->type = type;
}

EXPORTED const char *SIM_get_mem_op_type_name(mem_op_type_t type);

EXPORTED uint64 SIM_get_mem_op_value_le(
        const generic_transaction_t *NOTNULL mop);
EXPORTED uint64 SIM_get_mem_op_value_be(
        const generic_transaction_t *NOTNULL mop);
EXPORTED uint64 SIM_get_mem_op_value_cpu(
        const generic_transaction_t *NOTNULL mop);

EXPORTED void SIM_set_mem_op_value_le(generic_transaction_t *NOTNULL mop,
                                      uint64 value);
EXPORTED void SIM_set_mem_op_value_be(generic_transaction_t *NOTNULL mop,
                                      uint64 value);
EXPORTED void SIM_set_mem_op_value_cpu(generic_transaction_t *NOTNULL mop,
                                       uint64 value);

#if defined(DEVICE_IS_BIG_ENDIAN)
#if defined(DEVICE_IS_LITTLE_ENDIAN)
#error "Device cannot be both BIG and LITTLE endian!"
#endif
#define SIM_get_mem_op_value(mop)    SIM_get_mem_op_value_be(mop)
#define SIM_set_mem_op_value(mop, v) SIM_set_mem_op_value_be(mop, v)
#elif defined(DEVICE_IS_LITTLE_ENDIAN)
#define SIM_get_mem_op_value(mop)    SIM_get_mem_op_value_le(mop)
#define SIM_set_mem_op_value(mop, v) SIM_set_mem_op_value_le(mop, v)
#else     /* !DEVICE_IS_LITTLE_ENDIAN */
#define SIM_get_mem_op_value(mop)  UNDEFINED_ENDIAN
#define SIM_set_mem_op_value(mop, v)  UNDEFINED_ENDIAN
#endif    /* !DEVICE_IS_LITTLE_ENDIAN */

/* <add-fun id="device api memtrans">
   <short>get or set transaction address</short>
   Retrieve or set the physical or virtual (logical) addresses of a
   memory transaction.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_physical_address(generic_transaction_t *NOTNULL mop,
                                physical_address_t pa)
{ mop->physical_address = pa; }

/* <append-fun id="SIM_set_mem_op_physical_address"/> */
FORCE_INLINE physical_address_t
SIM_get_mem_op_physical_address(const generic_transaction_t *NOTNULL mop)
{ return mop->physical_address; }

/* <append-fun id="SIM_set_mem_op_physical_address"/> */
FORCE_INLINE void
SIM_set_mem_op_virtual_address(generic_transaction_t *NOTNULL mop,
                               logical_address_t va)
{ mop->logical_address = va; }

/* <append-fun id="SIM_set_mem_op_physical_address"/> */
FORCE_INLINE logical_address_t
SIM_get_mem_op_virtual_address(const generic_transaction_t *NOTNULL mop)
{ return mop->logical_address; }

/* <add-fun id="device api memtrans">
   <short>get transaction size</short>
   Retrieve the size, in bytes, of a memory transaction.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE unsigned
SIM_get_mem_op_size(const generic_transaction_t *NOTNULL mop)
{ return mop->size; }

/* <add-fun id="device api memtrans">
   <short>detect transaction atomicity</short>
   Return true if the transaction was part of an atomic instruction
   (usually a read followed by a write), false otherwise.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE bool
SIM_mem_op_is_atomic(const generic_transaction_t *NOTNULL mop)
{ return mop->atomic; }

/* <add-fun id="device api memtrans">
   <short>get/set transaction inquiry flag</short>
   Retrieve or change the transaction inquiry flag. An inquiry read has no
   side-effects. An inquiry write has no other side-effect than changing the
   bytes at the specified address and size.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_inquiry(generic_transaction_t *NOTNULL mop, bool inquiry)
#ifndef GULP_EXTERN_DECL
  { mop->inquiry = inquiry; }
#else
  /* work-around for SIMICS-8944 */ ;
#endif

/* <append-fun id="SIM_set_mem_op_inquiry"/> */
FORCE_INLINE bool
SIM_get_mem_op_inquiry(const generic_transaction_t *NOTNULL mop)
#ifndef GULP_EXTERN_DECL
  { return mop->inquiry; }
#else
  /* work-around for SIMICS-8944 */ ;
#endif

/* <add-fun id="device api memtrans">
   <short>detect transaction stall possibility</short>
   If true, the simulator will allow the transaction to stall execution.
   When false, a memory hierarchy must not attempt any stalling.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE bool
SIM_mem_op_may_stall(const generic_transaction_t *NOTNULL mop)
{ return mop->may_stall; }

/* <add-fun id="device api memtrans">
   <short>request transaction reissue</short>
   Request that the transaction will be re-issued if a non-zero stall time
   is returned from a memory hierarchy. Otherwise, the memory model
   will not see the transaction again.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_reissue(generic_transaction_t *NOTNULL mop)
{ mop->reissue = 1; }

/* <add-fun id="device api memtrans">
   <short>request transaction visibility</short>
   Request that future accesses from the same virtual address (using a
   granularity given by the <attr>min_cacheline_size</attr> processor
   attribute) will be seen by the memory hierarchy. Otherwise, the simulator
   may cache accesses to this address for performance so that they are not seen
   by the memory model.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_mem_op_ensure_future_visibility(generic_transaction_t *NOTNULL mop)
{ mop->block_flags |= Sim_Block_Memhier; }

/* <add-fun id="device api memtrans">
   <short>detect transaction split</short>
   If a memory transaction was split because it straddled an MMU page,
   return 1 if it is the first part and 2 for the second. If the transaction
   was not split, return zero.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE unsigned
SIM_get_mem_op_page_cross(const generic_transaction_t *NOTNULL mop)
{ return mop->page_cross; }

/* <add-fun id="device api memtrans">
   <short>get/set transaction initiator</short>
   Retrieve or change the transaction initiator type and object.
   These two parameters must agree.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_initiator(generic_transaction_t *NOTNULL mop,
                         ini_type_t type, conf_object_t *obj)
{ mop->ini_type = type; mop->ini_ptr = obj; }

/* <append-fun id="SIM_set_mem_op_initiator"/> */
FORCE_INLINE conf_object_t *
SIM_get_mem_op_initiator(const generic_transaction_t *NOTNULL mop)
{ return mop->ini_ptr; }

/* <append-fun id="SIM_set_mem_op_initiator"/> */
FORCE_INLINE ini_type_t
SIM_get_mem_op_ini_type(const generic_transaction_t *NOTNULL mop)
{ return mop->ini_type; }

/* <add-fun id="device api memtrans">
   <short>get/set transaction exception</short>
   Retrieve or change the transaction exception. If set to a value other
   than <pp>Sim_PE_No_Exception</pp>, the transaction will be interrupted
   and an exception will be taken.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_exception(generic_transaction_t *NOTNULL mop,
                         exception_type_t exc)
{ mop->exception = exc; }

/* <append-fun id="SIM_set_mem_op_exception"/> */
FORCE_INLINE exception_type_t
SIM_get_mem_op_exception(const generic_transaction_t *NOTNULL mop)
{ return mop->exception; }

#ifndef PYWRAP
/* <add-fun id="device api memtrans">
   <short>get/set transaction user data</short>
   Retrieve or change user data associated with the transaction.
   This data is not touched by Simics in any way and its handling and
   interpretation is left to the user. It can be used
   to pass information from a timing model to a snoop device, but the data
   does not survive across a stall.
   <di name="EXECUTION CONTEXT">Cell Context</di>
   </add-fun> */
FORCE_INLINE void
SIM_set_mem_op_user_data(generic_transaction_t *NOTNULL mop,
                         void *data)
{ mop->user_ptr = data; }

/* <append-fun id="SIM_set_mem_op_user_data"/> */
FORCE_INLINE void *
SIM_get_mem_op_user_data(const generic_transaction_t *NOTNULL mop)
{ return mop->user_ptr; }
#endif

#ifndef PYWRAP
EXPORTED generic_transaction_t SIM_make_mem_op_write(
        physical_address_t addr, bytes_t data,
        bool inquiry, conf_object_t *initiator);
EXPORTED generic_transaction_t SIM_make_mem_op_read(
        physical_address_t addr, buffer_t data,
        bool inquiry, conf_object_t *initiator);
#endif

struct arm_memory_transaction;
struct mips_memory_transaction;
struct pci_memory_transaction;
struct ppc_memory_transaction;
struct x86_memory_transaction;

#if defined(__cplusplus)
}
#endif

#endif  /* ! TURBO_SIMICS */

#endif
