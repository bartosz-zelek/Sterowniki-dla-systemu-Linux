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

#ifndef DOCEA_PERF3_INTERFACE_H
#define DOCEA_PERF3_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_perf3_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_perf3) {
  // Set the activity of a device to the new value.
  //
  // It is forbidden to change the past: an error is returned if the time is
  // older than the latest time for the same logical name.
  //
  // time: the time, in picoseconds, when the activity changes to become the
  // given value.
  //
  // name: the logical name identifying the device.
  //
  // value: the activity value.
  docea_perf3_status_result_t (*set_activity)(conf_object_t * obj,
                                              docea_perf3_time_t time,
                                              const char* name,
                                              double value);
};

#define DOCEA_PERF3_INTERFACE "docea_perf3"

#ifdef __cplusplus
}
#endif

#endif
