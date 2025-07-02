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

#ifndef SIMICS_UTIL_ENCODING_H
#define SIMICS_UTIL_ENCODING_H

#include <simics/util/os.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef _WIN32
typedef WCHAR os_char_t;
#else /* !_WIN32 */
typedef char os_char_t;
#endif

void initialize_encoding(void);

EXPORTED os_char_t *simics_to_filename(const char *ustr);

#if defined(__cplusplus)
}
#endif

#endif
