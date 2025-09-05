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

#ifndef SIMICS_MODEL_IFACE_PROCESSOR_INFO_H
#define SIMICS_MODEL_IFACE_PROCESSOR_INFO_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/processor/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/*
   <add id="processor_info_interface_t">

   An older version of the processor_info_v2 interface. See processor_info_v2
   for more information.

   <insert-until text="// ADD INTERFACE processor_info_interface_t"/>
   </add>

   <add id="processor_info_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

SIM_INTERFACE(processor_info) {
        tuple_int_string_t (*disassemble)(conf_object_t *obj,
                                          generic_address_t address,
                                          attr_value_t instruction_data,
                                          int sub_operation);
        void (*set_program_counter)(conf_object_t *obj,
                                    logical_address_t pc);
        logical_address_t (*get_program_counter)(conf_object_t *obj);

        physical_block_t (*logical_to_physical)(conf_object_t *obj,
                                                logical_address_t address,
                                                access_t access_type);
        int (*enable_processor)(conf_object_t *obj);
        int (*disable_processor)(conf_object_t *obj);
        int (*get_enabled)(conf_object_t *obj);

        cpu_endian_t (*get_endian)(conf_object_t *obj);
        conf_object_t *(*get_physical_memory)(conf_object_t *obj);

        int (*get_logical_address_width)(conf_object_t *obj);
        int (*get_physical_address_width)(conf_object_t *obj);

        const char *(*architecture)(conf_object_t *obj);
};

#define PROCESSOR_INFO_INTERFACE "processor_info"
// ADD INTERFACE processor_info_interface_t

/*
   <add id="processor_info_v2_interface_t">

   The <iface>processor_info_v2</iface> interface is implemented by
   processors models. The interface has processor generic functions
   that are architecture independent.

   The <fun>disassemble</fun> function returns the disassemble string for an
   instruction at <param>address</param> with opcode according to
   <param>instruction_data</param>. The <param>instruction_data</param> is an
   <em>attr_value_t</em> value of <em>data</em> type with the bytes of the
   opcode. The bytes are in the same order as they are stored in memory. For
   VLIW architectures, <param>sub_operation</param> is used to select which
   sub-operation to disassemble. The sub-operations start at zero, and a
   request for the entire unit including all sub-operations is encoded with
   sub-operation -1. A request for a sub-operation that is not present (for
   example when <param>sub-operation</param> is neither 0 nor -1 for non-VLIW
   architectures) results in the integer part of the return tuple being set to
   zero. If successful, the function should return a tuple with the size of the
   instruction in bytes and the disassembly string. The disassembly string
   should be allocated with MM_MALLOC or similar and is to be freed by the
   caller. If more bytes are needed, then the function should indicate that by
   returning a negative number in the tuple where the absolute value of the
   number is the required number of bytes. The string should be NULL if more
   bytes are needed. The implementor of <iface>processor_info_v2</iface> is
   allowed to request one additional byte at a time until enough bytes are
   passed to determine what the instruction is. Illegal instructions should
   still result in a valid returned tuple, where the integer part will be used
   by the disassemble command to skip that many bytes before disassembling the
   next instruction. The <param>address</param> can be used to display absolute
   destinations of program counter relative branches.

   The <fun>set_program_counter</fun> function sets the program
   counter in the processor. The <fun>get_program_counter</fun>
   function returns the current program counter.

   The <fun>logical_to_physical</fun> function translates a logical
   <param>address</param> to a physical address of the type defined by
   <param>access_type</param>. The function returns a <em>physical_block_t</em>
   struct with <param>valid</param> bit and the <param>address</param>. The
   address is valid when the valid bit is not <tt>0</tt>. The
   <fun>logical_to_physical</fun> function also returns
   <param>block_start</param> and <param>block_end</param>. The start and end
   of a block has the same logical to physical transformation as the translated
   address. The range is inclusive, so block_end should be the
   address of the last byte of the block.
   This information can be used to figure out how often the
   logical_to_physical function needs to be called. An implementation would
   typically return the page start and end here, but it is free to return any
   power of 2 sized block as long as it includes the translated address.

   The current operating mode of the processor is returned with
   <fun>get_processor_mode</fun>.

   The processor can be enabled or disabled with the
   <fun>enable_processor</fun> or <fun>disable_processor</fun>
   functions. The functions should return <tt>0</tt> if the processor
   changed from enabled to disabled or from disabled to enabled, and
   <tt>1</tt> if the processor did not change state. The current state
   is returned by the <fun>get_enabled</fun> function. Enabled or
   disabled here refers to the state that the user of the model has
   put the processor into. In particular, it is independent of the
   power mode of the processor. A processor that has powered down does
   not count as disabled in this sense, nor does the
   <fun>enable_processor</fun> wake up a processor that is in
   a power-saving sleep state.

   The endianness of the processor is returned by the
   <fun>get_endian</fun> function.

   The physical memory object is returned by the
   <fun>get_physical_memory</fun> function. The object returned by
   <fun>get_physical_memory</fun> is used to set breakpoints by the
   global <cmd>break</cmd> command, and to read and write physical
   memory through <cmd>set</cmd>, <cmd>get</cmd>,
   <cmd>load-binary</cmd>, <cmd>load-file</cmd>, and the default
   implementation of <cmd>disassemble</cmd>. The object returned
   implements the <iface>memory_space</iface> and
   <iface>breakpoint</iface> interfaces. The
   <iface>memory_space</iface> interface for the returned object is
   only be used in inquiry mode corresponding to actions by the
   simulator itself rather than by the simulated software. An
   implementation may return NULL from this method, which will lead to
   the command listed above not being supported when such a processor
   is selected.

   The <fun>get_logical_address_width</fun> function returns the
   number of logical/virtual address bits and the
   <fun>get_physical_address_width</fun> function returns the number
   of physical address bits.

   The processor architecture is returned by calling the
   <fun>architecture</fun> function. The architecture should be one of
   <tt>arm</tt>, <tt>mips32</tt>,
   <tt>mips64</tt>, <tt>ppc32</tt>, <tt>ppc64</tt>, <tt>sparc-v8</tt>,
   <tt>sparc-v9</tt>, <tt>x86</tt>, <tt>x86-64</tt>, or something else
   if none of the listed is a good match.

   All functions in the interface are optional. Each function can be
   set to NULL if it is not supported.

   <insert-until text="// ADD INTERFACE processor_info_v2_interface_t"/>

   Note that the original version of this interface
   (<iface>processor_info</iface>) must also be implemented. The only
   difference between the two interfaces is that the original version lacks the
   <fun>get_processor_mode</fun> function.

   </add>

   <add id="processor_info_v2_interface_exec_context">
   <table border="false">
   <tr><td><fun>disassemble</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>set_program_counter</fun></td>
           <td>Global Context (with some additions; see below)</td></tr>
   <tr><td><fun>get_program_counter</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>logical_to_physical</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_processor_mode</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>enable_processor</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>disable_processor</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_enabled</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_endian</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_physical_memory</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_logical_address_width</fun></td>
       <td>Cell Context</td></tr>
   <tr><td><fun>get_physical_address_width</fun></td>
       <td>Cell Context</td></tr>
   <tr><td><fun>architecture</fun></td><td>Cell Context</td></tr>
   </table>

   It is explicitly permitted to call <fun>set_program_counter</fun> from
   inside an execution breakpoint handler.
   </add>
*/

SIM_INTERFACE(processor_info_v2) {
        tuple_int_string_t (*disassemble)(conf_object_t *obj,
                                          generic_address_t address,
                                          attr_value_t instruction_data,
                                          int sub_operation);
        void (*set_program_counter)(conf_object_t *obj,
                                    logical_address_t pc);
        logical_address_t (*get_program_counter)(conf_object_t *obj);
        physical_block_t (*logical_to_physical)(conf_object_t *obj,
                                                logical_address_t address,
                                                access_t access_type);
        processor_mode_t (*get_processor_mode)(conf_object_t *obj);
        int (*enable_processor)(conf_object_t *obj);
        int (*disable_processor)(conf_object_t *obj);
        int (*get_enabled)(conf_object_t *obj);

        cpu_endian_t (*get_endian)(conf_object_t *obj);
        conf_object_t *(*get_physical_memory)(conf_object_t *obj);

        int (*get_logical_address_width)(conf_object_t *obj);
        int (*get_physical_address_width)(conf_object_t *obj);

        const char *(*architecture)(conf_object_t *obj);
};

#define PROCESSOR_INFO_V2_INTERFACE "processor_info_v2"
// ADD INTERFACE processor_info_v2_interface_t

/*
   <add id="processor_endian_interface_t">

   This interface is used for retrieving endianness and amends to the
   <iface>processor_info_v2</iface> interface.

   Many modern processors support mixed endian as well as separate data
   and instruction endianness. This interface reports endianness separately
   for data and instructions dynamically, not just the default as for the
   <iface>processor_info_v2</iface>. Previously endianness has been static,
   with only one endianness. With newer ARM processors this may cause issues
   for some Big Endian use cases since Little Endian is assumed throughout.
   Primarily due to the fact that they can have separate data and instruction
   endianness. Modifying the existing <iface>processor_info_v2</iface> easily
   gets complicated due to dependencies, so a new interface
   <iface>processor_endian</iface> was created.

   The <iface>processor_endian_interface_t</iface> interface can be
   implemented by processors models and returns the current endianness
   of the system.

   The function <fun>get_instruction_endian</fun> returns the active instruction
   endianness of the processor.

   The function <fun>get_data_endian</fun> returns endianness of data.

   <insert-until text="// ADD INTERFACE processor_endian_interface_t"/>

   </add>

   <add id="processor_endian_interface_exec_context">
   <table border="false">
   <tr><td><fun>get_instruction_endian</fun></td><td>Cell Context</td></tr>
   <tr><td><fun>get_data_endian</fun></td><td>Cell Context</td></tr>
   </table>

   </add>
*/

SIM_INTERFACE(processor_endian) {
        cpu_endian_t (*get_instruction_endian)(conf_object_t *obj);
        cpu_endian_t (*get_data_endian)(conf_object_t *obj);
};

#define PROCESSOR_ENDIAN_INTERFACE "processor_endian"
// ADD INTERFACE processor_endian_interface_t

/*
   <add id="processor_cli_interface_t">

   Some commands and features in the CLI use the
   <iface>processor_cli</iface> interface. Those commands will have
   limited functionality if the interface is not fully implemented.

   The first argument to each function is the object to act on. This object
   should implement both the <iface>processor_info</iface> interface and the
   <iface>processor_cli</iface> interface.

   The <fun>get_disassembly</fun> function is used for the
   <cmd>disassemble</cmd> command as well as to disassemble the next
   instruction to be executed, when control is returned to the CLI prompt. For
   most architectures, <fun>get_disassembly</fun> can be set to NULL, in which
   case the command will use other interfaces to provide a generic
   disassembly. The <fun>get_disassembly</fun> function should return a tuple
   with the length of the instruction in bytes and the disassembly string. The
   <param>addr_prefix</param> parameter selects the address type of the address
   parameter, whether it is a physical address ("p"), a linear address ("l") or
   a virtual address ("v"), just as returned from
   <fun>get_address_prefix</fun>. The <param>address</param> parameter is the
   program counter for the instruction to disassemble. If
   <param>print_cpu</param> is non-zero, then the name of the processor should
   be included first in the disassembly line. If <param>mnemonic</param> is not
   NULL, then it should be output instead of the instruction disassemble. The
   mnemonic is used to print exception or interrupt information as returned by
   the <fun>get_pending_exception_string</fun> function.

   <fun>get_pregs</fun> returns the string to output in the CLI for the
   <cmd>print-processor-registers</cmd> command. The <param>all</param>
   parameter is a boolean corresponding to the <param>-all</param> switch to the
   <cmd>print-processor-registers</cmd> command.

   The <fun>diff_regs</fun> function is used by the <cmd>stepi</cmd>
   command when the <param>-r</param> flag is used. The
   <fun>diff_regs</fun> function returns a list of register names,
   where each register in that list will be read through the
   <iface>int_register</iface> interface before and after an
   instruction.

   When returning to the CLI prompt, information about the next
   instruction or step to execute is printed. Normally, that is the
   disassemble of the instruction at the current program counter. The
   <fun>get_pending_exception_string</fun> function is called before
   the disassembly to find out if the next step will not be an
   instruction, but rather a taken exception or interrupt. The
   function should inspect the given <param>cpu</param> (an object
   implementing <iface>processor_info</iface> and
   <iface>processor_cli</iface>) and return NULL if the next step will
   be the execution of the instruction at the current program
   counter. If the next step will instead be the handling of an
   exception or interrupt, then a string saying that should be
   returned.

   The <fun>get_address_prefix</fun> function returns a string with
   the default address prefix for memory related commands. Simics
   defines the generic prefixes "v" for virtual addresses, "l" for
   linear addresses, and "p" for physical addresses. The default if
   <fun>get_address_prefix</fun> is NULL is "v" for virtual addresses.

   <fun>translate_to_physical</fun> translates an address to a
   physical address. If <fun>translate_to_physical</fun> is NULL, then
   the only allowed address prefixes are "v" (virtual) and "p"
   (physical), and the <fun>logical_to_physical</fun> function in the
   <iface>processor_info</iface> interface will be used to translate
   virtual addresses.

   <insert-until text="// ADD INTERFACE processor_cli_interface_t"/>
   </add>
   <add id="processor_cli_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(processor_cli) {
	tuple_int_string_t (*get_disassembly)(conf_object_t *obj,
                                              const char *addr_prefix,
                                              generic_address_t address,
                                              bool print_cpu,
                                              const char *mnemonic);
	char *(*get_pregs)(conf_object_t *cpu,
                           bool all);
	attr_value_t (*get_diff_regs)(conf_object_t *obj);
	char *(*get_pending_exception_string)(conf_object_t *obj);
	char *(*get_address_prefix)(conf_object_t *obj);
	physical_block_t (*translate_to_physical)(conf_object_t *obj,
                                                  const char *prefix,
                                                  generic_address_t address);
};

#define PROCESSOR_CLI_INTERFACE "processor_cli"
// ADD INTERFACE processor_cli_interface_t

/*
   <add id="opcode_info_interface_t">

   The <iface>opcode_info</iface> interface is implemented by
   processors that need to communicate information about the encoding
   of instructions to the GUI.

   The <fun>get_opcode_length</fun> function returns information about
   instruction encoding in the current operating mode of the
   processor. The <fun>min_alignment</fun> field indicates the
   smallest allowed alignment of instructions, typically 4 for regular
   RISC architectures. The <fun>max_length</fun> field specifies the
   maximum instruction length in bytes. The <fun>avg_length</fun> is
   an approximation of the average instruction size.

   <insert-until text="// ADD INTERFACE opcode_info_interface_t"/>
   </add>
   <add id="opcode_info_interface_exec_context">
   Cell Context for all methods.
   </add>
*/

typedef struct {
        int min_alignment;
        int max_length;
        int avg_length;
} opcode_length_info_t;

SIM_INTERFACE(opcode_info) {
        opcode_length_info_t (*get_opcode_length_info)(conf_object_t *obj);
};

#define OPCODE_INFO_INTERFACE "opcode_info"
// ADD INTERFACE opcode_info_interface_t

/*
   <add id="processor_gui_interface_t">

   The <iface>processor_gui</iface> interface is implemented by
   processors that support displays in the Simics native GUI. It is
   only registered to indicate support for the displays, and does not
   contain any actual functionality.

   <insert-until text="// ADD INTERFACE processor_gui_interface_t"/>
   </add>
   <add id="processor_gui_interface_exec_context">
   There are no methods in this interface.
   </add>
*/

SIM_INTERFACE(processor_gui) {
        void (*dummy)(conf_object_t *obj);
};

#define PROCESSOR_GUI_INTERFACE "processor_gui"
// ADD INTERFACE processor_gui_interface_t

#if defined(__cplusplus)
}
#endif

#endif
