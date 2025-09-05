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

#ifndef SIMICS_BASE_VERSION_H
#define SIMICS_BASE_VERSION_H

#include <simics/host-info.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED const char *SIM_version(void);
EXPORTED const char *SIM_version_base(void);
EXPORTED const char *SIM_version_major(void);
EXPORTED char *SIM_vmxmon_version(void);
EXPORTED char *SIM_copyright(void);

EXPORTED void SIM_license(void);
EXPORTED char *SIM_license_file(const char *format);

EXPORTED void VT_deprecate(int depr_build_id,
                           const char *NOTNULL warn_msg,
                           const char *NOTNULL ref_msg);

EXPORTED void OLD_copyright(void);

#ifndef PYWRAP

/* <add-type id="api_function_t def"></add-type> */
typedef void (*api_function_t)(void);

EXPORTED api_function_t SIM_get_api_function(const char *function);

#define GET_API_FUNCTION(v, f)                                 \
        typeof(f) *v = (typeof(f) *)SIM_get_api_function(      \
                SYMBOL_TO_STRING(f));
#endif

#if defined(__cplusplus)
}
#endif

#endif
