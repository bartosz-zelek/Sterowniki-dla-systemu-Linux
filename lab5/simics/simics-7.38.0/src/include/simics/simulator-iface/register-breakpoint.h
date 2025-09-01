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

#ifndef SIMICS_SIMULATOR_IFACE_REGISTER_BREAKPOINT_H
#define SIMICS_SIMULATOR_IFACE_REGISTER_BREAKPOINT_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif


/* <add id="register_breakpoint_interface_t"> This is an optional CPU interface
   allowing the execution to break upon any register change, both explicit by
   the software and implicit changes by the model itself.

   The <fun>add_breakpoint</fun> function adds a new breakpoint. The
   <arg>reg_name</arg> is the register name. When register becomes the
   <arg>value</arg>, simulator will stop. The <arg>break_upon_change</arg>
   means whether simulator should stop upon change of the register value. In
   this case, <arg>value</arg> is not used. The <arg>mask</arg> can be used
   when only certain bits are of interest.

   The <fun>remove_breakpoint</fun> function removes a breakpoint with a given
   id. If the id is -1, then all breakpoints are removed.

   The function <fun>get_breakpoints</fun> returns a list of defined
   breakpoints. Each breakpoint in the list is described by a tuple:
   (breakpoint_id, register_name, break_value, mask). If the breakpoint is
   triggered upon every register value change, then break_value is NIL.

   <insert-until text="// ADD INTERFACE register_breakpoint_interface"/> </add>

   <add id="register_breakpoint_interface_exec_context">
   Global Context for all methods.
   </add>
*/
SIM_INTERFACE(register_breakpoint) {
        int (*add_breakpoint)(conf_object_t *obj, const char *reg_name,
                              uint64 value, uint64 mask,
                              bool break_upon_change);
        bool (*remove_breakpoint)(conf_object_t *obj, int id);
        attr_value_t (*get_breakpoints)(conf_object_t *obj);
};

#define REGISTER_BREAKPOINT_INTERFACE "register_breakpoint"
// ADD INTERFACE register_breakpoint_interface

#if defined(__cplusplus)
}
#endif

#endif
