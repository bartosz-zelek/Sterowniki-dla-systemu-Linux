/******************************************************************************
INTEL CONFIDENTIAL
Copyright 2020-2025 Intel Corporation.

This software and the related documents are Intel copyrighted materials, and
your use of them is governed by the express license under which they were
provided to you (License). Unless the License provides otherwise, you may not
use, modify, copy, publish, distribute, disclose or transmit this software or
the related documents without Intel's prior written permission.

This software and the related documents are provided as is, with no express or
implied warranties, other than those that are expressly stated in the License.
******************************************************************************/

#ifndef DOCEA_PTM_TYPES_H
#define DOCEA_PTM_TYPES_H

#include <stddef.h>

/* This header declares common Docea ISIM types. */

#ifdef __cplusplus
extern "C" {
#endif

/* This macro and the following test are required to ensure that we can safely
 return an uint_least64_t value as a size_t. The Docea native library uses
 uint_least64_t and expects 64bit+ support. */
#define BUILD_BUG_ON(condition) \
  typedef char BUILD_BUG_ON__[(condition) ? -1 : 1]
/* An error at the code line below means that the current platform has
 size_t < uint_least64_t, hence it is not supported. */
BUILD_BUG_ON(sizeof(size_t) < sizeof(uint_least64_t));

/* The type of unique identifiers for object providing or requiring a value,
 e.g. the id of the output of a voltage regulator. */
typedef size_t docea_id_t;

/* The status type returned by functions.
 0 means success, any other value denotes an error. */
typedef size_t docea_status_t;

/* The result type returned by functions that do not return a value.
 A status code 0 means success, any other value denotes an error. */
typedef struct {
  docea_status_t status;
} docea_status_result_t;
SIM_PY_ALLOCATABLE(docea_status_result_t);

/* The result type putting together a floating point value, a status code and
 the actual time when the value could be computed by the Docea simulator.
 A status code 0 means success, any other value denotes an error (hence other
 values in the result are meaningless). */
typedef struct {
  docea_status_t status;
  double value;
  double time;
} docea_double_timed_result_t;
SIM_PY_ALLOCATABLE(docea_double_timed_result_t);

#ifdef __cplusplus
}
#endif

#endif
