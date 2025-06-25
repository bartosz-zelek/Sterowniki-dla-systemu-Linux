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

#ifndef SIMICS_MODEL_IFACE_DECODER_H
#define SIMICS_MODEL_IFACE_DECODER_H

#include <simics/base/types.h>
#include <simics/base/memory.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="service routine function">
   <insert-upto text=";"/><ndx>service_routine_t</ndx>
   </add>
*/
typedef exception_type_t (*service_routine_t)(conf_object_t *cpu, 
                                              uint64 arg,
                                              lang_void *user_data);

/* <add id="instruction info">
   <insert-upto text="instruction_info_t;"/>
   <ndx>instruction_info_t</ndx>

   <var>ii_ServiceRoutine</var> is a pointer to a function that will
   be called by Simics every time the instruction is executed. It has
   the following prototype:
   
   <insert id="service routine function"/> 

   The service routine function should return an exception when it is
   finished to signal its status. If no exception occurs
   <const>Sim_PE_No_Exception</const> should be returned.

   See <type>exception_type_t</type> in
   <file>src/include/simics/base/memory.h</file> for the different
   exceptions available.

   A special return value, <const>Sim_PE_Default_Semantics</const>, can be
   returned; this signals Simics to run the default semantics for the
   instruction. This is useful if the semantics of an instruction
   should be changed but the user routine does not want to handle it all
   the time.

   Note that in a shared memory multiprocessor, the CPU
   used in decoding may differ from the CPU that executes the
   instruction, since the decoded instructions may be cached.

   <var>ii_Arg</var> is the argument <var>arg</var> that will be
   passed on to the service routine function. Op code bit-fields for
   the instruction such as register numbers or intermediate values can
   be stored here. The <var>ii_UserData</var> field can also be used
   to pass information to the service routine if more data is needed.

   <var>ii_Type</var> is either <pp>UD_IT_SEQUENTIAL</pp> or
   <pp>UD_IT_CONTROL_FLOW</pp>.  A sequential type means that the
   instruction does not perform any branches and the update of the
   program counter(s) is handled by Simics. In a control flow
   instruction on the other hand it is up to the user to set the
   program counter(s).

   <var>ii_LogicalAddress</var> and <var>ii_PhysicalAddress</var>
   holds the logical and physical addresses of the instruction to be
   decoded.

   </add> */
typedef struct instruction_info {
        service_routine_t  ii_ServiceRoutine;
        uint64             ii_Arg;
        unsigned int       ii_Type;
        lang_void         *ii_UserData;
        logical_address_t  ii_LogicalAddress;
        physical_address_t ii_PhysicalAddress;
} instruction_info_t;

/* <add id="user decoder">
   <insert-upto text="decoder_t;"/>
   <ndx>decoder_t</ndx>

   The <fun>decode</fun> function is called to decode an instruction
   pointed to by <param>code</param>.  The first byte corresponds to
   the lowest address of the instruction in the simulated
   memory. <param>valid_bytes</param> tells how many bytes can be
   read. The CPU is given in the <param>cpu</param> parameter.  When
   the decoder has successfully decoded an instruction, it should set
   the <var>ii_ServiceRoutine</var>, the <var>ii_Arg</var>, and the
   <var>ii_Type</var> members of the <param>ii</param> structure (see
   below), and returns the number of bytes used in the decoding.  If
   it does not apply to the given instruction, it should return zero.
   If the decoder needs more data than <param>valid_bytes</param> it
   should return a negative number corresponding to the total number
   of bytes it will need to continue the decoding. The underlying
   architecture limits the number of bytes that can be requested,
   e.g. no more than 4 bytes can be requested on most RISC
   architectures. Simics will call the decoder again when more bytes
   are available. This process is repeated until the decoder accepts
   or rejects the instruction.  A decoder should never request more
   data than it needs. For example, if an instructions can be rejected
   by looking at the first byte, the decoder should never ask for more
   bytes.

   The <type>instruction_info_t</type> is defined as follows:

   <insert id="instruction info"/>

   The <fun>disassemble</fun> function is called to disassemble an
   instruction.  It uses the same <param>code</param>,
   <param>valid_bytes</param>, and <param>cpu</param> parameters as
   the <fun>decode</fun> function. If the disassembly is valid, then
   the string part of the returned <tt>tuple_int_string_t</tt> struct
   should be a MALLOCed string with the disassembly and the integer
   part should be its length in bytes.  The caller is responsible for
   freeing the disassembly string. The string member should be NULL
   and the integer part should be zero if the disassembly is not
   valid.  If the disassemble function needs more data than
   <param>valid_bytes</param> it should return a negative number in
   the integer part in the same way as the <fun>decode</fun> function,
   and set the string part to NULL.

   The <fun>flush</fun> function is called to free any memory
   allocated when decoding an instruction and any user data associated
   with the instruction.  It should return zero if it does not
   recognize the instruction, and non-zero if it has accepted it.
   Usually, the way to recognize if a decoded instruction is the right
   one to flush is to compare <tt>ii->ii_ServiceRoutine</tt> with the
   function that was set in the <param>decode</param> function. Note
   that the <param>cpu</param> parameter is the processor that caused
   the flush. It is more or less an arbitrary processor and should be
   ignored.

   In addition to the function pointers, the
   <type>decoder_t</type> structure contains a
   <var>user_data</var> pointer that is passed to all the
   functions.  This can be used for passing any data to the decoder
   functions.

   </add> */
typedef struct {
        void *user_data;
        int (*NOTNULL decode)(uint8 *code,
                              int valid_bytes,
                              conf_object_t *cpu,
                              instruction_info_t *ii,
                              void *user_data);
        tuple_int_string_t (*NOTNULL disassemble)(uint8 *code,
                                                  int valid_bytes,
                                                  conf_object_t *cpu,
                                                  void *user_data);
        int (*NOTNULL flush)(instruction_info_t *ii,
                             void *user_data);
} decoder_t;

/* <add id="decoder_interface_t">
   <ndx>decoder_interface_t</ndx>

   <insert-upto text="};"/>

   The <iface>decoder</iface> interface is implemented by processors
   that allows connecting user decoders. This allows a user to
   implement the semantics of instructions that are not available in
   the standard Simics model or change the semantics of instructions
   implemented by Simics. This interface replaces
   <fun>SIM_register_arch_decoder</fun> and
   <fun>SIM_unregister_arch_decoder</fun> functions.

   The <fun>register_decoder</fun> function adds a decoder and
   <fun>unregister_decoder</fun> removes a decoder.

   The decoder is installed/removed for every object of the same class as the
   <arg>obj</arg> argument which must be the same object from
   which the interface was fetched.

   When Simics decodes an instruction, it will first see if any
   instruction decoders are registered for the current CPU class.
   For any decoders it finds, Simics will let it try to decode the
   instruction.  The decoders are called in order, starting with the
   last registered decoder, and if one decoder accepts the instruction,
   the rest of the decoders will not be called.

   The decoder is specified by the <tt>decoder_t</tt> data structure that the 
   user supplies:

   <insert id="user decoder"/>

   </add>
   <add id="decoder_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(decoder) {
        void (*register_decoder)(conf_object_t *obj, 
                                 decoder_t *NOTNULL decoder);
        void (*unregister_decoder)(conf_object_t *obj, 
                                   decoder_t *NOTNULL decoder);
};

#define DECODER_INTERFACE "decoder"

/* Instruction type */

#define UD_IT_SEQUENTIAL   1
#define UD_IT_CONTROL_FLOW 2

#if defined(__cplusplus)
}
#endif

#endif
