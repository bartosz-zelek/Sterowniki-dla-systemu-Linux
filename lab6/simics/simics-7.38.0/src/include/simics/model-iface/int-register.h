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

#ifndef SIMICS_MODEL_IFACE_INT_REGISTER_H
#define SIMICS_MODEL_IFACE_INT_REGISTER_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* used in register_info() interface function */
/* <add id="ireg_info_t"><ndx>ireg_info_t</ndx><insert-upto text=";"/></add> */
typedef enum {
        Sim_RegInfo_Catchable
} ireg_info_t;

/*
   <add id="int_register_interface_t"> The <iface>int_register</iface>
   interface is used for access to registers in a processor.  It
   can be used to access any kind of integer register, not only the
   "normal" registers.  This includes all kinds of control registers,
   hidden registers and anything else that might be useful to access as
   a register.  The only limitation is that the register value should
   be representable as a 64-bit unsigned integer.

   This interface can be implemented by other classes than processors,
   but it is likely to be found mostly in processors.
 
   Registers are identified by a number, and there are two functions
   to translate from register names to register numbers and back.  The
   translation need not be one-to-one, which means that one register
   can have several names.  A register name can, however, only
   translate to a single register number.

   Often, registers are grouped in <i>register banks</i>, where
   registers in the bank are numbered from 0 up. Registers in a bank
   should have consecutive numbers (unless their numbering is very sparse).
   This allows a user to deduce register numbers by calling
   <fun>get_number</fun> for the first register only.
   The first register numbers should be used for the general-purpose integer 
   registers, if possible (so that integer register <b>r</b>N has number N).

   Using this interface to read or write registers does not cause any
   side effects, such as triggering interrupts or signalling haps.

   <b>get_number</b> translates a register name to its number. Returns -1 if
   the register does not exist.

   <b>get_name</b> translates a register number to its canonical name.

   <b>read</b> reads a register value.

   <b>write</b> writes a new register value.

   <b>all_registers</b> returns a list of all register numbers that can
   be used for this object.

   <b>register_info</b> returns information about a single register.
   The information return depends on the <param>info</param> parameter.

   <dl>
   
   <dt>Sim_RegInfo_Catchable</dt><dd>Return 1 if
   <const>Core_Control_Register_Write</const> and
   <const>Core_Control_Register_Read</const> are triggered when this
   register is written or read.</dd>  Return 0 otherwise.

   </dl>

   <small>
   <insert id="ireg_info_t"/>

   <insert-until text="// ADD INTERFACE int_register_interface"/>
   </small>

   </add>
   <add id="int_register_interface_exec_context">
   Cell Context for all methods, except for <fun>write</fun> where the
   register is a program counter; Global Context in that case.
   </add>
*/
SIM_INTERFACE(int_register) {
        int (*get_number)(conf_object_t *NOTNULL obj,
                          const char *NOTNULL name);
        const char *(*get_name)(conf_object_t *NOTNULL obj, int reg);
        uint64 (*read)(conf_object_t *NOTNULL obj, int reg);
        void (*write)(conf_object_t *NOTNULL obj, int reg, uint64 val);
        attr_value_t (*all_registers)(conf_object_t *NOTNULL obj);
        int (*register_info)(conf_object_t *NOTNULL obj, int reg,
                             ireg_info_t info);
};

#define INT_REGISTER_INTERFACE "int_register"
// ADD INTERFACE int_register_interface

#if defined(__cplusplus)
}
#endif

#endif
