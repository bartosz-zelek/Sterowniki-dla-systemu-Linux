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

#ifndef SIMICS_SIMULATOR_PYTHON_H
#define SIMICS_SIMULATOR_PYTHON_H

#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED void SIM_source_python(const char *NOTNULL file);
EXPORTED void SIM_source_python_in_module(const char *NOTNULL file,
                                          const char *NOTNULL module);

EXPORTED attr_value_t SIM_run_python(const char *NOTNULL line);
EXPORTED attr_value_t SIM_call_python_function(
        const char *NOTNULL func, attr_value_t *NOTNULL args);

EXPORTED attr_value_t VT_call_python_module_function(
        const char *module, const char *func, attr_value_t *args);

EXPORTED SIM_PYOBJECT *SIM_get_python_interface_type(const char *NOTNULL name);

EXPORTED void VT_python_decref(SIM_PYOBJECT *NOTNULL o);

#if defined(__cplusplus)
}
#endif

#endif
