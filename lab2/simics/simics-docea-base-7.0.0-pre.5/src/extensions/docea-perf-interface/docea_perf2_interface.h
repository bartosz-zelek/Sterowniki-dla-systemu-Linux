/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2021-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/

#ifndef DOCEA_PERF2_INTERFACE_H
#define DOCEA_PERF2_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_perf2_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_perf2) {
  // Update the cdyn of a core with the given value applied until the given
  // time.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the time, in picoseconds, until when the cdyn had the given value.
  //
  // name: the logical name identifying the core.
  //
  // value: the cdyn value.
  docea_perf2_status_result_t (*set_past_cdyn)(conf_object_t * obj,
                                               docea_perf2_time_t time,
                                               const char* name,
                                               double value);
};

#define DOCEA_PERF2_INTERFACE "docea_perf2"

#ifdef __cplusplus
}
#endif

#endif
