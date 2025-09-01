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

#ifndef SIMICS_SIMULATOR_IFACE_DISASSEMBLE_H
#define SIMICS_SIMULATOR_IFACE_DISASSEMBLE_H

#include <simics/base/types.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add-type id="disasm_instr_t"> </add-type> */
typedef struct {
        int start;          /* Where the instructions starts in the buffer */
        int length;         /* Length of instruction, or -1 if incomplete */
        char *string;       /* Disassembly string (allocated) */
} disasm_instr_t;

/* <add id="disassemble_interface_t">
   The disassemble interface can be used to disassemble code from a
   buffer in memory. It is typically used to disassemble code for the
   host architecture independent of the target architecture
   implemented in a particular version of Simics.

   <insert-until text="// ADD INTERFACE disassemble_interface"/>

   <fun>init()</fun> is used to initialize a new disassemble
   session. You should provide a buffer in <tt>buff</tt>, the buffer
   length in bytes in <tt>buff_len</tt> and the base address for this
   chunk in <tt>address</tt>. The <tt>address</tt> parameter is used
   to calculate program counter relative offsets (for branches and
   other program counter relative constructs).

   <ndx>disasm_instr_t</ndx>
   <insert id="disasm_instr_t"/>

   <fun>next()</fun> returns a structure with the next disassembled
   instruction. Repeated use of <fun>next()</fun> will disassemble
   additional instructions.

   </add>
   <add id="disassemble_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(disassemble) {
        /* Set up new block to disassemble */
        void (*init)(conf_object_t *obj, uint8 *buff,
                     int buff_len, uint64 address);
        /* Disassemble the next instruction */
        disasm_instr_t (*next)(conf_object_t *obj);
};

#define DISASSEMBLE_INTERFACE "disassemble"
// ADD INTERFACE disassemble_interface

#if defined(__cplusplus)
}
#endif

#endif
