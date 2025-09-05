/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_CPU_INSTRUMENTATION_H
#define SIMICS_MODEL_IFACE_CPU_INSTRUMENTATION_H

#include <simics/pywrap.h>
#include <simics/base/conf-object.h>
#include <simics/base/memory-transaction.h>
#include <simics/base/types.h>
#include <simics/processor/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct cpu_instrumentation_info instruction_handle_t;
typedef struct cpu_memory_info memory_handle_t;
typedef struct cpu_replace_info decoder_handle_t;
typedef struct cpu_exception_info exception_handle_t;
typedef struct cpu_exception_return_info exception_return_handle_t;
typedef struct cpu_address_info address_handle_t;
typedef struct cached_instruction_data cached_instruction_handle_t;
typedef struct instrumentation_entry _cpu_instrumentation_entry_t;
typedef _cpu_instrumentation_entry_t cpu_cb_handle_t;
typedef _cpu_instrumentation_entry_t cpu_stream_handle_t;

/* <add id="cpu_bytes_t DOC"> <ndx>cpu_bytes_t</ndx>
   <name index="true">cpu_bytes_t</name>
   <doc> 
   <doc-item name="NAME">cpu_bytes_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_bytes_t"/></doc-item>
   <doc-item name="DESCRIPTION"> 

   An immutable sequence of bytes. The ownership is not transferred to the
   callee/caller when called or returned.
   The corresponding Python type is a string of bytes. May only be used in a
   defined scope which should be documented in each use case.
 </doc-item> 
  </doc>
  </add>
*/

/* <add-type id="cpu_bytes_t"></add-type> */
typedef struct cpu_bytes {
        size_t size;
#ifndef PYWRAP
        const uint8 *data;
#endif
} cpu_bytes_t;

/* <add id="cpu_instruction_query_interface_t"> 

   The <iface>cpu_instruction_query</iface> interface can be used in functions
   of the following types:

   <ul>
   <li><type>cpu_instruction_cb_t</type></li>
   <li><type>cpu_cached_instruction_cb_t</type></li>
   <li><type>cpu_instruction_decoder_cb_t</type></li>
   </ul>

   where an <type>instruction_handle_t</type> pointer is available. The
   handle is only valid in the callback. The interface is used to request
   information about the instruction being executed by the processor
   <arg>cpu</arg>.

   <insert-until text="// ADD INTERFACE cpu_instruction_query"/>

   The <fun>logical_address</fun> and <fun>physical_address</fun> is used to
   get the different addresses of the instruction being executed.

   Note that if the instruction crosses a page boundary the last part of the
   instruction will have a different mapping for the physical address than
   returned by <fun>physical_address</fun>. To get hold of the physical address
   on the second page, use the <fun>logical_to_physical</fun> method of the
   <iface>processor_info(_v2)</iface> and provide the logical address of the
   first byte on the second page.

   The <fun>get_instruction_bytes</fun> method is used to read the instruction
   bytes. The returned data is of a cpu_bytes_t type that contains the data and
   the size. The data member is only available during the execution of the
   callback. The data cannot be changed. Corresponding type in Python is a
   string of bytes.

   <insert id="cpu_bytes_t"/>

   Additional information can be read out with an architectural specific query
   interface, see <iface>x86_instruction_query</iface> for details.

   </add>
   <add id="cpu_instruction_query_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   receiving a handle of type <type>instruction_handle_t</type>.
   </add>
 */
SIM_INTERFACE(cpu_instruction_query) {
        logical_address_t (*logical_address)(
                conf_object_t *cpu, instruction_handle_t *handle);
        physical_address_t (*physical_address)(
                conf_object_t *cpu, instruction_handle_t *handle);
        cpu_bytes_t (*get_instruction_bytes)(
                conf_object_t *cpu, instruction_handle_t *handle);
};

#define CPU_INSTRUCTION_QUERY_INTERFACE "cpu_instruction_query"
// ADD INTERFACE cpu_instruction_query

/* <add-type id="page_crossing_info_t"> 
   States the type of memory accesses related to page crossings. 
   </add-type>
*/
typedef enum {
        Sim_Page_Crossing_None,
        Sim_Page_Crossing_First,
        Sim_Page_Crossing_Second,
} page_crossing_info_t;
        
/* <add id="cpu_memory_query_interface_t"> 

   The <iface>cpu_memory_query</iface> interface is used by
   callbacks of the <type>cpu_memory_cb_t</type> type and requires a
   valid <type>memory_handle_t</type> handle. The handle is only valid during
   the call of the callback. It is used to request information about the memory
   operation being issued by the processor <arg>cpu</arg>.

   <insert-until text="// ADD INTERFACE cpu_memory_query"/>

   The <fun>logical_address</fun> and <fun>physical_address</fun> methods are
   used to get the different addresses of the memory operation.

   Below, callbacks registered by the <fun>register_read_before_cb</fun> or the
   <fun>register_write_before_cb</fun> in the
   <iface>cpu_instrumentation_subscribe</iface> interface or in the
   <iface>cpu_cached_instruction</iface> interface are referred to as being
   in <i>before</i> context. Callbacks registered by the
   <fun>register_read_after_cb</fun> or the <fun>register_write_after_cb</fun>
   in the <iface>cpu_instrumentation_subscribe</iface> interface or in the
   <iface>cpu_cached_instruction</iface> interface are referred to as being
   in <i>after</i> context.

   The <fun>set_host_ptr</fun> method can be used to redirect where data should
   be read from or written to depending on if it is a read or a write
   operation.  The method is only useful for callbacks registered in
   <i>before</i> context. The data pointer <arg>p</arg> needs to be valid
   during the execution of the instruction and must point to enough space to
   carry out the operation. This method is not available in Python.

   The <fun>get_bytes</fun> method is used to retrieve the bytes that is going
   to be read/written if called in a <i>before</i> context and is used to read
   out value that was read/written in <i>after</i> context. The value is
   returned as <type>cpu_bytes_t</type>:

   <insert id="cpu_bytes_t"/>

   The member <tt>data</tt> contains the data pointer, and the member
   <tt>size</tt> contains tha size of the data. It is illegal to access beyond
   the limit. For such access, see the <fun>get_surrounding_bytes</fun> below.

   The read value in <i>before</i> context may not be
   available (a device access for example) and in this case the data member
   will be NULL.

   The <fun>set_bytes</fun> method is used to override the bytes to be read or
   written. The method is only valid in the <i>before</i> context. This method
   can be used instead of the <fun>set_host_ptr</fun> to change the value of
   the operation. The value is passed as <type>cpu_bytes_t</type> and the
   supplied data in the data member need not to be valid after the callback
   returns since the data is copied. The length of the data cannot be changes
   and must be the same as returned by the <fun>get_bytes</fun> method.
        
   The <fun>atomic</fun> method returned true if the operation is considered
   atomic, false otherwise.

   The <fun>arch</fun> method returns the <type>ini_type_t</type> of the memory
   operation.

   Accesses that cross a page boundary are split into two subsequent accesses,
   one for the first page and one for the second page. The
   <fun>get_page_crossing_info</fun> method can be used to distinguish the
   different cases from each other. The returned value from the method is of
   type <type>page_crossing_info_t</type> and can be one of:
   <tt>Sim_Page_Crossing_None</tt> (no crossing access),
   <tt>Sim_Page_Crossing_First</tt> (first part of a crossing access), or
   <tt>Sim_Page_Crossing_Second</tt> (second part of a crossing access).

   The <fun>get_surrounding_bytes</fun> method provides quick access to the
   data surrounding the access. The <arg>granularity_log2</arg> specifies the
   size and alignment of the buffer being returned. For example using 6 for
   <arg>granularity_log2</arg>, will fetch 64 aligned bytes around the
   address of the access. Typically, the largest supported granularity_log2
   size is 12, meaning a 4 KiB page. The returned value is of type buffer_t and
   is only valid in the <type>cpu_memory_cb_t</type> callback. The
   data can be accessed by using the <tt>buffer_t.data</tt> member in the
   returned value. Data can only be read up to the size of the buffer, which is
   stored in the <tt>buffer_t.len</tt> member. Valid memory is only returned if
   the access terminates in simulator cached memory.  If not, the 
   <tt>buffer_t.len</tt> will be is 0, <tt>buffer_t.data</tt> cannot be used.

   Additional information can be read out with an architectural specific
   query interface, see <iface>x86_memory_query</iface> interface for details.

   </add>
   <add id="cpu_memory_query_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   receiving a handle of type <type>memory_handle_t</type>.
   </add>
 */
SIM_INTERFACE(cpu_memory_query) {
        logical_address_t (*logical_address)(
                conf_object_t *cpu, memory_handle_t *handle);
        physical_address_t (*physical_address)(
                conf_object_t *cpu, memory_handle_t *handle);
#ifndef PYWRAP
        void (*set_host_ptr)(
                conf_object_t *cpu, memory_handle_t *handle,
                void *p);
#endif
        cpu_bytes_t (*get_bytes)(conf_object_t *cpu,
                                 memory_handle_t *handle);

        void (*set_bytes)(
                conf_object_t *cpu, memory_handle_t *handle,
                cpu_bytes_t bytes);

        bool (*atomic)(
                conf_object_t *obj, memory_handle_t *handle);
        ini_type_t (*arch)(
                conf_object_t *obj, memory_handle_t *handle);
        page_crossing_info_t (*get_page_crossing_info)(
                conf_object_t *obj, memory_handle_t *handle);
        buffer_t (*get_surrounding_bytes)(
                conf_object_t *cpu, memory_handle_t *handle,
                unsigned granularity_log2);
        
};

#define CPU_MEMORY_QUERY_INTERFACE "cpu_memory_query"        
// ADD INTERFACE cpu_memory_query

/* <add id="cpu_exception_query_interface_t"> 

   The <iface>cpu_exception_query</iface> interface is used to query information
   about an exception for the a generic cpu architecture and should be used from
   a <fun>cpu_exception_cb_t</fun> callback.

   <insert-until text="// ADD INTERFACE cpu_exception_query"/>

   <fun>exception_number</fun> is used to get the vector for the exception.
   </add>

   <add id="cpu_exception_query_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   receiving a handle of type <type>exception_handle_t</type>.
   </add>
*/

SIM_INTERFACE(cpu_exception_query) {
        int (*exception_number)(conf_object_t *cpu, exception_handle_t *handle);
        logical_address_t (*fault_pc)(conf_object_t *cpu,
                                      exception_handle_t *handle);
};
#define CPU_EXCEPTION_QUERY_INTERFACE "cpu_exception_query"
// ADD INTERFACE cpu_exception_query
        
/* <add id="cpu_instruction_cb_t DOC"> <ndx>cpu_instruction_cb_t</ndx>
   <name index="true">cpu_instruction_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_instruction_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_instruction_cb_t"/></doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> or the
   <iface>cpu_cached_instruction</iface> interfaces to get a callback before or
   after an instruction has been executed. The <param>cpu</param> is the
   processor which executed an instruction. The <param>handle</param> is an
   opaque handle associated with the instruction being executed. It can be used
   with the <iface>cpu_instruction_query</iface> interface to get more
   information about the instruction. The <param>user_data</param> is the user
   data for the callback.
   </doc-item> 
  </doc>
  </add>

<add-type id="cpu_instruction_cb_t"></add-type> */
typedef void (*cpu_instruction_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        instruction_handle_t *handle,
        lang_void *user_data);


/* <add id="cpu_callback_free_user_data_cb_t DOC">
   <ndx>cpu_callback_free_user_data_cb_t</ndx>
   <name index="true">cpu_callback_free_user_data_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_callback_free_user_data_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_callback_free_user_data_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Callback function registered through the
   <iface>cpu_cached_instruction</iface> interface to free instruction specific
   user data when the instruction is not being cached anymore.  The
   <arg>user_data</arg> is the pointer to the callback data. <arg>obj</arg> is
   the connection object and <arg>cpu</arg> is the processor that the callback
   was registered for. 
   </doc-item>
   </doc>
   </add>

<add-type id="cpu_callback_free_user_data_cb_t"></add-type> */
typedef void (*cpu_callback_free_user_data_cb_t)(
        conf_object_t *obj, conf_object_t *cpu, lang_void *user_data);

/* <add id="cpu_memory_cb_t DOC">
   <ndx>cpu_memory_cb_t</ndx>
   <name index="true">cpu_memory_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_memory_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_memory_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> or the
   <iface>cpu_cached_instruction</iface> interfaces to get a callback before or
   after an memory access is executed. The <param>cpu</param> is the
   processor which executed a load/store. The <param>handle</param> is an
   opaque handle associated with the instruction being executed. It can be used
   with the <iface>cpu_memory_query</iface> interface to get more
   information about the access. The <param>user_data</param> is the user
   data for the callback.

   </doc-item>
   </doc>
   </add>

<add-type id="cpu_memory_cb_t"></add-type> */
typedef void (*cpu_memory_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        memory_handle_t *handle,
        lang_void *user_data);

/* <add id="cpu_cached_instruction_interface_t">

   This interface allows registration of callbacks for individual instructions,
   i.e., instructions at a specific address.  This interface can only be used
   inside a <type>cpu_cached_instruction_cb_t</type> callback where there is a
   valid <arg>ci_handle</arg>. The callback is installed by calling the
   <fun>register_cached_instruction_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface. The signature of the
   callback function looks like this:

   <insert id="cpu_cached_instruction_cb_t"/>

   When the callback is installed Simics will call it every time an instruction
   is put into the internal instruction cache, which might have different
   entries for different execution modes. For example, in an unlikely case
   where the same instruction is executed in 32-bit mode and later in 64-bit
   mode (running an x86 processor for instance), this callback will be called
   twice, one time for each mode. This allows the user to install specific
   callbacks for this specific instruction and apply filtering based on
   instruction types, etc. This approach is more efficient than doing dynamic
   filtering in the instruction and read/write callbacks installed by the
   <fun>cpu_instrumentation_subscribe</fun> interface.

   The <arg>iq_handle</arg> together with the
   <iface>cpu_instruction_query</iface> interface let a user examine
   instruction properties. The <arg>user_data</arg> is the user data for the
   callback.

   <insert-until text="// ADD INTERFACE cpu_cached_instruction"/>

   The method <fun>register_instruction_before_cb</fun> installs a callback
   that is called before the cached instruction is executed. The
   <arg>ci_handle</arg> is used to bind the callback to the cached instruction.
   The <arg>user_data</arg> is the callback's user data, and a callback for
   freeing the data is also available. The <arg>free_cb</arg> looks like this:

   <insert id="cpu_callback_free_user_data_cb_t"/>

   It is called when Simics wants to free cached instructions, which can happen
   in various situations. <arg>obj</arg> is the connection object that should
   free the data. The callback may be NULL if not needed.

   The <arg>cb</arg> callback type is the same as used for instructions in the
   <iface>cpu_instrumentation_subscribe</iface> interface:
   
   <insert id="cpu_instruction_cb_t"/> 

   The object <arg>obj</arg> is the user object that registers the callback and
   <arg>cpu</arg> is the processor object executing the instruction. The
   <arg>handle</arg> can be used to query more data about the instruction. See
   the <iface>cpu_instruction_query</iface> interface for more information. The
   <arg>user_data</arg> is the user data associated with the callback.

   The user data is a convenient location for storing information about the
   instruction, such as user specific decode information, etc. The user data is
   private for each installed callback.

   The cached information about the instruction is bound to its physical
   address so the logical address cannot typically be saved since it may vary
   between calls (if the MMU-mapping has changed).

   <fun>register_instruction_after_cb</fun> installs a callback that is called
   after a cached instruction is executed. Otherwise it works in the same as
   the before variant.
   However, reading the program counter register for a control flow instruction
   in this callback will reflect the new location, whereas using the
   <iface>cpu_instruction_query</iface> for reading out the instruction address
   will still return the address of the control flow instruction.

   <fun>register_read_before_cb</fun> installs a callback that is called before
   each read operation in the cached instruction. The callback is the
   same as used for read and writes in the
   <iface>cpu_instrumentation_subscribe</iface> interface:

   <insert id="cpu_memory_cb_t"/> 
   
   As for instructions, <arg>obj</arg> is the object that registered the
   callback and <arg>cpu</arg> is the processor doing the access.  The
   <arg>handle</arg> can be used to further operate on the access by using the
   <iface>cpu_memory_query</iface> interface. The
   <arg>user_data</arg> and the <arg>free_cb</arg> arguments to
   <fun>register_read_before_cb</fun> can be used to store user data for the
   read operations in the cached instruction. The user data is private for each
   installed callback but is shared between all read before operations in the
   instruction. 

   <fun>register_read_after_cb</fun> installs a callback that is called after
   each read operation in the cached instruction. The user data is private for
   each installed callback but is shared between all read after operations in
   the instruction. Otherwise it works in the same way as the before variant.

   <fun>register_write_before_cb</fun> and <fun>register_write_after_cb</fun>
   installs callbacks that is called before and after each write operation in
   the cached instructions. Otherwise they work in the same way as the read
   variants.

   Note that installing a read or write callback on an instruction without any
   memory operations will be useless.

   The <fun>add_counter</fun> method can be used to add simple counters for
   this particular instruction. The <arg>counter</arg> argument is a pointer to
   a 64 bit counter value that will be incremented each time the instruction is
   executed. This is the same thing (but more efficient) as registering a
   callback through the <fun>register_instruction_before_cb</fun> method and
   increasing the counter each time it is called.

   Passing true for the argument <arg>use_atomic_increment</arg> makes the
   increment of the counter atomic, which may be useful if the tool cannot have
   an instrumentation connection per processor. This will however be slower due
   to the nature of atomic instructions.

   The location of the counter value can only be removed/deallocated after a
   call to either the <fun>remove_callback</fun> method (passing the
   <tt>cpu_cb_handle_t</tt> returned by
   <fun>register_cached_instruction_cb</fun>), or to the
   <fun>remove_connection_callbacks</fun> method (with the connection object as
   the argument) in the <iface>cpu_instrumentation_subscribe interface</iface>.

   This method is not available in Python.

   The <iface>instrumentation_order</iface> interface cannot be used to
   reorder callbacks installed with the <iface>cpu_cached_instruction</iface>
   interface. The cached callbacks are always called in same order as the 
   <fun>cpu_cached_instruction_cb_t</fun> callback that installed them, 
   and before any callback installed by the corresponding methods in the 
   <iface>cpu_instrumentation_subscribe</iface>.
   </add>

   <add id="cpu_cached_instruction_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   registered by the <fun>register_cached_instruction_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface.
   </add>
*/
SIM_INTERFACE(cpu_cached_instruction) {
        void (*register_instruction_before_cb)(
                conf_object_t *cpu,
                cached_instruction_handle_t *ci_handle,
                cpu_instruction_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_instruction_after_cb)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                cpu_instruction_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_read_before_cb)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                cpu_memory_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_read_after_cb)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                cpu_memory_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_write_before_cb)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                cpu_memory_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_write_after_cb)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                cpu_memory_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
#ifndef PYWRAP
        void (*add_counter)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                uint64 *counter,
                bool use_atomic_increment);
#endif
};

#define CPU_CACHED_INSTRUCTION_INTERFACE "cpu_cached_instruction"
// ADD INTERFACE cpu_cached_instruction

/* <add id="cpu_cached_instruction_once_interface_t">

   This interface extends the <iface>cpu_cached_instruction</iface> interface
   and allows callbacks to be installed that trigger only once, i.e., after
   the first time they have been invoked they are automatically
   removed. Otherwise they are identical to the corresponding methods in the
   <iface>cpu_cached_instruction</iface> interface.

   <insert-until text="// ADD INTERFACE disambiguate_cpu_cached_instruction_once"/></add>

   <add id="cpu_cached_instruction_once_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   registered by the <fun>register_cached_instruction_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface.
   </add>
 */
SIM_INTERFACE(cpu_cached_instruction_once) {
        void (*register_instruction_before_once_cb)(
                conf_object_t *cpu,
                cached_instruction_handle_t *ci_handle,
                cpu_instruction_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
        void (*register_instruction_after_once_cb)(
                conf_object_t *cpu,
                cached_instruction_handle_t *ci_handle,
                cpu_instruction_cb_t cb,
                lang_void *user_data,
                cpu_callback_free_user_data_cb_t free_cb);
};

#define CPU_CACHED_INSTRUCTION_ONCE_INTERFACE "cpu_cached_instruction_once"
// ADD INTERFACE disambiguate_cpu_cached_instruction_once

#ifndef PYWRAP        
// Only for internal use        
SIM_INTERFACE(internal_cached_instruction) {
        void (*add_counters)(
                conf_object_t *obj,
                cached_instruction_handle_t *ci_handle,
                uint64 *i_counter,
                uint64 *t_counter,
                bool use_atomic_increment);
        const char *(*get_service_routine_name)(
                conf_object_t *obj,
                instruction_handle_t *iq_handle);
};
#endif
        
/* <add id="cpu_cached_instruction_cb_t DOC">
   <ndx>cpu_cached_instruction_cb_t</ndx>
   <name index="true">cpu_cached_instruction_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_cached_instruction_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_cached_instruction_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> interface's
   <fun>register_cached_instruction_cb</fun> member function.

   This callback will be called when an instruction is about to be cached in
   the processor model (this is not the same as being in the processor's real
   hardware instruction cache). The <param>cpu</param> is the processor which
   executed an instruction. The <param>ci_handle</param> is an opaque handle
   associated with the instruction being executed, it is used in
   the <iface>cpu_cached_instruction</iface> interface to install instruction
   specific callbacks. The <param>iq_handle</param> is used with the
   <iface>cpu_instruction_query</iface> interface to get more information about
   the instruction. The <param>user_data</param> is the user
   data for the callback.
   </doc-item> 
  </doc>
  </add>
<add-type id="cpu_cached_instruction_cb_t"></add-type> */
typedef void (*cpu_cached_instruction_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        cached_instruction_handle_t *ci_handle,
        instruction_handle_t *iq_handle,
        lang_void *user_data);
        
/* <add-type id="cpu_emulation_t"></add-type> */
typedef enum {
        CPU_Emulation_Fall_Through,
        CPU_Emulation_Control_Flow,
        CPU_Emulation_Default_Semantics,
        CPU_Emulation_Stall,
} cpu_emulation_t;
        
/* <add id="cpu_emulation_cb_t DOC">
   <ndx>cpu_emulation_cb_t</ndx>
   <name index="true">cpu_emulation_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_emulation_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_emulation_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instruction_decoder</iface> interface.

   This callback will be called when an user added instruction is about to
   be executed in the processor. The <param>cpu</param> is the processor which
   executes an instruction. The <param>instruction_user_data</param> is
   the user-data associated with the instruction.
   The return type <type>cpu_emulation_t</type> is used to tell Simics how the
   next instruction should be fetched. For more information, see the
   <iface>cpu_instruction_decoder</iface> interface.
   </doc-item> 
  </doc>
  </add>

<add-type id="cpu_emulation_cb_t"></add-type> */        
typedef cpu_emulation_t (*cpu_emulation_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        lang_void *user_data);

/* <add id="cpu_instruction_decoder_interface_t">

   This interface is used to replace an existing instruction with a user
   defined one or add new instructions to the simulation. The interface can
   only be used from a <fun>cpu_instruction_decoder_cb_t</fun> callback
   registered by the <fun>register_instruction_decoder_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface.

   The interface consist of just one method and looks like this:

   <insert-until text="// ADD INTERFACE cpu_instruction_decoder"/>

   <fun>register_emulation_cb</fun> is used to set a callback function that
   implements the semantics of the new or changed instruction.  Every time the
   instructions is executed on the <arg>cpu</arg> this function will be called
   instead of the build-in implementation. The <arg>handle</arg> is the
   <type>decoder_handle_t</type> handle passed to the
   <fun>cpu_instruction_decoder_cb_t</fun> callback. The <arg>user_data</arg>
   argument is user data for the callback. The <arg>free_cb</arg> is a clean-up
   callback function that Simics calls when the instruction is overwritten or
   if Simics wants to flush decoding caches. This function should deallocate
   any the user data if present. Can be NULL if not used.

   The signature of the emulation callback looks like this:

   <insert id="cpu_emulation_cb_t"/>

   <arg>obj</arg> is the connection object, the same object as passed to the
   cpu_instruction_decoder_cb_t callback. <arg>cpu</arg> is the processor
   executing the replaced instruction. <arg>user_data</arg> is user data for
   the emulation callback. This is a useful place for storing immediate or
   register values for the new instruction. In the emulation function the whole
   Cell Context API is available for use.

   <tt>CPU_Emulation_Fall_Through</tt> should be returned from the emulation
   callback if replaced one is a fall through instruction.  The program counter
   does not need to be updated. If the replaced instruction is doing any
   control flow then <tt>CPU_Emulation_Control_Flow</tt> should be returned and
   the program counter should be set to the destination address. This can be
   done for fall through instruction as well but is less efficient.

   <tt>CPU_Emulation_Default_Semantics</tt> can also be returned to indicate that
   the default semantics should be used instead of the user defined. This can
   be useful if the instruction only should be replaced under certain
   circumstances.
   </add>

   <add id="cpu_instruction_decoder_interface_exec_context"> 
   Threaded Context for all methods, but must be called from a callback
   registered by the <fun>register_instruction_decoder_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface.
   </add>
 */
SIM_INTERFACE(cpu_instruction_decoder) {
        void (*register_emulation_cb)(conf_object_t *cpu,
                                      cpu_emulation_cb_t cb,
                                      decoder_handle_t *handle,
                                      lang_void *user_data,
                                      cpu_callback_free_user_data_cb_t free_cb);
};

#define CPU_INSTRUCTION_DECODER_INTERFACE "cpu_instruction_decoder"
// ADD INTERFACE cpu_instruction_decoder

/* <add id="cpu_instruction_decoder_cb_t DOC">
   <ndx>cpu_instruction_decoder_cb_t</ndx>
   <name index="true">cpu_instruction_decoder_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_instruction_decoder_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_instruction_decoder_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> interface's
   <fun>register_instruction_decoder_cb()</fun> member function.

   This callback will be called when an instruction is about to be cached in
   the processor model (this is not the same as being in the processor's real
   hardware instruction cache). The <param>cpu</param> is the processor which
   executed an instruction. The <param>decoder_handle</param> is an opaque
   handle associated with the instruction being executed and used in the
   <iface>cpu_instruction_decoder</iface> interface.
   The <param>iq_handle</param> is used with the
   <iface>cpu_instruction_query</iface> interface to get more information on
   the instruction. The <param>user_data</param> is the user data
   associated with the callback.
   </doc-item> 
  </doc>
  </add>

 <add-type id="cpu_instruction_decoder_cb_t"></add-type> */
typedef int (*cpu_instruction_decoder_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        decoder_handle_t *decoder_handle,
        instruction_handle_t *iq_handle,
        lang_void *user_data);
        
/* <add id="cpu_instruction_disassemble_cb_t DOC">
   <ndx>cpu_instruction_disassemble_cb_t</ndx>
   <name index="true">cpu_instruction_disassemble_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_instruction_disassemble_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_instruction_disassemble_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> interface's
   <fun>register_instruction_decoder_cb()</fun> member function.

   The callback will be called when a replaced instruction is being
   disassembled. 
   </doc-item> 
  </doc>
  </add>

<add-type id="cpu_instruction_disassemble_cb_t"></add-type> */
typedef tuple_int_string_t (*cpu_instruction_disassemble_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        generic_address_t addr,
        cpu_bytes_t bytes);
        
/* <add id="cpu_address_cb_t DOC">
   <ndx>cpu_address_cb_t</ndx>
   <name index="true">cpu_address_cb_t</name>
   <doc> 
   <doc-item name="NAME">cpu_address_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="cpu_address_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION"> 

   Instrumentation callback function registered through the
   <iface>cpu_instrumentation_subscribe</iface> interface's
   <fun>register_address_before()</fun> member function.

   The callback will be called prior to a processor memory access, allowing
   the callback to change the logical address for an access.

   The argument <arg>obj</arg> is the object installing the callback and the
   <arg>cpu</arg> is the processor generating the logical
   <arg>address</arg>. If the access crosses a page boundary the access will be
   split into two calls. The <arg>part</arg> argument tells if the address
   belongs to the first or the second part. The <type>cpu_address_part_t</type>
   type has two values: <tt>CPU_Address_Part_First</tt> and
   <tt>CPU_Address_Part_Second</tt>. The <arg>handle</arg> can be used to
   extract more information about he address by using a architecture specific
   interface. See the <iface>x86_address_query</iface> for example.

   </doc-item> 
  </doc>
  </add>
 <add-type id="cpu_address_cb_t"></add-type> */
typedef logical_address_t (*cpu_address_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        logical_address_t address,
        address_handle_t *handle,
        lang_void *user_data);

/* <add-type id="cpu_exception_cb_t"></add-type> */
typedef void (*cpu_exception_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        exception_handle_t *exception_handle,
        lang_void *user_data);

/* <add-type id="cpu_exception_return_cb_t"></add-type> */
typedef void (*cpu_exception_return_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        exception_return_handle_t *exception_handle,
        lang_void *user_data);

/* <add-type id="cpu_exception_scope_t"></add-type> */
typedef enum {
        CPU_Exception_All = -1
        /* Any other value corresponds to the exception number as 
           defined in the exception interface for the processor */
} cpu_exception_scope_t;

/* <add-type id="cpu_control_register_scope_t"></add-type> */
typedef enum {
        CPU_Control_Register_All = -1
        /* Any other value corresponds to the register number as 
           defined in the int_register interface for the processor */
} cpu_control_register_scope_t;

/* <add-type id="cpu_mode_change_cb_t"></add-type> */
typedef void (*cpu_mode_change_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        processor_mode_t old_mode, processor_mode_t new_mode,
        lang_void *user_data);

/* <add-type id="cpu_control_register_read_cb_t"></add-type> */
typedef void (*cpu_control_register_read_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        int register_number,
        lang_void *user_data);

/* <add-type id="cpu_control_register_write_cb_t"></add-type> */
typedef void (*cpu_control_register_write_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        int register_number,
        uint64 value,
        lang_void *user_data);
        
/* <add-type id="cpu_access_scope_t">
   Used to select the access scope for instrumentation of memory
   accesses. CPU_Access_Scope_Explicit selects explicit accesses used by an
   instruction such as loading or storing a value. CPU_Access_Scope_Implicit
   selects implicit accesses such as table walks and exceptions handling. See
   the <iface>cpu_instrumentation_subscribe</iface> for more information.
   </add-type> */
typedef enum {
        CPU_Access_Scope_Explicit,
        CPU_Access_Scope_Implicit
} cpu_access_scope_t;
        
/* <add id="cpu_instrumentation_subscribe_interface_t"> 

   The <iface>cpu_instrumentation_subscribe</iface> interface is implemented by
   processor classes that support CPU instrumentation. The interface can be
   used to monitor and instrument various actions through callbacks. It is
   designed for being more efficient than haps and will not synchronize any
   threads. This means that any user of the interface must assume that
   registered callbacks can be called in parallel by multiple threads if any of
   the multi-threaded execution modes are used in Simics.

   A processor implementing the interface is regarded as a provider of
   instrumentation and is typically used by an instrumentation tool object. The
   tool object is supposed to register callbacks in this interface and act when
   they are called. To handle different threads it is recommended that the
   tools uses a connection object (a sub-object created by the tool) that
   registers these callbacks for each processor that it monitors. Then, any
   data collected can be stored in these connection objects and will thus not
   be subject to concurrent access from different threads that hosts the
   processors. The data can than be aggregated by the tool when
   appropriate. This approach needs no synchronization locks and allows for
   efficient simulation.

   See <iface>instrumentation_order</iface> interface for more details on the
   callback order.

   The <arg>cpu</arg> argument in all methods below is the processor object
   implementing this interface.

   The <arg>connection</arg> argument is the "user" object of this interface,
   typically a connection object as described above. However, it can be any
   object and even NULL if there is no suitable object to pass. For instance,
   if the callback is registered by a Python script. In this case
   synchronization will be handled by the Python layer.

   All registration methods in the interface install at least one callback, the
   <arg>cb</arg> argument, that will be called at a particular instrumentation
   point in the processor object. The Simics Cell Context API is
   available for use in these callbacks. The callbacks have different
   signatures depending on their use case. The <arg>data</arg>
   argument is user defined data that can be associated with each
   callback. Every time the callback is called this data will be passed as an
   argument to the callback. The data can be unique for each registration, even
   though the same callback pointer is used.

   <note>
   Note, not all CPU models implement all parts of the instrumentation API.
   Register methods not implemented in this interface will be NULL.
   </note>

   <insert-until text="// ADD INTERFACE cpu_instrumentation_subscribe"/> 

   <b>Callback Related Methods</b>

   Every function in this interface that registers a callback returns a unique
   handle of type <type>cpu_cb_handle_t</type>. The <fun>remove_callback</fun>
   method removes the callback associated with the handle. The
   <fun>enable_callback</fun> and <fun>disable_callback</fun> methods can be
   used to temporarily enable and disable a callback. 

   The <fun>remove_connection_callbacks</fun> removes all callbacks associated
   with a connection object, i.e., all the callbacks that was registered with
   the same connection object. The <fun>enable_connection_callbacks</fun> and
   <fun>disable_connection_callbacks</fun> enables and disables all callbacks
   associated with a connection object. NULL cannot be passed to these methods
   to handle callbacks installed without any connection object.

   The design philosophy is that registering and removing a callback can be
   relatively expensive, whereas enable and disable a callback should be
   cheap. On the other hand, a disabled callback can slow down the simulation
   speed a bit more compared to running without callbacks.

   <b>Instruction Related Methods</b>

   The method <fun>register_instruction_before_cb</fun> installs a callback
   that is called before an instruction is executed. The callback type is as
   follows:
   
   <insert id="cpu_instruction_cb_t"/> 

   The object <arg>obj</arg> is the connection or the user object that
   registers the callback (or NULL if there is no object) and <arg>cpu</arg> is
   the processor object executing the instruction. If a dedicated connection
   object associated with each processor is used, the object's private data can
   store the interface pointers needed to access processor state.  This is a
   useful trick to speed up the simulation speed. Otherwise such interface
   pointers need to be acquired each time the callback is called. If no
   connection object is used the pointers can be saved in the callback's user
   data. The <arg>handle</arg> can be used to query more data about
   the instruction, using the <iface>cpu_instruction_query</iface>
   interface. The <arg>user_data</arg> is the user data associated with the
   callback.

   The registered callback is called for every instruction type. Use the
   <fun>register_cached_instruction_cb</fun> method to control which type
   of instructions that should be instrumented.

   <fun>register_instruction_after_cb</fun> installs a callback that is called
   after an instruction is executed. The callback is of the same type as for
   the before variant and is called for every instruction type. Use the
   <fun>register_cached_instruction_cb</fun> method to control which type
   of instructions that should be instrumented. Reading the program counter
   register for a control flow instruction in this callback will reflect the
   new location, whereas using the <iface>cpu_instruction_query</iface> for
   reading out the instruction address will still return the address of the
   control flow instruction.

   None: When an exception occurs the instruction is aborted and any
   installed callbacks by the <fun>register_instruction_after_cb</fun> method
   are not called.

   <br/>
   <b>Memory Related Methods</b>

   <fun>register_read_before_cb</fun> installs a callback that is called before
   a memory read operation is performed. The callback type is as follows:

   <insert id="cpu_memory_cb_t"/> 
   
   As for instructions, <arg>obj</arg> is the object that registered the
   callback and <arg>cpu</arg> is the processor doing the access.  The
   <arg>handle</arg> can be used to further operate on the
   access by using the <iface>cpu_memory_query</iface> interface. The interface
   can for instance be used to read the data of the memory operation. For more
   information, see the documentation for the <iface>cpu_memory_query</iface>
   interface.

   The <arg>scope</arg> argument defines the access scope for the
   callback. There are two defined scopes: <tt>CPU_Access_Scope_Explicit</tt>
   and <tt>CPU_Access_Scope_Implicit</tt>. The explicit scope installs a
   callback for every operation done explicitly by an instruction, such as
   loading or storing a value. The implicit scope installs a callback for every
   implicit access, such as table walks or memory accesses performed by
   exception and interrupt handling. If all accesses are requested the same
   callback function can be registered for both scopes by registering the
   callback twice, one for each scope. 

   If a memory access crosses a page boundary the access will be split into two
   separate calls, one for the first part covering the first page, and one for
   the second part covering the second page. See the <fun>get_page_crossing_info</fun>
   method in the <iface>cpu_memory_query</iface> interface for a way to
   distinguish the different cases.

   The <arg>user_data</arg> is the data associated with the callback.

   Similar to <fun>register_read_before_cb</fun>, the following three methods
   work in the same way:

   <fun>register_read_after_cb</fun> installs a callback that is called after
   a memory read operation is performed. 

   <fun>register_write_before_cb</fun> installs a callback that is called before
   a memory write operation is performed.

   <fun>register_write_after_cb</fun> installs a callback that is called after
   a memory write operation is performed.

   Memory accesses of prefetch types or any other control operation calls the
   read callbacks. To distinguish them from reads an architecture specific
   interface can be used to lookup the access type. See the
   <iface>x86_memory_query</iface> interface for example.

   None: When an exception occurs the instruction is aborted and any
   installed memory callbacks after this point are not called.

   <br/>
   <b>Addresses</b>

   <note>Only available on x86 targets.</note>
   <fun>register_address_before_cb</fun> can be used to register a callback
   that will be called each time a logical address is generated by an
   instruction for an explicit read or a write operation. This occurs before
   the actual memory operation takes place. This allows a user to inspect and
   change the target address for the operation. The callback has the following
   signature:

   <insert id="cpu_address_cb_t"/>

   The argument <arg>obj</arg> is the object installing the callback and the
   <arg>cpu</arg> is the processor generating the logical
   <arg>address</arg>. The <arg>handle</arg> can be used to
   extract more information about the address by using an architecture specific
   interface. See the <iface>x86_address_query</iface> for details.

   The new logical address should be returned by the callback.

   <br/>
   <b>Cached Instructions</b>

   <fun>register_cached_instruction_cb</fun> is used for installing a callback
   that is called every time Simics inserts the instruction into an internal
   instruction cache, i.e., executing from a specific address with a specific
   execution mode for the first time. Executing the instruction again will
   typically not invoke the callback since the instruction is already placed in
   the instruction cache. However, if the cache is flushed and the instruction
   is executed again the callback will once again be called. The callback has
   the following signature:

   <insert id="cpu_cached_instruction_cb_t"/>

   From this callback one can use the <arg>cpu_cached_instruction</arg>
   interface and <arg>ci_handle</arg> to register instrumentation callbacks for
   this specific instruction alone. The installed callbacks will be called
   every time the instruction is executed (even the first time just after the
   <tt>cpu_cached_instruction_cb_t</tt> callback has returned).  
   This means that the
   user can filter out certain instructions of interest and instrument only
   those. The <arg>iq_handle</arg> and the <iface>cpu_instruction_query</iface>
   interface can be use to do the filtering by examining the instruction. The
   <arg>user_data</arg> is the callback user data for the callback.

   For callbacks registered for memory operations only those in the explicit
   scope issued by the instruction will be instrumented, e.g., hardware table
   walks and exceptions will not be considered by this method. To instrument
   these operations use the <fun>register_(read/write)_(before/after)_cb</fun>
   with the implicit scope instead.

   If no callbacks are registered by calling the
   <arg>cpu_cached_instruction</arg> interface, the instruction will not be
   instrumented.

   <br/>
   <b>Instruction Set Augmentation</b>
 
   <fun>register_instruction_decoder_cb</fun> lets the user redefine
   instruction semantics in Simics, or implement new instructions. The
   <arg>cb</arg> argument is a callback function that will be called every time
   Simics decodes an instruction. From this callback the user can accept the
   instruction or deny it. In most cases this only happens once per instruction
   address since Simics usually caches decoding results in the internal
   instruction cache. If the cache is flushed the callback may be called again.
 
   The callback signature looks like this:
   
   <insert id="cpu_instruction_decoder_cb_t"/>

   The instruction bytes are read by using the <fun>get_instruction_bytes</fun>
   method of the <iface>cpu_instruction_query</iface> interface together with
   the <arg>iq_handle</arg>. The returned value is of a
   <type>cpu_bytes_t</type> type. To access the bytes use the <tt>data</tt> and
   the <tt>size</tt> members in the returned value.

   If the decoder requires more bytes (i.e., because the new instruction is
   longer), a negative integer value should be returned by the <arg>cb</arg>
   function, indicating the number of bytes needed. For example, if the
   available bytes are 3 but the decoder need at least 4 bytes, -4 should be
   returned. The callback will then be called again with more available bytes
   (this can be repeated if the new instruction requires even more bytes at
   this point). Note that requesting more data than actual needed can cause
   spurious page faults if the data crosses a page boundary. 

   If the instruction is accepted by the callback a positive integer number
   should be returned corresponding to the length of the instruction. In this
   case the <fun>register_emulation_cb</fun> method of the
   <iface>cpu_instruction_decoder</iface> interface should be called to set the
   actual (user) function that Simics will call each time the instruction is
   executed. 

   If the <arg>cb</arg> callback should ignore the instruction the number 0
   should be returned. This means that any other registered decoder will have a
   chance to decode the instruction. If no decoder accepts it, Simics' default
   implementation will be used.

   The <fun>register_emulation_cb</fun> method also needs the
   <arg>decoder_handle</arg> which is available in the dedoder callback. For
   more information, see the documentation of the
   <iface>cpu_instruction_decoder</iface> interface.

   A <arg>disass_cb</arg> argument should also be passed to the
   <fun>register_instruction_decoder_cb</fun> method. This function is called
   every time Simics wants to disassemble an instruction. For every accepted
   instruction a corresponding disassembly string should be returned by this
   function. It has the following signature:

   <insert id="cpu_instruction_disassemble_cb_t"/>

   <arg>obj</arg> is the object registering the
   <fun>register_instruction_decoder_cb</fun> and <arg>cpu</arg> is the
   processor disassembling the instruction. <fun>addr</fun> is the address of
   the instruction in a generic form. This means that it is typically a
   physical address or a logical address depending on the context of the
   disassembling. The address can be used for offset calculations, i.e.,
   displaying an absolute address instead of a relative one, for example in a
   branch instruction. The <arg>bytes</arg> argument should be used to read
   instruction bytes. The return value is of type
   <type>tuple_int_string_t</type> and should be filled with the instruction
   length and an allocated (e.g., malloc) string representing the disassembly
   of the instruction. The ownership of the string is transferred to the
   calling environment which will free it when it is no longer needed.

   If too few bytes are passed for the instruction to be disassembled a
   negative value should be returned for the length indicating the needed
   bytes. The <arg>disass_cb</arg> is then called again with more bytes. If the
   instruction is rejected a length of 0 should be returned and the string
   should be set to NULL. 

   <br/>
   <b>Exception Related Methods</b>

   <fun>register_exception_before_cb</fun> is used to register a callback that
   will be called before an exception or interrupt is taken.  The
   <arg>exception_number</arg> can be used to select a callback on a specific
   exception, with the same number as used in the <iface>exception</iface>
   interface. If all exceptions should be subscribed to, the
   <tt>CPU_Exception_All</tt> constant can be used.
   The callback signature looks like this:

   <insert id="cpu_exception_cb_t"/>
   
   The <arg>obj</arg> is the object registering the callback and <arg>cpu</arg>
   is the processor that takes the exception or receives the interrupt. The
   handle is used to get more architectural information about the exception,
   for example, see the <iface>x86_exception_query</iface> for details. The
   <arg>user_data</arg> is the callback user data.

   No architectural state has been changed in this callback to reflect the
   exception or interrupt, e.g., the program counter will still be at the
   faulting instruction. Since an exception can occur while handling an
   exception it is not always the case that this callback corresponds to the
   final taken exception. Recursive exception will result in another call to
   this callback.
  
   <fun>register_exception_after_cb</fun> is used to register a callback that
   will be called after an exception was taken. It takes the same arguments as
   <fun>register_exception_before_cb</fun>. In this callback the architectural
   state has been updated, e.g., the program counter points to the first
   instruction of the exception handler. The callback is of the same type as
   for the before variant.

   <fun>register_exception_return_before_cb</fun> is used to register a
   callback that will be called just before an exception handler is done and
   resumes execution of the normal program code.  The callback signature looks
   like this:

   <insert id="cpu_exception_return_cb_t"/>
   
   The <arg>obj</arg> is the object registering the callback and <arg>cpu</arg>
   is the processor that takes the exception or receives an interrupt. The
   handle is used to get more architectural information about the
   exception. Currently there is no interface available for this.
   
   <fun>register_exception_return_after_cb</fun> is used to register a callback
   that will be called after an exception has been executed, e.g., the program
   counter points to the normal program where execution will continue.

   <br/>
   <b>Execution Mode</b>

   <fun>register_mode_change_cb</fun> is used to register a callback that will
   be called every time the processor changes mode. The supported modes are:
   user, supervisor, and hypervisor.

   The callback signature looks lite this:

   <insert id="cpu_mode_change_cb_t"/>

   The <arg>obj</arg> is the object registering the callback and <arg>cpu</arg>
   is the processor that changes mode. The <arg>old_mode</arg> is the mode
   before the change and new_mode is the mode after the change.

   <br/>
   <b>Control Registers</b>

   <fun>register_control_register_read_before_cb</fun> is used to register a
   callback that will be called before every control register read. The
   <arg>register_number</arg> is the control register number to install the
   callback on. It is the same number which is used in the
   <iface>int_register</iface> interface. The constant
   <tt>CPU_Control_Register_All</tt> can be used to subscribe to all control
   registers.

   The callback signature looks like this:

   <insert id="cpu_control_register_read_cb_t"/>

   The <arg>obj</arg> is the object registering the callback and <arg>cpu</arg>
   is the processor that reads the control register <arg>register_number</arg>.
   The <arg>user_data</arg> is the user data for the callback.

   <fun>register_control_register_write_before_cb</fun> is used to register a
   callback that will be called before every control register write. The
   <arg>register_number</arg> is the control register number to install the
   callback on. It is the same number which is used in the
   <iface>int_register</iface> interface. The constant
   <tt>CPU_Control_Register_All</tt> can be used to subscribe to all control
   registers. 

   The callback signature looks like this:

   <insert id="cpu_control_register_write_cb_t"/>

   The <arg>obj</arg> is the object registering the callback and <arg>cpu</arg>
   is the processor that writes to the control register
   <arg>register_number</arg>.  The <arg>value</arg> is the value that will be
   written.  The <arg>user_data</arg> is the user data for the callback.

   </add>
   <add id="cpu_instrumentation_subscribe_interface_exec_context">
   Global Context for all methods.
   </add>
 */
SIM_INTERFACE(cpu_instrumentation_subscribe) {
        /* Callback specific methods */
        void (*remove_callback)(
                conf_object_t *NOTNULL cpu,
                cpu_cb_handle_t *handle);
        void (*enable_callback)(
                conf_object_t *NOTNULL cpu,
                cpu_cb_handle_t *handle);
        void (*disable_callback)(
                conf_object_t *NOTNULL cpu,
                cpu_cb_handle_t *handle);

        /* Callback groups methods, operating on several callbacks 
           associated to a connection. */
        void (*remove_connection_callbacks)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *NOTNULL connection);
        void (*enable_connection_callbacks)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *NOTNULL connection);
        void (*disable_connection_callbacks)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *NOTNULL connection);

        /* Subscribe methods */
        cpu_cb_handle_t *(*register_instruction_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_instruction_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_instruction_after_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_instruction_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_read_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_access_scope_t scope,
                cpu_memory_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_read_after_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_access_scope_t scope,
                cpu_memory_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_write_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_access_scope_t scope,
                cpu_memory_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_write_after_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_access_scope_t scope,
                cpu_memory_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_address_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_address_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_cached_instruction_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_cached_instruction_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_instruction_decoder_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_instruction_decoder_cb_t cb,
                cpu_instruction_disassemble_cb_t disass_cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_exception_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                int exception_number,
                cpu_exception_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_exception_after_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                int exception_number,
                cpu_exception_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_exception_return_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_exception_return_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_exception_return_after_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_exception_return_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_mode_change_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_mode_change_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_control_register_read_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                int register_number,
                cpu_control_register_read_cb_t cb,
                lang_void *data);
        cpu_cb_handle_t *(*register_control_register_write_before_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                int register_number,
                cpu_control_register_write_cb_t cb,
                lang_void *data);
};
        
#define CPU_INSTRUMENTATION_SUBSCRIBE_INTERFACE \
        "cpu_instrumentation_subscribe"
// ADD INTERFACE cpu_instrumentation_subscribe

typedef enum {
        CPU_Stream_Instruction_PA,
        CPU_Stream_Instruction_VA,
        CPU_Stream_Instruction_VA_After,

        CPU_Stream_Read_PA,
        CPU_Stream_Read_VA,

        CPU_Stream_Write_PA,
        CPU_Stream_Write_VA,

        CPU_Stream_Read_Bytes,
        CPU_Stream_Written_Bytes,

        CPU_Stream_Address_VA,
        CPU_Stream_Object,

        CPU_Stream_Opcode = 17,

        CPU_Stream_Local = 32,
        CPU_Stream_User_Data = 63 // last, enum can used as bit number
} cpu_stream_enum_t;

// Contains bits created by using bit numbers from cpu_stream_enum_t
typedef uint64 cpu_stream_bits_t;

#define CPU_STREAM_TAG_WIDTH 6
#define CPU_STREAM_TAG_MASK ((1 << CPU_STREAM_TAG_WIDTH) - 1)
#define CPU_STREAM_MAKE_TAG(type, data) \
        (((data) & ~CPU_STREAM_TAG_MASK) | ((type) & CPU_STREAM_TAG_MASK))
#define CPU_STREAM_MAKE_TAG_SHIFTED_DATA(type, data)                                \
        (((data) << CPU_STREAM_TAG_WIDTH) | ((type) & CPU_STREAM_TAG_MASK))
#define CPU_STREAM_BIT(type) (1ULL << (uint64)type)

typedef struct {
        uint64 tag;  // stored as CPU_STREAM_MAKE_TYPE(type, data)
        uint64 data; // tag-specific-data
} cpu_stream_data_t;

#ifndef PYWRAP
/* <add id="cpu_cached_stream_interface_t">
   This interface is internal.
   </add>

   <add id="cpu_cached_stream_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   registered by the <fun>register_cached_instruction_cb</fun> method in the
   <iface>cpu_instrumentation_subscribe</iface> interface.
   </add>
*/
SIM_INTERFACE(cpu_cached_stream) {
        void (*add_stream)(conf_object_t *obj,
                           cached_instruction_handle_t *ci_handle,
                           cpu_stream_bits_t bits,
                           cpu_stream_data_t **data,
                           uint64 user_data); // only with CPU_Stream_User_Data
};
#define CPU_CACHED_STREAM_INTERFACE "cpu_cached_stream"
// ADD INTERFACE cpu_cached_stream
#endif

/* <add id="cpu_instrumentation_stream_t">
   This interface is internal.
   </add>

   <add id="cpu_instrumentation_stream_interface_exec_context">
   Global Context.
   </add>
*/
SIM_INTERFACE(cpu_instrumentation_stream) {
        cpu_stream_handle_t *(*add_stream)(conf_object_t *NOTNULL cpu,
                                           conf_object_t *connection,
                                           cpu_stream_bits_t bits,
                                           cpu_stream_data_t **data,
                                           cpu_access_scope_t scope);
        void (*remove_stream)(conf_object_t *cpu, cpu_stream_handle_t *handle);
};

#define CPU_INSTRUMENTATION_STREAM_INTERFACE \
        "cpu_instrumentation_stream_interface"

#ifndef PYWRAP
/* <add id="pre_decoder_cb_t DOC">
   <ndx>pre_decoder_cb_t</ndx>
   <name index="true">pre_decoder_cb_t</name>
   <doc>
   <doc-item name="NAME">pre_decoder_cb_t</doc-item>
   <doc-item name="SYNOPSIS"><insert id="pre_decoder_cb_t"/>
   </doc-item>
   <doc-item name="DESCRIPTION">

   This callback is used by the <iface>pre_decoder_interface_t</iface> to
   observe and modify the instruction byte stream.

   The <arg>obj</arg> is the connection object which owns the callback, or
   NULL, if no connection is used. <arg>cpu</arg> is the cpu which decodes the
   instructions. <arg>instruction_start_address</arg> is the address of the
   first byte in the instruction that is being decoded. For x86 this is the
   linerar address. <arg>opcode_bytes</arg> is a pointer to the first opcode
   byte in the instruction. The <arg>valid_bytes</arg> tells how many bytes
   that can read from the first byte.

   If the decoded instruction crosses a page boundary this callback is invoked
   twice, first with the bytes on the first page and then with additional bytes
   located on the next page. The <arg>instruction_position</arg> tells where in
   the instruction opcode the next bytes are added from the second page, and
   hence will be greater 0 if this occurs.

   The <arg>position_physical_address</arg> reflects the physical address of
   the <arg>instruction_position</arg>.

   The function should return true if the memory was modified, otherwise false.
   </doc-item>
  </doc>
  </add>

<add-type id="pre_decoder_cb_t"></add-type> */
typedef bool (*pre_decoder_cb_t)(
        conf_object_t *obj,
        conf_object_t *cpu,
        generic_address_t instruction_start_address,
        physical_address_t position_physical_address,
        uint8 *opcode_bytes,
        unsigned valid_bytes,
        unsigned instruction_position,
        lang_void *user_data);

/* <add id="pre_decoder_interface_t">

   This interface extends the <iface>cpu_instrumentation_subscribe</iface>
   interface and allows a user to observe and change the bytes in the
   instruction stream before the target processor tries to decode them.  This
   can be used to model data encryption of memory or instruction caches with
   different content than the memory.

   It is currently offered as a separate interface for backwards compatibility,
   and only available for C/C++ development, i.e., no Python mapping exists.
   The interface is only implemented for x86 target processors.

   The <fun>register_pre_decoder_cb</fun> method registers a callback,
   <arg>cb</arg> of type <type>pre_decoder_cb_t</type>, which is called before
   an instruction is decoded an put into Simics internal decode cache. This
   means that this callback is called only the first time an instruction is
   executed (unless it is evicted from the decode cache).

   The <arg>cpu</arg> is the processor that decodes the instructions, and
   <arg>connection</arg> is the instrumentation connect object that receives
   the callback. The connection can be NULL, if no connection is available. The
   <arg>data</arg> is the callback data for the callback.

   See the documentation for the <type>pre_decoder_cb_t</type> for more
   information.

   To remove the callback use either <fun>remove_callback</fun> or
   <fun>remove_connection_callbacks</fun> methods in the
   <iface>cpu_instrumentation_subscribe interface</iface>. To identify the
   callback to remove, pass the return value, a <type>cpu_cb_handle_t</type>
   handle, from the register method or the connection object used. The
   callback cannot be disabled.

   <insert-until text="// ADD INTERFACE pre_decoder"/>

   </add>
   <add id="pre_decoder_interface_exec_context">
   Global context.
   </add>
 */
SIM_INTERFACE(pre_decoder) {
        cpu_cb_handle_t *(*register_pre_decoder_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                pre_decoder_cb_t cb,
                lang_void *data);
};
#endif

#define PRE_DECODER_INTERFACE "pre_decoder"
// ADD INTERFACE pre_decoder

#if defined(__cplusplus)
}
#endif
        
#endif  /* SIMICS_SIMULATOR_IFACE_CPU_INSTRUMENTATION_H */
