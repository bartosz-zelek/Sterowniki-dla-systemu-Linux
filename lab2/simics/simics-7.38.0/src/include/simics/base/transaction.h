/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_BASE_TRANSACTION_H
#define SIMICS_BASE_TRANSACTION_H

#include <simics/base/breakpoints.h>
#include <simics/base/types.h>
#include <simics/base/map-target.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="device api types">
   <name index="true">translation_t</name>
   <insert id="translation_t DOC"/>
   </add> */

/* <add id="translation_t DOC">
   <ndx>translation_flags_t</ndx>
   <ndx>translation_t</ndx>
   <name index="true">translation_t</name>

   <doc>
   <doc-item name="NAME">translation_t</doc-item>
   <doc-item name="DESCRIPTION">
   <insert id="translation_t description"/>
   </doc-item>
   </doc>
   </add>

   <add id="translation_t description">

   The <type>translation_t</type> type is used for the implementation
   of the <iface>translator</iface> and <iface>transaction_translator</iface>
   interfaces. It describes the range
   for which the translation is valid, its target as well as
   translation properties.

   The range for which the translation is valid is specified by the
   fields <param>base</param> and <param>size</param>. As a special case,
   if <param>size</param> and <param>base</param> are both 0, then the
   translation is valid for the entire address space. To allow optimizations
   (e.g., caching of translations) translators should return as wide ranges
   as possible.

   The <param>target</param> field specifies the object and interface
   port which is mapped into the address range in the form of a map target.
   Map targets can be created using the function
   <fun>SIM_new_map_target</fun>. Please note that the ownership over
   the returned map target is not transferred to the interface caller.
   This means that to avoid memory leaks the reference to the map
   target must be kept by the implementing object, and
   <fun>SIM_free_map_target</fun> function should be later used to
   deallocate the map target. Possible map targets include IO
   banks, RAM, ROM, memory spaces, port spaces, bridges, and translators.
   The <param>base</param> address in the source address space is
   mapped to the target address returned in the <param>start</param> field.

   A null value returned in the <param>target</param> field signifies that
   the translation cannot be done. This can happen if there is nothing mapped
   in the range defined by <param>base</param> and <param>size</param>
   (transactions directed to this region will be terminated
   with the pseudo exception <const>Sim_PE_IO_Not_Taken</const>) or
   if a translation valid for all requested accesses cannot be performed.
   In the latter case, the requestor is expected to repeat
   the interface call with just a single bit set in the access mask,
   e.g. <const>Sim_Access_Read</const>.

   If the returned translation is not static but instead depends on e.g.
   a device register, then the translator can set the flags field
   to <const>Sim_Translation_Dynamic</const>. This flag indicates that
   the translation must not be cached. If this flag is not used, then it
   is the responsibility of the translator to call
   either <fun>SIM_map_target_flush</fun> (preferably)
   or <fun>SIM_translation_changed</fun> function when a previously performed
   translation is no longer valid.

   The <const>Sim_Translation_Ambiguous</const> flag should not generally
   be used by models. It is used by Simics objects of
   the <class>memory-space</class> class to indicate an error in
   the memory mapping when several destinations are specified for the address.

   <insert-until text="// ADD TYPE translation"/>

   <doc-item name="SEE ALSO">
     <fun>SIM_map_target_flush</fun>,
     <fun>SIM_translation_changed</fun>
   </doc-item>

   </add>
*/
typedef enum {
        Sim_Translation_Dynamic = 1,
        Sim_Translation_Ambiguous = 2
} translation_flags_t;

typedef struct translation {
        const map_target_t *target;  /* target of translation */

        physical_address_t base;     /* base address of translated range */
        physical_address_t start;    /* start address in mapped object */
        physical_address_t size;     /* size of translated range */

        translation_flags_t flags;
} translation_t;
// ADD TYPE translation

typedef enum {
        Sim_Transaction_Target_Unset            = 0,
        Sim_Transaction_Target_Ram              = 1 << 0,
        Sim_Transaction_Target_Device           = 1 << 1,
        Sim_Transaction_Target_Uncacheable        = 1 << 2,
        Sim_Transaction_Target_Cacheable_Ram   = Sim_Transaction_Target_Ram,
        Sim_Transaction_Target_Cacheable_Device = Sim_Transaction_Target_Device,
        Sim_Transaction_Target_Uncacheable_Ram    =
                Sim_Transaction_Target_Ram | Sim_Transaction_Target_Uncacheable,
        Sim_Transaction_Target_Uncacheable_Device =
                Sim_Transaction_Target_Device | Sim_Transaction_Target_Uncacheable,
} transaction_target_type_t;

typedef struct {
        transaction_target_type_t val;
} transaction_target_type_ret_t;

/* <add id="device api transaction types">
     <name index="true">transaction_flags_t</name>
     <insert id="transaction_flags_t DOC"/></add>

   <add id="device api transaction types">
     <name index="true">atom_id_t</name>
     <insert id="atom_id_t DOC"/></add>

   <add id="device api transaction types">
     <name index="true">atom_t</name>
     <insert id="atom_t DOC"/></add>

   <add id="device api transaction types">
     <name index="true">transaction_t</name>
     <insert id="transaction_t DOC"/></add>

   <add id="device api transaction types">
     <name index="true">transaction_completion_t</name>
     <insert id="transaction_completion_t DOC"/></add>
*/

#define ATOM_TYPE_list_end              int
#define ATOM_TYPE_completion            transaction_completion_t
#define ATOM_TYPE_data                  uint8 *
#define ATOM_TYPE_size                  uint32
#define ATOM_TYPE_user_data             lang_void *
#define ATOM_TYPE_memop                 generic_transaction_t *
#define ATOM_TYPE_bytemap               transaction_bytemap_t *
#define ATOM_TYPE_fill_value            uint8
#define ATOM_TYPE_initiator             conf_object_t *
#define ATOM_TYPE_owner                 conf_object_t *
#define ATOM_TYPE_trace_atom_access     transaction_trace_atom_access_t *
#define ATOM_TYPE_target_type           transaction_target_type_ret_t *
#ifdef PYWRAP
#define ATOM_TYPE_flags                 uint32
#else
#define ATOM_TYPE_flags                 transaction_flags_t
#endif

/* <add id="atom_id_t DOC">
   <ndx>atom_id_t</ndx>
   <name index="true">atom_id_t</name>
   <doc>

   <doc-item name="NAME">atom_id_t</doc-item>

   <doc-item name="DESCRIPTION"> Each atom type is associated
   with a unique id, the <type>atom_id_t</type>. Most atoms types are
   pre-defined by Simics Core and have static ids, but there are
   also dynamically assigned ids which are used for custom atom types.

   Atom ids are internal to Simics Core and should never be used explicitly
   by a Simics models. Instead, there are API functions like
   e.g. <fun>ATOM_size</fun> or <fun>ATOM_initiator</fun> which should be
   used instead.
   </doc-item>
   </doc></add>
*/
typedef enum {
        Sim_Atom_Id_illegal = 0,
        Sim_Atom_Id_list_end,
        Sim_Atom_Id_flags,
        Sim_Atom_Id_data,
        Sim_Atom_Id_size,
        Sim_Atom_Id_fill_value,
        Sim_Atom_Id_initiator,
        Sim_Atom_Id_user_data,
        Sim_Atom_Id_completion,
        Sim_Atom_Id_owner,
        Sim_Atom_Id_memop,
        Sim_Atom_Id_bytemap,
        Sim_Atom_Id_trace_atom_access,
        Sim_Atom_Id_target_type,
        Sim_Atom_Id_pcie_type = 0x100,
        Sim_Atom_Id_pcie_requester_id,
        Sim_Atom_Id_pcie_msg_type,
        Sim_Atom_Id_pcie_ecs,
        Sim_Atom_Id_pcie_msg_route,
        Sim_Atom_Id_pcie_pasid,
        Sim_Atom_Id_pcie_at,
        Sim_Atom_Id_pcie_error_ret,
        Sim_Atom_Id_pcie_byte_count_ret,
        Sim_Atom_Id_pcie_device_id,
        Sim_Atom_Id_pcie_destination_segment,
        Sim_Atom_Id_pcie_link_negotiation,
        Sim_Atom_Id_pcie_ats_translation_request_cxl_src,
        Sim_Atom_Id_pcie_ats_translation_request_no_write,
        Sim_Atom_Id_pcie_ats_invalidate_request_itag,
        Sim_Atom_Id_pcie_ats_invalidate_completion_itag_vector,
        Sim_Atom_Id_pcie_ats_invalidate_completion_count,
        Sim_Atom_Id_pcie_prs_page_request,
        Sim_Atom_Id_pcie_prs_page_group_response,
        Sim_Atom_Id_pcie_prs_stop_marker,
        Sim_Atom_Id_pcie_requester_segment,
        Sim_Atom_Id_pcie_ide_secured,
        Sim_Atom_Id_arm_privileged = 0x200,
        Sim_Atom_Id_arm_nonsecure,
        Sim_Atom_Id_arm_memory_attributes,
        Sim_Atom_Id_arm_smmu_attributes,
        Sim_Atom_Id_arm_nsaid,
        Sim_Atom_Id_arm_device_id,
        Sim_Atom_Id_riscv_wg_wid = 0x300,
        Sim_Atom_Id_riscv_device_id,
        Sim_Atom_Id_riscv_process_id,
        Sim_Atom_Id_riscv_io_error_ret,
        Sim_Atom_Id_cxl_type = 0x400,
} atom_id_t;

#define ATOM_COMMON_(type)                                              \
        static inline atom_t                                            \
        pywrap_atom_from_##type(ATOM_TYPE_##type val) {                 \
                atom_t a = { (atom_id_t)0, (void *)(uintptr_t)val };    \
                return a; }                                             \
        static inline ATOM_TYPE_##type                                  \
        pywrap_##type##_from_atom(atom_t *NOTNULL a)  {                 \
                return (ATOM_TYPE_##type)(uintptr_t)a->ptr; }           \
        struct dummy_semicolon_eater_##type { int dummy; }

#ifndef PYWRAP
#define SIM_ATOM(type)                                                  \
        static inline atom_t ATOM_##type(ATOM_TYPE_##type val) {        \
                atom_t a = { Sim_Atom_Id_##type,                        \
                              (void *)(uintptr_t)val };                 \
                return a; }                                             \
        ATOM_COMMON_(type)

#define SIM_ACCESSIBLE_ATOM(type)                                       \
        static inline ATOM_TYPE_##type ATOM_get_transaction_##type(     \
                                  const transaction_t *NOTNULL t) {     \
                return (ATOM_TYPE_##type)(uintptr_t)                    \
                        VT_transaction_atom(t, Sim_Atom_Id_##type); }   \
        static inline const ATOM_TYPE_##type * ATOM_transaction_##type(       \
                const transaction_t *NOTNULL t) {                             \
                const atom_t *a = VT_lookup_transaction_atom(                 \
                        t, Sim_Atom_Id_##type);                               \
                return a ? (const ATOM_TYPE_##type *)(&(a->ptr)) : NULL; }    \
        SIM_ATOM(type)                                                  \

#define LOOKUP_ATOM_ID_ONCE_(typestr)                                   \
        static atom_id_t atom_id;                                       \
        if (unlikely(atom_id == 0))                                     \
                atom_id = VT_get_atom_class_id(typestr)

#define SIM_CUSTOM_ATOM(type)                                           \
        static inline atom_t ATOM_##type(ATOM_TYPE_##type val) {        \
                LOOKUP_ATOM_ID_ONCE_(#type);                            \
                atom_t a = { atom_id, (void *)(uintptr_t)val };         \
                return a; }                                             \
        static inline ATOM_TYPE_##type ATOM_get_transaction_##type(     \
                                  const transaction_t *NOTNULL t) {     \
                LOOKUP_ATOM_ID_ONCE_(#type);                            \
                return (ATOM_TYPE_##type)(uintptr_t)                    \
                        VT_transaction_atom(t, atom_id); }              \
        static inline const ATOM_TYPE_##type * ATOM_transaction_##type(       \
                const transaction_t *NOTNULL t) {                             \
                LOOKUP_ATOM_ID_ONCE_(#type);                                  \
                const atom_t *a = VT_lookup_transaction_atom(t, atom_id);     \
                return a ? (const ATOM_TYPE_##type *)(&(a->ptr)) : NULL; }    \
        static inline void ATOM_register_##type(void) {                 \
                VT_register_atom_class(#type,                           \
                                       "pywrap_atom_from_" #type,       \
                                       "pywrap_" #type "_from_atom"); } \
        ATOM_COMMON_(type)

#else
#define SIM_CUSTOM_ATOM(type)     ATOM_COMMON_(type)
#define SIM_ACCESSIBLE_ATOM(type) ATOM_COMMON_(type)
#define SIM_ATOM(type)            ATOM_COMMON_(type)
#endif


/* <add id="atom_t DOC">
   <ndx>atom_t</ndx>
   <name index="true">atom_t</name>
   <doc>

   <doc-item name="NAME">atom_t</doc-item>

   <doc-item name="DESCRIPTION"> The <type>atom_t</type> type is a container
   type for tagged data associated with a transaction. The kind of data
   stored in the atom is determined by the <var>id</var> field, and a pointer
   to the data or the data itself is stored in the <var>ptr</var> field.

   Atoms should always be initialized using provided constructor functions
   like <fun>ATOM_flags</fun> or <fun>ATOM_size</fun>. Usage of
   the constructors ensures that the data payload is of the correct type
   and that the <fun>id</fun> is set to the correct value.

   Atom lists must be terminated with the special <const>ATOM_LIST_END</const>
   marker.
   </doc-item>
   </doc></add>
*/
typedef struct {
        atom_id_t id;
#ifndef PYWRAP
        void *ptr;
#endif
} atom_t;

#define ATOM_LIST_END           ATOM_list_end(0)


/* <add id="transaction_t DOC">
   <ndx>transaction_t</ndx>
   <name index="true">transaction_t</name>
   <doc>

   <doc-item name="NAME">transaction_t</doc-item>

   <doc-item name="DESCRIPTION"> A <type>transaction_t</type>
   represents a memory transaction. The properties of the
   transaction is stored in the form of an atom list, where each
   atom describes a particular aspect of the transaction, like the
   size of the transaction.

   The field <var>atoms</var> points to the atoms list,
   which must be terminated with the constant <const>ATOM_LIST_END</const>.

   The <var>prev</var> field points to an optional parent transaction.
   If a particular atom is not found in the atoms list, then the
   parent's list of atoms is consulted instead. The <var>prev</var>
   pointer is also used when a chained transaction is monitored
   with <fun>SIM_monitor_chained_transaction</fun>.

   Besides the fields above, the transaction contains some internal
   fields that should be initialized to 0. The internal fields should
   not be referenced explicitly since they are likely to change
   in future Simics releases.

   For details, please refer to "Transactions" chapter in
   the Model Builder's User Guide.

   </doc-item>
   </doc></add>
*/
typedef struct transaction {
#ifndef PYWRAP
        atom_t *atoms;
#endif
        struct transaction *prev;
#ifndef PYWRAP
        struct transaction_cache *_cache;
        uint64 _status;
#endif
} transaction_t;

/* <add id="transaction_completion_t DOC">
   <ndx>transaction_completion_t</ndx>
   <name index="true">transaction_completion_t</name>
   <doc>
   <doc-item name="NAME">transaction_completion_t</doc-item>
   <doc-item name="SYNOPSIS">
        <insert id="transaction_completion_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   Callback invoked when an asynchronous transaction is completed.
   The callback is stored in a <type>completion</type> atom belonging
   to the transaction <arg>t</arg>. Similarly, <arg>obj</arg> is an
   object stored in either an <type>owner</type> atom or an
   <type>initiator</type> atom. The former takes precedence if both are
   present.

   Completion callbacks are only invoked for transactions monitored
   with either <fun>SIM_monitor_transaction</fun> or
   <fun>SIM_monitor_chained_transaction</fun>, or for transactions
   deferred with <fun>SIM_defer_owned_transaction</fun>.

   The completion status for the operation is given in the
   <param>ex</param> argument, and is usually equal to
   <const>Sim_PE_No_Exception</const>.

   The return value of the callback is the completion status
   for the transaction <arg>t</arg>. This status is used to complete
   the parent transaction if the transaction is being monitored with
   <fun>SIM_monitor_chained_transaction</fun>. The return value is
   also returned by <fun>SIM_monitor_transaction</fun> or
   <fun>SIM_monitor_chained_transaction</fun> when a transaction is
   completed synchronously.

   If the callback returns <const>Sim_PE_Deferred</const>, then
   the transaction <arg>t</arg> is left uncompleted. It must then
   be completed later on by an explicit call to
   <fun>SIM_complete_transaction</fun>.

   </doc-item></doc></add>

   <add-type id="transaction_completion_t def"></add-type>  */
typedef exception_type_t (*transaction_completion_t)(
        conf_object_t *obj, transaction_t *t, exception_type_t ex);

typedef struct transaction_bytemap transaction_bytemap_t;

/* <add id="transaction_flags_t DOC">
   <ndx>transaction_flags_t</ndx>
   <name index="true">transaction_flags_t</name>
   <doc>

   <doc-item name="NAME">transaction_flags_t</doc-item>
   <doc-item name="SYNOPSIS">
        <insert id="transaction_flags_t def"/></doc-item>
   <doc-item name="DESCRIPTION">
   The <type>transaction_flags_t</type> type is bitmask
   used to specify the transaction type. It is a combination
   of the following bits:

   <tt>Sim_Transaction_Fetch</tt> indicates that the transaction is
   an instruction fetch.

   <tt>Sim_Transaction_Write</tt> is set if the transaction is a write.

   <tt>Sim_Transaction_Control</tt> is set if the transaction does not
   actually transfer any data. One example of such transactions is
   cache control operations.

   The <tt>Sim_Transaction_Inquiry</tt> bit signifies that side
   effects normally triggered by the transaction should be suppressed.
   Examples of side effects include triggering breakpoints and
   clearing "read-to-clear" device registers.

   When neither <tt>Sim_Transaction_Fetch</tt>
   nor <tt>Sim_Transaction_Write</tt> is set the transaction is
   a read transaction.

   </doc-item></doc></add>
   <add-type id="transaction_flags_t def"></add-type>  */
typedef enum {
        Sim_Transaction_Fetch         = 1 << 0,
        Sim_Transaction_Write         = 1 << 1,
        Sim_Transaction_Control       = 1 << 2,

        Sim_Transaction_Inquiry       = 1 << 8,
        Sim_Transaction_Incoherent    = 1 << 9,
        Sim_Transaction_Atomic        = 1 << 10,
} transaction_flags_t;


/* Accessors. */
EXPORTED bool SIM_transaction_is_fetch(
        const transaction_t *NOTNULL t);
EXPORTED bool SIM_transaction_is_write(
        const transaction_t *NOTNULL t);
EXPORTED bool SIM_transaction_is_read(
        const transaction_t *NOTNULL t);
EXPORTED bool SIM_transaction_is_inquiry(
        const transaction_t *NOTNULL t);
EXPORTED bool SIM_transaction_is_deferrable(
        const transaction_t *NOTNULL t);
EXPORTED unsigned SIM_transaction_size(
        const transaction_t *NOTNULL t);
EXPORTED conf_object_t *SIM_transaction_initiator(
        const transaction_t *NOTNULL t);
EXPORTED transaction_flags_t SIM_transaction_flags(
        const transaction_t *NOTNULL t);

/* Transaction completion. */
EXPORTED void SIM_complete_transaction(
        transaction_t *NOTNULL t,
        exception_type_t ex);
EXPORTED transaction_t *SIM_defer_transaction(
        conf_object_t *obj,
        transaction_t *t);
EXPORTED transaction_t *SIM_defer_owned_transaction(
        transaction_t *NOTNULL t);
EXPORTED void SIM_replace_transaction(
        transaction_t *NOTNULL t_old,
        transaction_t *t_new);

EXPORTED exception_type_t SIM_monitor_transaction(
        transaction_t *NOTNULL t,
        exception_type_t ex);
EXPORTED exception_type_t SIM_monitor_chained_transaction(
        transaction_t *NOTNULL t,
        exception_type_t ex);
EXPORTED exception_type_t SIM_transaction_wait(
        transaction_t *NOTNULL t,
        exception_type_t ex);
EXPORTED exception_type_t SIM_poll_transaction(
        transaction_t *NOTNULL t);


/* Transaction data transfer. */
EXPORTED void SIM_set_transaction_bytes(
        const transaction_t *NOTNULL t,
        bytes_t bytes);
EXPORTED void SIM_set_transaction_value_le(
        const transaction_t *NOTNULL t,
        uint64 val);
EXPORTED void SIM_set_transaction_value_be(
        const transaction_t *NOTNULL t,
        uint64 val);
EXPORTED void SIM_set_transaction_bytes_offs(
        const transaction_t *NOTNULL t,
        unsigned offs,
        bytes_t bytes);
EXPORTED void SIM_set_transaction_bytes_constant(
        const transaction_t *NOTNULL t,
        uint8 value);
EXPORTED void SIM_get_transaction_bytes(
        const transaction_t *NOTNULL t,
        buffer_t buf);
EXPORTED void SIM_get_transaction_bytes_offs(
        const transaction_t *NOTNULL t,
        unsigned offs,
        buffer_t buf,
        bool zerofill_holes);

EXPORTED uint64 SIM_get_transaction_value_le(
        const transaction_t *NOTNULL t);
EXPORTED uint64 SIM_get_transaction_value_be(
        const transaction_t *NOTNULL t);


/* Transaction Issue. */
EXPORTED exception_type_t SIM_issue_transaction(
        const map_target_t *NOTNULL mt,
        transaction_t *NOTNULL t,
        uint64 addr);

/* This declaration fits better into <simics/base/memory.h>. We had to put it
   here since our headers intermix type definitions and func declarations. */
EXPORTED bool SIM_inspect_address_routing(
        const map_target_t *NOTNULL mt,
        transaction_t *NOTNULL t,
        uint64 addr,
        bool (*NOTNULL callback)(
                const map_target_t *mt,
                const transaction_t *t,
                uint64 addr,
                uint64 base,
                uint64 start,
                uint64 size,
                access_t access,
                translation_flags_t flags,
                lang_void *data),
        lang_void *data);

EXPORTED bool SIM_inspect_breakpoints(
        const map_target_t *NOTNULL mt,
        transaction_t *NOTNULL t,
        uint64 start,
        uint64 end,
        bool (*NOTNULL callback)(
                conf_object_t *trigger_object,
                breakpoint_set_t bp_set,
                const transaction_t *t,
                uint64 addr,
                uint64 size,
                lang_void *data),
        lang_void *data);

/* Transaction Checkpointing. */
EXPORTED int64 SIM_get_transaction_id(
        transaction_t *t);
EXPORTED void SIM_reconnect_transaction(
        transaction_t *t, int64 id);
EXPORTED transaction_t *VT_get_transaction(
        int64 id);

/* Atom handling. */
EXPORTED void VT_register_atom_class(
        const char *NOTNULL name,
        const char *pywrap_atom_from_type,
        const char *pywrap_type_from_atom);

EXPORTED atom_id_t VT_get_atom_class_id(
        const char *NOTNULL name);
EXPORTED atom_id_t VT_lookup_atom_class_id(
        const char *NOTNULL name);

EXPORTED attr_value_t VT_list_registered_atoms(void);

#ifndef PYWRAP
EXPORTED void *VT_transaction_atom(
        const transaction_t *NOTNULL t,
        atom_id_t id);
#endif

EXPORTED const atom_t *VT_lookup_transaction_atom(
        const transaction_t *NOTNULL t,
        atom_id_t id);

EXPORTED void SIM_register_python_atom_type(
        const char *NOTNULL name);

SIM_ATOM(completion);
SIM_ATOM(flags);
SIM_ATOM(user_data);
SIM_ACCESSIBLE_ATOM(memop);
SIM_ATOM(fill_value);
SIM_ATOM(initiator);
SIM_ATOM(owner);
#ifndef PYWRAP
SIM_ATOM(list_end);
SIM_ATOM(data);
SIM_ATOM(size);
SIM_ATOM(bytemap);
#endif

typedef struct transaction_trace_atom_access {
        void (*callback)(
                const transaction_t *t, atom_id_t atom_id, lang_void *data);
        lang_void *cb_data;
#ifndef PYWRAP
        uint64 _internal;  /* field used by Simics */
#endif
} transaction_trace_atom_access_t;

SIM_ATOM(trace_atom_access);
SIM_ACCESSIBLE_ATOM(target_type);

#ifdef DOC
/* Below we document a few functions defined above. Our documentation
   system doesn't work well for functions defined via macros. We use
   the "#ifdef DOC" hack to handle this. */

/* <add-fun id="device api transaction">
   <short>transaction atom's constructor</short>

   The functions construct transaction atoms.

   <fun>ATOM_flags</fun> returns a transaction atom specifying transaction
   flags (see description of <type>transaction_flags_t</type> for information
   about available transaction flags).

   <fun>ATOM_data</fun> returns a transaction atom that holds the pointer
   to a buffer that is used to get the data from (for write transactions)
   to store the data to (for read and instruction fetch transactions).

   <fun>ATOM_size</fun> returns a transaction atom that holds
   the size of a transaction.

   <fun>ATOM_initiator</fun> returns a transaction atom that holds
   the initiator of a transaction.

   <fun>ATOM_completion</fun> creates a completion atom - a special atom
   that holds a callback that is invoked when a transaction is completed
   asynchronously.

   <fun>ATOM_list_end</fun> returns a special atom that should end the list
   of transaction atoms. One can use the <pp>ATOM_LIST_END</pp> macro instead.

   <di name="RETURN VALUE">An atom value</di>
   <di name="EXECUTION CONTEXT">All contexts (including Threaded Context)</di>
   <di name="EXAMPLE">Sample C code to create a 1-byte read transaction:
   <small>
   <pre>
     uint8 val;
     atom_t atoms[] = {
         // the flags atom value specifies the transaction type:
         // - 0 defines a read transaction
         // - Sim_Transaction_Write - a write transaction
         // - Sim_Transaction_Fetch - an instruction fetch transaction
         ATOM_flags(0),

         ATOM_data(&amp;val),
         ATOM_size(sizeof val),
         ATOM_initiator(obj),
         ATOM_LIST_END
     };
     transaction_t t = { atoms };
   </pre>
   </small>
   </di>

   <di name="SEE ALSO">
       <type>transaction_t</type>, <type>transaction_flags_t</type>,
       <type>transaction_completion_t</type>
   </di>

   </add-fun> */
static inline atom_t ATOM_flags(transaction_flags_t val);

/* <append-fun id="ATOM_flags"/> */
static inline atom_t ATOM_data(uint8 *val);

/* <append-fun id="ATOM_flags"/> */
static inline atom_t ATOM_size(uint32 val);

/* <append-fun id="ATOM_flags"/> */
static inline atom_t ATOM_initiator(conf_object_t *val);

/* <append-fun id="ATOM_flags"/> */
static inline atom_t ATOM_completion(transaction_completion_t val);

/* <append-fun id="ATOM_flags"/> */
static inline atom_t ATOM_list_end(int val);

/* ATOM_bytemap, ATOM_fill_value, ATOM_memop, ATOM_owner, ATOM_user_data are not
   documented for now. We may document (at least, e.g., ATOM_fill_value which
   can be quite useful) in future. */

#endif  /* DOC */

/* We deliberately provide SIM_issue_{read,write}_transaction helper function
   only in C. In Python, SIM_issue_transaction and transaction_t Python-wrapper
   are really convenient; adding more functions can just confuse users
   about what functions to use. */
#if !defined(PYWRAP)
/* <add-fun id="device api transaction">
   <short>functions for issuing <type>transaction_t</type> transactions</short>

   These functions provide a simple way to issue read and write
   <type>transaction_t</type> transactions in C code. In Python, please use
   the <fun>SIM_issue_transaction</fun> function.

   The functions have the following parameters:<br/>

   - <param>mt</param> specifies the destination to send a transaction to.<br/>

   - <param>addr</param> is the address used to issue a transaction.<br/>

   - the <fun>SIM_issue_read_transaction</fun>'s <param>buf</param> parameter
     has <type>buffer_t</type> type and specifies a writable buffer for storing
     the read data. The <param>data.data</param> field points to the host memory
     where the results should be stored. The <param>data.len</param> field
     specifies the number of bytes to read.<br/>

   - The <fun>SIM_issue_write_transaction</fun>'s <param>bytes</param> parameter
     is of <type>bytes_t</type> type and provides the data that should
     be written. The <param>data.data</param> field points to the buffer
     holding the data that should be written. The <param>data.len</param> field
     specifies the number of bytes to write.<br/>

   - <param>inquiry</param> specifies whether the transaction should have
     the <tt>Sim_Transaction_Inquiry</tt> flag set. The flag signifies that
     side effects normally triggered by the transaction should be suppressed.
     Examples of side effects include triggering breakpoints and clearing
     "read-to-clear" device registers.<br/>

   - <param>initiator</param> specifies the object issuing a transaction. It
     may be <const>NULL</const>. The value is used for setting the
     initiator atom of the transaction.<br/>

   These functions don't support the case when a model needs to provide custom
   atoms in <type>transaction_t</type> transactions. For such cases as well as
   when using Python the <fun>SIM_issue_transaction</fun> function should be
   used.

   <di name="RETURN VALUE">Status of the issued transaction, usually
     <const>Sim_PE_No_Exception</const> or <const>Sim_PE_IO_Not_Taken</const>.
     See the <type>exception_type</type>'s documentation for more information.
   </di>
   <di name="EXECUTION CONTEXT">Cell Context</di>
   <di name="EXAMPLE">
     <insert id="example-code-SIM_issue_read_transaction"/>
     <insert id="example-code-SIM_issue_write_transaction"/>
   </di>

   <di name="SEE ALSO">
       <type>exception_type_t</type>, <type>map_target_t</type>,
       <fun>SIM_issue_transaction</fun>, <type>transaction_t</type>
   </di>

   </add-fun> */
FORCE_INLINE exception_type_t
SIM_issue_read_transaction(
        map_target_t *NOTNULL mt, uint64 addr, buffer_t buf,
        bool inquiry, conf_object_t *initiator)
#ifndef GULP_EXTERN_DECL  /* work-around for SIMICS-8944 */
{
        atom_t atoms[5] = {
                ATOM_flags(inquiry
                           ? Sim_Transaction_Inquiry : (transaction_flags_t)0),
                ATOM_data(buf.data),
                /* NB: ATOM_size takes uint32 but buf.len is size_t. Let's hope
                   that no one sends very large transactions: */
                ATOM_size((uint32)buf.len),
                ATOM_initiator(initiator),
                ATOM_LIST_END,
        };
        /* Some tricks to make code valid and free from GCC's warnings: */
        transaction_t t =
#if defined(__cplusplus)
                {};  /* supported in C only since C23 (and as GCC extension) */
#else
                { 0 };
#endif
        t.atoms = atoms;
        return SIM_issue_transaction(mt, &t, addr);
}
#else
;
#endif  /* GULP_EXTERN_DECL */

/* <append-fun id="SIM_issue_read_transaction"/> */
FORCE_INLINE exception_type_t
SIM_issue_write_transaction(
        map_target_t *NOTNULL mt, uint64 addr, bytes_t bytes,
        bool inquiry, conf_object_t *initiator)
#ifndef GULP_EXTERN_DECL  /* work-around for SIMICS-8944 */
{
        atom_t atoms[5] = {
                ATOM_flags((transaction_flags_t)(
                                   Sim_Transaction_Write
                                   | (inquiry
                                      ? Sim_Transaction_Inquiry : 0))),
                ATOM_data((uint8 *)bytes.data),
                /* NB: ATOM_size takes uint32 but buf.len is size_t. Let's hope
                   that no one sends very large transactions: */
                ATOM_size((uint32)bytes.len),
                ATOM_initiator(initiator),
                ATOM_LIST_END,
        };
        /* Some tricks to make code valid and free from GCC's warnings: */
        transaction_t t =
#if defined(__cplusplus)
                {};  /* supported in C only since C23 (and as GCC extension) */
#else
                { 0 };
#endif
        t.atoms = atoms;
        return SIM_issue_transaction(mt, &t, addr);
}
#else
;
#endif  /* GULP_EXTERN_DECL */

#endif  /* PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
