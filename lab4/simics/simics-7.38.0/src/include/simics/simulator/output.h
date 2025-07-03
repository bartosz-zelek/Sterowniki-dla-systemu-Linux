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

#ifndef SIMICS_SIMULATOR_OUTPUT_H
#define SIMICS_SIMULATOR_OUTPUT_H

#include <simics/base/types.h>

#include <simics/simulator/output-pr-err.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(PYWRAP)

EXPORTED int SIM_write(const void *NOTNULL src, int length);
EXPORTED int SIM_flush(void);
EXPORTED int SIM_puts(const char *NOTNULL s);
EXPORTED int SIM_putchar(int c);

EXPORTED int SIM_printf(const char *NOTNULL format, ...) PRINTF_FORMAT(1, 2);

/* pr is just an alias for SIM_printf */
EXPORTED int pr(const char *format, ...) PRINTF_FORMAT(1, 2);

EXPORTED int SIM_printf_vararg(const char *format, va_list ap)
        PRINTF_FORMAT(1, 0);

#endif /* ! PYWRAP */

typedef void (*output_handler_t)(lang_void *data,
                                 const char *src, size_t length);

EXPORTED void SIM_add_output_handler(output_handler_t NOTNULL func,
                                     lang_void *user_data);
EXPORTED void SIM_remove_output_handler(output_handler_t NOTNULL func,
                                        lang_void *user_data);

EXPORTED void SIM_set_quiet(bool quiet);
EXPORTED bool SIM_get_quiet(void);
EXPORTED void SIM_set_verbose(bool verbose);
EXPORTED bool SIM_get_verbose(void);

EXPORTED void VT_real_network_warnings(void);

EXPORTED bool VT_use_ipv4(void);

#if defined(__cplusplus)
}
#endif

#endif
