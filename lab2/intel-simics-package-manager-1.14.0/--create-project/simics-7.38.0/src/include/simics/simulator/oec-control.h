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

#ifndef SIMICS_SIMULATOR_OEC_CONTROL_H
#define SIMICS_SIMULATOR_OEC_CONTROL_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Make sure that Simics is not running when this function is called, and take
   the necessary steps depending on the situation */
EXPORTED void VT_assert_outside_execution_context(
        const char *func, const char *file, int line);

#ifdef SIMICS_CORE
/* Inside Simics Core, function name alone is sufficient identification */
#define ASSERT_OUTSIDE_EXECUTION_CONTEXT() \
        VT_assert_outside_execution_context(__func__, NULL, 0)
#else
#define ASSERT_OUTSIDE_EXECUTION_CONTEXT() \
        VT_assert_outside_execution_context(__func__, __FILE__, __LINE__)
#endif

/* Return true if VT_assert_outside_execution_context() would warn or assert in
   the current situation. This function is meant to be used before calling
   VT_assert_outside_execution_context() to print specific warnings before
   asserting. */
EXPORTED bool VT_outside_execution_context_violation(void);

#if defined(__cplusplus)
}
#endif

#endif
