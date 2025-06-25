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

#ifndef DOCEA_MODEL_OBS_TYPES_H
#define DOCEA_MODEL_OBS_TYPES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

// The status type returned by functions of the corresponding interface.
// Value 0 means success, any other value denotes an error.
typedef uint64 docea_model_obs_status_t;

// The result type putting together a floating point value and a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_model_obs_status_t status;
  double value;
} docea_model_obs_double_result_t;
SIM_PY_ALLOCATABLE(docea_model_obs_double_result_t);

// The result type putting together a boolean value and a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_model_obs_status_t status;
  bool value;
} docea_model_obs_bool_result_t;
SIM_PY_ALLOCATABLE(docea_model_obs_bool_result_t);

// The result type putting together a NULL-terminated character string value and
// a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_model_obs_status_t status;
  char *value;
} docea_model_obs_string_result_t;
SIM_PY_ALLOCATABLE(docea_model_obs_string_result_t);

// A representation of the physical simulation time. This is a number of
// picoseconds.
typedef uint64 docea_model_obs_time_t;

#ifdef __cplusplus
}
#endif

#endif
