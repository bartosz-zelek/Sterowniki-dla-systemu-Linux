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

#ifndef SIMICS_SIMULATOR_OUTPUT_PR_ERR_H
#define SIMICS_SIMULATOR_OUTPUT_PR_ERR_H

#include <stdarg.h>

#include <simics/host-info.h>
#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(PYWRAP)
EXPORTED void pr_err(const char *str, ...) PRINTF_FORMAT(1, 2);
EXPORTED void pr_warn(const char *str, ...) PRINTF_FORMAT(1, 2);
EXPORTED void SIM_printf_error(const char *str, ...) PRINTF_FORMAT(1, 2);
EXPORTED void SIM_printf_error_vararg(const char *format,
                                      va_list ap) PRINTF_FORMAT(1, 0);
EXPORTED void SIM_printf_warning(const char *str, ...) PRINTF_FORMAT(1, 2);
EXPORTED void SIM_printf_warning_vararg(const char *format,
                                        va_list ap) PRINTF_FORMAT(1, 0);
#endif /* ! PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
