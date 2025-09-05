/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2023-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/

#ifndef DOCEA_SESSION_TYPES_H
#define DOCEA_SESSION_TYPES_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#ifdef __cplusplus
extern "C" {
#endif

// The status returned by functions of the Docea "session" interface.
// Value 0 means success, any other value denotes an error.
typedef uint64 docea_session_status_t;

// A representation of the simulation time. This is a number of picoseconds.
typedef uint64 docea_session_time_t;

// The result returned by functions of the Docea "session" interface that do not
// return any specific value.
// Besides the status, this result may contain further details in the future.
typedef struct {
  docea_session_status_t status;
} docea_session_status_result_t;
SIM_PY_ALLOCATABLE(docea_session_status_result_t);

// The configuration for a simulation session.
typedef struct {
  docea_session_time_t initial_time_picoseconds;
  bool drop_buffered_events;
} docea_session_configuration_t;
SIM_PY_ALLOCATABLE(docea_session_configuration_t);

#ifdef __cplusplus
}
#endif

#endif
