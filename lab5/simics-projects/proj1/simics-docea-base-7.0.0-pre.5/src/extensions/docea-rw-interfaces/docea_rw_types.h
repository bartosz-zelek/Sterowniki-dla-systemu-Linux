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

#ifndef DOCEA_RW_TYPES_H
#define DOCEA_RW_TYPES_H

#include <simics/device-api.h>

#ifdef __cplusplus
extern "C" {
#endif

// The status type retourned by functions of the Docea read/write interfaces.
// Value 0 means success, any other value denotes an error.
typedef uint64 docea_rw_status_t;

// The result type returned by functions that do not return any specific value.
typedef struct {
  docea_rw_status_t status;
} docea_rw_status_result_t;
SIM_PY_ALLOCATABLE(docea_rw_status_result_t);

// The result type of functions that return a numeric value of type float.
// It gathers the result status, the result value and the result actual time in
// picoseconds, that is, the time when the value could be computed by the
// simulator.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_rw_status_t status;
  double value;
  uint64 time_ps;
} docea_rw_double_timed_result_t;
SIM_PY_ALLOCATABLE(docea_rw_double_timed_result_t);

#ifdef __cplusplus
}
#endif

#endif
