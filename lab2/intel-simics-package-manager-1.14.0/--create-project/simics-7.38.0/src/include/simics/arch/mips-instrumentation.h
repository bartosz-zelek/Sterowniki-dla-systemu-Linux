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

#ifndef SIMICS_ARCH_MIPS_INSTRUMENTATION_H
#define SIMICS_ARCH_MIPS_INSTRUMENTATION_H

#include <simics/base/types.h>
#include <simics/pywrap.h>
#include <simics/model-iface/cpu-instrumentation.h>

#if defined(__cplusplus)
extern "C" {
#endif
        
/* <add id="mips_exception_query_interface_t"> 

   Used to query information of an exception from a
   <fun>cpu_exception_cb_t</fun> callback.
   <insert-until text="// ADD INTERFACE mips_exception_query"/>
   
   <fun>pc</fun> is used to get the address of the faulting instruction.
   <fun>return_pc</fun> is used to get the return address for the exception.
   <fun>number</fun> is used to get the exception number.
   </add>

   <add id="mips_exception_query_interface_exec_context">
   Cell Context for all methods.
   </add>
*/
SIM_INTERFACE(mips_exception_query) {
        logical_address_t (*return_pc)(conf_object_t *cpu,
                                       exception_handle_t *handle);
};

#define MIPS_EXCEPTION_QUERY_INTERFACE "mips_exception_query"
// ADD INTERFACE mips_exception_query

#if defined(__cplusplus)
}
#endif

#endif
