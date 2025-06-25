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

#ifndef DOCEA_OBS2_TYPES_H
#define DOCEA_OBS2_TYPES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

// The status type returned by functions of the corresponding interface.
// Value 0 means success, any other value denotes an error.
typedef uint64 docea_obs2_status_t;

// The result type returned by functions that do not return a value.
// A status code 0 means success, any other value denotes an error.
typedef struct {
  docea_obs2_status_t status;
} docea_obs2_status_result_t;
SIM_PY_ALLOCATABLE(docea_obs2_status_result_t);

// The result type putting together a floating point value and a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_obs2_status_t status;
  double value;
} docea_obs2_double_result_t;
SIM_PY_ALLOCATABLE(docea_obs2_double_result_t);

// A representation of the physical simulation time. This is a number of
// picoseconds.
typedef uint64 docea_obs2_time_t;

// The result type putting together an unsigned integer value and a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
typedef struct {
  docea_obs2_status_t status;
  uint64 value;
} docea_obs2_uint_result_t;
SIM_PY_ALLOCATABLE(docea_obs2_uint_result_t);

// The result type putting together a NULL-terminated character string value and
// a status code.
// A status code 0 means success, any other value denotes an error. In case of
// error, any other value in the result is meaningless.
// The ownership of the character string is transferred to the caller of the
// function returning this result. The caller will be in charge of freeing the
// string.
typedef struct {
  docea_obs2_status_t status;
  char *value;
} docea_obs2_string_result_t;
SIM_PY_ALLOCATABLE(docea_obs2_string_result_t);

// The types of the callback functions that can be registered are defined below.
// Each callback function accepts a private data context (the one specified by the
// caller when registering the callback), plus the arguments that are relevant
// to the event that triggers the callback.
// Each callback type comes with two variants: the first is compatible with
// languages supported by Simics, namely Python, the second (the "_c" variant)
// is specific to the C language.

// The type of callback to register for notifications of newly accessible
// results.
//
// time: the time, in picoseconds, since the start of the simulation.
typedef void (*docea_obs2_accessible_result_callback_t)(cbdata_call_t callback_context,
                                                        docea_obs2_time_t time);

// The type of C language callback to register for notifications of newly
// accessible results.
//
// time: the time, in picoseconds, since the start of the simulation.
typedef void (*docea_obs2_accessible_result_callback_c_t)(void *callback_context,
                                                          docea_obs2_time_t time);

// The type of callback to register for cdyn events notifications.
//
// time: the time, in picoseconds, since the start of the simulation.
//
// name: the logical name identifying the core (or other cdyn source) for which
// the cdyn reception event is notified. This callback will be triggered with a
// `name` parameter that has been allocated and whose ownership is transferred
// to the callback.
typedef void (*docea_obs2_cdyn_event_callback_t)(cbdata_call_t callback_context,
                                                 docea_obs2_time_t time,
                                                 char *name,
                                                 double value);

// The type of C language callback to register for cdyn events notifications.
//
// time: the time, in picoseconds, since the start of the simulation.
//
// name: the logical name identifying the core (or other cdyn source) for which
// the cdyn reception event is notified. This callback will be triggered with a
// `name` parameter that has been allocated and whose ownership is transferred
// to the callback.
typedef void (*docea_obs2_cdyn_event_callback_c_t)(void *callback_context,
                                                   docea_obs2_time_t time,
                                                   char *name,
                                                   double value);

#ifdef __cplusplus
}
#endif

#endif
