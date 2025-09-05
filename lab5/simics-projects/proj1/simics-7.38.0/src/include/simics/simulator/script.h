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

#ifndef SIMICS_SIMULATOR_SCRIPT_H
#define SIMICS_SIMULATOR_SCRIPT_H

#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED attr_value_t SIM_run_command(const char *NOTNULL line);

EXPORTED void SIM_run_command_file(const char *NOTNULL file, bool local);
EXPORTED void SIM_run_command_file_params(const char *NOTNULL file, bool local,
                                          attr_value_t params);

EXPORTED attr_value_t SIM_load_target(const char *NOTNULL target,
                                      const char *ns, attr_value_t presets,
                                      attr_value_t cmdline_args);

EXPORTED void VT_load_target_preset_yml(const char *NOTNULL target,
                                        const char *ns,
                                        attr_value_t presets,
                                        const char *preset_yml);

EXPORTED bool SIM_get_batch_mode(void);

EXPORTED void VT_interrupt_script(bool user);

#if defined(__cplusplus)
}
#endif

#endif
