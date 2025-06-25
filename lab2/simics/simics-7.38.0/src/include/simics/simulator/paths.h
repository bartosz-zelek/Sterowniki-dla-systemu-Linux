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

#ifndef SIMICS_SIMULATOR_PATHS_H
#define SIMICS_SIMULATOR_PATHS_H

#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED char *SIM_native_path(const char *NOTNULL path);

EXPORTED char *SIM_lookup_file(const char *file);

EXPORTED void SIM_add_directory(const char *NOTNULL directory, bool prepend);
EXPORTED void SIM_clear_directories(void);
EXPORTED attr_value_t SIM_get_directories(void);

EXPORTED const char *VT_get_saved_cwd(void);
EXPORTED int64 VT_logical_file_size(const char *NOTNULL filename);

#if defined(__cplusplus)
}
#endif

#endif
