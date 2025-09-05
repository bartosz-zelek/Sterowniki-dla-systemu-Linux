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

#ifndef SIMICS_PROCESSOR_TIME_H
#define SIMICS_PROCESSOR_TIME_H

#include <simics/base/types.h>
#include <simics/base/time.h>
#include <simics/base/event.h>

#if defined(__cplusplus)
extern "C" {
#endif

EXPORTED void VT_clock_frequency_about_to_change(conf_object_t *NOTNULL obj);
EXPORTED void VT_clock_frequency_change(conf_object_t *NOTNULL obj,
                                        uint64 cycles_per_second);

#if defined(__cplusplus)
}
#endif

#endif
