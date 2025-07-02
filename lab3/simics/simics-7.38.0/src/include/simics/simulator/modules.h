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

#ifndef SIMICS_SIMULATOR_MODULES_H
#define SIMICS_SIMULATOR_MODULES_H

#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED attr_value_t SIM_get_all_modules(void);
EXPORTED attr_value_t SIM_get_all_failed_modules(void);
EXPORTED void SIM_add_module_dir(const char *NOTNULL path);

EXPORTED void SIM_module_list_refresh(void);

EXPORTED void SIM_load_module(const char *NOTNULL module);

#if defined(__cplusplus)
}
#endif

#endif
