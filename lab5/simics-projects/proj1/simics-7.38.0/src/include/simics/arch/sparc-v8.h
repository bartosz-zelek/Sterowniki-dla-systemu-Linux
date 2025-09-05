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

#ifndef SIMICS_ARCH_SPARC_V8_H
#define SIMICS_ARCH_SPARC_V8_H

#include <simics/pywrap.h>
#include <simics/base-types.h>
#include <simics/base/cbdata.h>
#include <simics/base/conf-object.h>
#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="sparc_v8_interface_t">

   <insert-until text="// ADD INTERFACE sparc_v8_interface"/>
   SPARC V8 specific functions.
   
   This interface is implemented by SPARC V8 processors to provide
   various functionality that is specific for this class of processors.
   
   The <fun>read_window_register</fun> and <fun>write_window_register</fun>
   functions can be used to access registers in any register window.
   
   The register number when accessing windowed registers is 0 - 7 for accesses
   to the global registers, 8 - 15 for the out registers of the selected
   window, 16 - 23 for the local registers, and 24 - 31 for the in registers.
   
   The <fun>power_down</fun> function is used to command the processor to enter
   power down mode. During power down mode, the processor will not execute any
   instructions, it will instead fast forward the execution to the next event.
   If the event is an external interrupt the power down mode is exited and
   execution resumed, in other cases the event will be handled and the
   processor will remain in power down mode and fast forward to the next event.
   
   One use for the <fun>power_down</fun> function is for example to have an
   external device such as an memory mapped PMU force the processor into power
   down mode.
   </add>
   <add id="sparc_v8_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(sparc_v8) {
        uint64 (*read_window_register)(conf_object_t *cpu, int window, int reg);

        void (*write_window_register)(conf_object_t *cpu,
                                      int window, int reg, uint64 value);
        void (*power_down)(conf_object_t *cpu);
};
#define SPARC_V8_INTERFACE "sparc_v8"
// ADD INTERFACE sparc_v8_interface

/* <add id="sparc_v8_ecc_fault_injection_interface_t">
   
   <insert-until text="// ADD INTERFACE sparc_v8_ecc_fault_injection"/>

   Support the injection of uncorrectable ECC faults. It can be used to
   inject (1) V8_Exc_Data_Access_Exception,
   (2) V8_Exc_Instruction_Access_Exception, and
   (3) V8_Exc_R_Register_Access_Error. The functions in this interface
   can only be called by breakpoint callback functions. Once an ecc
   fault is injected, new ecc faults can not be injected until the current
   one is handled.
   
   The <fun>inject_data_access_exception</fun> plants an
   V8_Exc_Data_Access_Error for memory access on a location. The fault is
   injected when the breakpoint callback function of the location is called.

   The <fun>inject_instr_access_exception</fun> plants an
   V8_Exc_Instruction_Access_Exception for an instruction, and this fault is
   injected when the breakpoint callback function of the instruction location
   is called.

   The <fun>inject_data_access_exception</fun> is used for injecting
   V8_Exc_R_Register_Access_Error for a register access. It is right now not
   implemented.
   </add>

   <add id="sparc_v8_ecc_fault_injection_interface_exec_context">
   Cell Context for all methods.
   </add>
 */
SIM_INTERFACE(sparc_v8_ecc_fault_injection) {
        void (*inject_instr_access_exception)(conf_object_t *cpu);
        void (*inject_data_access_exception)(conf_object_t *cpu);
        void (*inject_reg_access_error)(conf_object_t *cpu);        
};
#define SPARC_V8_ECC_FAULT_INJECTION_INTERFACE "sparc_v8_ecc_fault_injection"
// ADD INTERFACE sparc_v8_ecc_fault_injection

#if defined(__cplusplus)
}
#endif
#endif
