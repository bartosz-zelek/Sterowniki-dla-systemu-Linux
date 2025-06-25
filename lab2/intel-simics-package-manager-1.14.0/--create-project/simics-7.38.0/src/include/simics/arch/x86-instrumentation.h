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

#ifndef SIMICS_ARCH_X86_INSTRUMENTATION_H
#define SIMICS_ARCH_X86_INSTRUMENTATION_H

#include <simics/arch/x86.h>
#include <simics/model-iface/cpu-instrumentation.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="x86_instruction_query_interface_t"> 

   The <iface>x86_instruction_query</iface> interface is used to query
   additional information about an instruction for the x86 architecture and is a
   complement to the <iface>cpu_instruction_query</iface> interface.

   <insert-until text="// ADD INTERFACE x86_instruction_query"/>

   The <fun>linear_address</fun> is used to get the linear address of the
   instruction.
   </add>

   <add id="x86_instruction_query_interface_exec_context">
   Threaded Context for all methods, but must be called from a callback
   receiving a handle of type <type>instruction_handle_t</type>.
   </add>
*/     

SIM_INTERFACE(x86_instruction_query) {
        linear_address_t (*linear_address)(conf_object_t *cpu,
                                           instruction_handle_t *handle);
};

#define X86_INSTRUCTION_QUERY_INTERFACE "x86_instruction_query"
// ADD INTERFACE x86_instruction_query

/* <add id="x86_memory_query_interface_t"> 

   The <iface>x86_memory_query</iface> interface is used to query additional
   information about an instruction for the x86 architecture and is a complement
   to the <iface>cpu_memory_query</iface> interface.

   <insert-until text="// ADD INTERFACE x86_memory_query"/>

   The <fun>linear_address</fun> is used to get the linear address of the
   access.

   The <fun>segment</fun> function is used to get the segment register used
   in the access.

   The <fun>access_type</fun> returns the <type>x86_access_type_t</type> of the
   access and the <fun>memory_type</fun> returns the
   <type>x86_memory_type_t</type> of the access.
   </add>

   <add id="x86_memory_query_interface_exec_context">
   Cell Context for all methods, but must be called from a callback
   receiving a handle of type <type>memory_handle_t</type>.
   </add>
*/     
SIM_INTERFACE(x86_memory_query) {
        linear_address_t (*linear_address)(conf_object_t *cpu,
                                           memory_handle_t *handle);
        x86_seg_t (*segment)(conf_object_t *cpu, memory_handle_t *handle);
        x86_access_type_t (*access_type)(conf_object_t *obj,
                                         memory_handle_t *handle);
        x86_memory_type_t (*memory_type)(conf_object_t *obj,
                                         memory_handle_t *handle);
};

#define X86_MEMORY_QUERY_INTERFACE "x86_memory_query"        

// ADD INTERFACE x86_memory_query

/* <add id="x86_exception_query_interface_t"> 

   The <iface>x86_exception_query</iface> interface is used to query information
   about an exception for the x86 architecture and should be used from a
   <fun>cpu_exception_cb_t</fun> callback.

   <insert-until text="// ADD INTERFACE x86_exception_query"/>

   The <fun>vector</fun> method is used to get the exception vector.  The
   <fun>source</fun> method is used to get the source of the exception and the
   <fun>error_code</fun> method is used to get the error code of the exception.
   </add>

   <add id="x86_exception_query_interface_exec_context">
   Cell Context for all methods, but must be called from a callback
   receiving a handle of type <type>exception_handle_t</type>.
   </add>
*/

SIM_INTERFACE(x86_exception_query) {
        uint8 (*vector)(
                conf_object_t *cpu, exception_handle_t *handle);
        x86_exception_source_t (*source)(
                conf_object_t *cpu, exception_handle_t *handle);
        uint32 (*error_code)(
                conf_object_t *cpu, exception_handle_t *handle);
};
#define X86_EXCEPTION_QUERY_INTERFACE "x86_exception_query"        
// ADD INTERFACE x86_exception_query

/* <add id="x86_address_query_interface_t"> 

   The <iface>x86_address_query</iface> interface is used to query information
   about an address for the x86 architecture and should be used from a
   <fun>cpu_memory_address_cb_t</fun> callback.

   <insert-until text="// ADD INTERFACE x86_address_query"/>

   The <fun>segment</fun> is used to get the segment register used in the
   address calculation. The <arg>handle</arg> is the address handle passed to
   <fun>cpu_memory_address_cb_t</fun>.

   If the access crosses a page boundary the access will be split into two
   calls. The <fun>get_page_crossing_info</fun> can be used to distinguish the
   different cases from each other. The value returned is of type
   <type>page_crossing_info_t</type> and is one of:
   <tt>Sim_Page_Crossing_None</tt> (no crossing access),
   <tt>Sim_Page_Crossing_First</tt> (first part of a crossing access),
   <tt>Sim_Page_Crossing_Second</tt> (second part of a crossing access).
   </add>

   <add id="x86_address_query_interface_exec_context">
   Cell Context for all methods, but must be called from a callback
   receiving a handle of type <type>address_handle_t</type>.
   </add>
*/

SIM_INTERFACE(x86_address_query) {
        x86_seg_t (*segment)(
                conf_object_t *cpu, address_handle_t *handle);
        page_crossing_info_t (*get_page_crossing_info)(
                conf_object_t *cpu, address_handle_t *handle);
};
#define X86_ADDRESS_QUERY_INTERFACE "x86_address_query"        
// ADD INTERFACE x86_address_query

/* <add-type id="x86_mode_switch_cb_t"></add-type> */
typedef void (*x86_mode_switch_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        x86_detailed_exec_mode_t mode,
        lang_void *user_data);
        
/* <add id="x86_instrumentation_subscribe_interface_t"> 

   The <iface>x86_instrumentation_subscribe</iface> interface is an x86
   specific complement to the <iface>cpu_instrumentation_subscribe</iface>
   interface. It is implemented by processor objects that support
   instrumentation. It has the same requirements as the
   <iface>cpu_instrumentation_subscribe</iface> interface.

   <insert-until text="// ADD INTERFACE x86_instrumentation_subscribe"/>

   The <fun>register_mode_switch_cb</fun> method is used to register a callback
   that is called whenever the processor <arg>cpu</arg> changes execution
   mode. The <arg>connection</arg> argument is the user object that
   registers the callback. This object will be passed to the callback when it
   is called. <arg>cb</arg> is the callback and <arg>user_data</arg> is user data
   for the callback. The signature of the callback looks like this:
   
   <insert id="x86_mode_switch_cb_t"/>

   The <arg>obj</arg> is the connection object that registered the
   callback. <arg>new_mode</arg> argument contains the new mode. The possible
   modes available are captured in the <type>x86_detailed_exec_mode_t</type>
   type:

   <insert id="x86_detailed_exec_mode_t"/>

   The <arg>user_data</arg> is the user data associated with the callback.

   <fun>register_illegal_instruction_cb</fun> lets a user to implement new
   instructions. The <arg>cb</arg> argument is a callback function that will be
   called every time Simics does not decode an instruction. Allows new x86
   instructions to be implemented which otherwise cause illegal instruction
   exception. Compared to <fun>register_instruction_decoder_cb</fun> method of
   <iface>cpu_instrumentation_subscribe</iface> interface this interface cannot
   change any instruction that correctly decodes according to the existing
   instruction set architecture. From this callback the user can accept the
   instruction or deny it. In most cases this only happens once per instruction
   address since Simics usually caches decoding results in the internal
   instruction cache. If the cache is flushed the callback may be called again.
   This instrumentation feature (if used alone) does not prevent VMP execution
   since illegal instructions cause exit while running inside VMP mode.

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
   chance to decode the instruction. If no decoder accepts it, Simics will
   generate illegal instruction exception.

   The <fun>register_emulation_cb</fun> method also needs the
   <arg>decoder_handle</arg> which is available in the dedoder callback. For
   more information, see the documentation of the
   <iface>cpu_instruction_decoder</iface> interface.

   A <arg>disass_cb</arg> argument should also be passed to the
   <fun>register_illegal_instruction_cb</fun> method. This function is called
   every time Simics wants to disassemble an instruction. For every accepted
   instruction a corresponding disassembly string should be returned by this
   function. It has the following signature:

   <insert id="cpu_instruction_disassemble_cb_t"/>

   <arg>obj</arg> is the object registering the
   <fun>register_illegal_instruction_cb</fun> and <arg>cpu</arg> is the
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

   To remove the callback used one of the methods <fun>remove_callback</fun> or
   <fun>remove_connection_callbacks</fun> in the
   <iface>cpu_instrumentation_subscribe</iface> interface.

   The callback can also be enabled and disabled with the corresponding methods
   in the <iface>cpu_instrumentation_subscribe</iface> interface.
   </add>

   <add id="x86_instrumentation_subscribe_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(x86_instrumentation_subscribe) {
        cpu_cb_handle_t *(*register_mode_switch_cb)(
                conf_object_t *cpu, conf_object_t *connection,
                x86_mode_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_illegal_instruction_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_instruction_decoder_cb_t cb,
                cpu_instruction_disassemble_cb_t disass_cb,
                lang_void *data);
};
#define X86_INSTRUMENTATION_SUBSCRIBE_INTERFACE \
        "x86_instrumentation_subscribe"
// ADD INTERFACE x86_instrumentation_subscribe

/* <add id="x86_instrumentation_subscribe_v2_interface_t">
   The <iface>x86_instrumentation_subscribe_v2</iface> interface is an x86
   specific complement to the <iface>cpu_instrumentation_subscribe</iface>
   interface. It is implemented by x86 processor objects that support
   instrumentation. It works in the same way as the
   <iface>cpu_instrumentation_subscribe</iface> interface, and the methods
   <fun>remove/enabled/disable_callback</fun> and
   <fun>remove/enabled/disable_connection_callbacks</fun> in that interface
   should be used to operate on callbacks in this interface as well, using the
   <type>cpu_cb_handle_t</type> handle.

   The v2 variant fixes some problems with the
   <iface>x86_instrumentation_subscribe</iface>. In that interface, the
   <fun>register_mode_switch_cb</fun> method was invoked when the new mode had
   already been set up. This means that if you need to do some bookkeeping
   when leaving a mode, that mode had already been switched out in the callback.
   For instance, descriptor registers has already been changed.

   This interface adds two methods to handle this,
   <fun>register_mode_leave_cb</fun> and <fun>register_mode_enter_cb</fun>. The
   leave variant is called when the processor still is located in the previous
   mode, but about to enter a new mode, and the enter variant is called when
   the processor has switched to the new mode (corresponds to the
   <fun>register_mode_switch_cb</fun> in the old
   <iface>x86_instrumentation_subscribe</iface> interface).

   The <fun>register_illegal_instruction_cb</fun> works as in the
   <iface>x86_instrumentation_subscribe</iface> interface.

   <insert-until text="// ADD INTERFACE disambiguate_x86_instrumentation_subscribe_v2"/>

   </add>

   <add id="x86_instrumentation_subscribe_v2_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(x86_instrumentation_subscribe_v2) {
        cpu_cb_handle_t *(*register_mode_enter_cb)(
                conf_object_t *cpu, conf_object_t *connection,
                x86_mode_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_mode_leave_cb)(
                conf_object_t *cpu, conf_object_t *connection,
                x86_mode_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_illegal_instruction_cb)(
                conf_object_t *NOTNULL cpu,
                conf_object_t *connection,
                cpu_instruction_decoder_cb_t cb,
                cpu_instruction_disassemble_cb_t disass_cb,
                lang_void *data);
};

#define X86_INSTRUMENTATION_SUBSCRIBE_V2_INTERFACE \
        "x86_instrumentation_subscribe_v2"
// ADD INTERFACE disambiguate_x86_instrumentation_subscribe_v2

/* <add-type id="vmx_mode_t"> This enum is used to distinguish VMX modes. It is
   used by the <type>vmx_mode_switch_cb_t</type>.
   </add-type>
*/
typedef enum {
        Vmx_Off = 0,
        Vmx_Root = 1,
        Vmx_Non_Root = 2
} vmx_mode_t;

/* <add-type id="vmx_mode_switch_cb_t">
   This callback type is used by the
   <iface>vmx_instrumentation_subscribe</iface> to implements VMX mode switch
   instrumentation.
   </add-type>
*/
typedef void (*vmx_mode_switch_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        vmx_mode_t mode,
        physical_address_t vmcs_address,
        lang_void *user_data);

/* <add id="vmx_instrumentation_subscribe_interface_t">

   This x86 specific interface allows a user to keept track of VMX mode
   switches in the processor.

   The <fun>register_vmx_mode_leave_cb</fun> registers a callback of type
   <type>vmx_mode_switch_cb_t</type> that will be called before the processor
   leaves a particular VMX mode.

   The <fun>register_vmx_mode_enter_cb</fun> registers a callback of type
   <type>vmx_mode_switch_cb_t</type> that will be called after the processor
   enters a particular VMX mode.

   The callback in both cases looks like this:
   <insert id="vmx_mode_switch_cb_t"/>

   The <arg>obj</arg> argument is the user object that registered the
   callback, or NULL if no such object exists.

   The <arg>cpu</arg> argument is the processor that switches VMX mode.

   The mode that will be left or entered is passed in the
   <arg>mode</arg> argument in the callback.

   The different VMX modes are defined in the following type:
   <insert id="vmx_mode_t"/>

   The <arg>vmcs_address</arg> argument specifies the Virtual Machine Control
   Structure address that is currently being used. If not available, it will be
   passed as (physical_address_t)(-1).

   When leaving a mode and then entering another mode, the mode can actually be
   reported as to be the same in certain situations. This is not a bug, it is a
   consequent of how Simics is implemented. It would require a lot of extra code
   to filter those out.

   The <arg>user_data</arg> argument is the callback user data, passed to the
   register methods.

   <insert-until text="// ADD INTERFACE vmx_instrumentation_subscribe"/>

   </add>

   <add id="vmx_instrumentation_subscribe_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(vmx_instrumentation_subscribe) {
        cpu_cb_handle_t *(*register_vmx_mode_leave_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                vmx_mode_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_vmx_mode_enter_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                vmx_mode_switch_cb_t cb,
                lang_void *user_data);
};

#define VMX_INSTRUMENTATION_SUBSCRIBE_INTERFACE \
        "vmx_instrumentation_subscribe"
// ADD INTERFACE vmx_instrumentation_subscribe

/* <add-type id="smm_switch_cb_t">
   This callback type is used by the
   <iface>smm_instrumentation_subscribe</iface> to implement instrumentation.
</add-type> */
typedef void (*smm_switch_cb_t)(
        conf_object_t *obj, conf_object_t *cpu,
        lang_void *user_data);

/* <add id="smm_instrumentation_subscribe_interface_t">

   This interface allows a user to keept track of System Management Mode
   switches in the processor.

   The <fun>register_smm_enter_before_cb</fun> registers a callback of type
   <type>smm_switch_cb_t</type> that will be called before the processor
   enters System Management Mode.

   The <fun>register_smm_enter_after_cb</fun> registers a callback of type
   <type>smm_switch_cb_t</type> that will be called after the processor
   has entered System Management Mode.

   The <fun>register_smm_leave_before_cb</fun> registers a callback of type
   <type>smm_switch_cb_t</type> that will be called before the processor
   leaves System Management Mode.

   The <fun>register_smm_leave_after_cb</fun> registers a callback of type
   <type>smm_switch_cb_t</type> that will be called after the processor
   has left System Management Mode.

   The callback in all cases looks like this:
   <insert id="smm_switch_cb_t"/>

   The <arg>obj</arg> argument is the user object that registered the
   callback, or NULL if no such object exists.

   The <arg>cpu</arg> argument is the processor that enters/leaves System
   Management Mode.

   The <arg>user_data</arg> argument is the callback user data, passed to the
   register methods.
   </add>

   <add id="smm_instrumentation_subscribe_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(smm_instrumentation_subscribe) {
        cpu_cb_handle_t *(*register_smm_enter_before_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                smm_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_smm_enter_after_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                smm_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_smm_leave_before_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                smm_switch_cb_t cb,
                lang_void *user_data);
        cpu_cb_handle_t *(*register_smm_leave_after_cb)(
                conf_object_t *cpu,
                conf_object_t *connection,
                smm_switch_cb_t cb,
                lang_void *user_data);
};
#define SMM_INSTRUMENTATION_SUBSCRIBE_INTERFACE \
        "smm_instrumentation_subscribe"
// ADD INTERFACE smm_instrumentation_subscribe

typedef enum {
        X86_Stream_Instruction_LA        = CPU_Stream_Local,
        X86_Stream_Instruction_LA_After,
        X86_Stream_Read_Memory,
        X86_Stream_Write_Memory,
        X86_Stream_Last
} x86_stream_enum_t;

#if defined(__cplusplus)
}
#endif

#endif
