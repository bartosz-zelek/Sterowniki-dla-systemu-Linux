/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_UTIL_INIT_H
#define SIMICS_UTIL_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

void init_vtutils(void);
void vtutils_set_low_memory_handler(void (*f)(void));
void vtutils_set_assert_error_handler(void (*f)(int line, const char *file,
                                                const char *mod_date,
                                                const char *message));
void vtutils_set_fatal_error_handler(void (*f)(const char *msg));

#ifdef __cplusplus
}
#endif

#endif
