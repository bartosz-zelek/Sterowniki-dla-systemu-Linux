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

#ifndef SIMICS_SIMULATOR_HOST_PROFILING_H
#define SIMICS_SIMULATOR_HOST_PROFILING_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct profile_area profile_area_t;

EXPORTED profile_area_t *SIM_add_profiling_area(
        const char *NOTNULL name, uintptr_t start, uintptr_t end);

EXPORTED void SIM_remove_profiling_area(profile_area_t *NOTNULL handle);

#if defined(__cplusplus)
}
#endif

#endif
