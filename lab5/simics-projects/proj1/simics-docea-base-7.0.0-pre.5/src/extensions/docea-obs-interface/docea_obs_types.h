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

#ifndef DOCEA_OBS_TYPES_H
#define DOCEA_OBS_TYPES_H

#include "docea_ptm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// The result type putting together a floating point value and a status code.
// A status code 0 means success, any other value denotes an error (hence other
// values in the result are meaningless).
typedef struct {
  docea_status_t status;
  double value;
} docea_double_result_t;
SIM_PY_ALLOCATABLE(docea_double_result_t);

#ifdef __cplusplus
}
#endif

#endif
