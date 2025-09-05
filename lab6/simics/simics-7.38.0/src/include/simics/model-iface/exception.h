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

#ifndef SIMICS_MODEL_IFACE_EXCEPTION_H
#define SIMICS_MODEL_IFACE_EXCEPTION_H

#include <simics/base/types.h>
#include <simics/base/attr-value.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="exception_interface_t">

   The <iface>exception</iface> interface is used to translate
   exception numbers, as received by the Core_Exception hap, to names,
   and vice versa.

   The <fun>get_number</fun> function returns the number associated
   with an exception name, or -1 if the no exception with the given
   name exist. The <fun>get_name</fun> returns the name
   associated with an exception number. The <fun>get_source</fun>
   function is only used on X86 targets and returns the source for an
   exception, as an exception number can be raised from different
   sources. The <fun>all_exceptions</fun> function returns a list of
   all exceptions numbers.

   The exception numbers are architecturally defined, while their
   names are defined by the model.

   <insert-until text="// ADD INTERFACE exception_interface"/>
   
   </add>
   <add id="exception_interface_exec_context">
   Cell Context for all methods.
   </add> */
SIM_INTERFACE(exception) {
        int (*get_number)(conf_object_t *NOTNULL obj,
                          const char *NOTNULL name);
        const char *(*get_name)(conf_object_t *NOTNULL obj, int exc);
        int (*get_source)(conf_object_t *NOTNULL obj, int exc);
        attr_value_t (*all_exceptions)(conf_object_t *NOTNULL obj);
};

#define EXCEPTION_INTERFACE "exception"
// ADD INTERFACE exception_interface

#if defined(__cplusplus)
}
#endif

#endif
