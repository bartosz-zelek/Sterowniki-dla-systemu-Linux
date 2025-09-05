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
#ifndef WRITABLE_FLOAT_INTERFACE_H
#define WRITABLE_FLOAT_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_rw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(writable_float) {
  // Write the given new value for the given time into the parameter identified
  // by the object.
  //
  // time_ps: the time, in picoseconds, when the parameter changes to become the
  // new value.
  docea_rw_status_result_t (*write)(conf_object_t * obj, uint64 time_ps,
                                    double new_value);
};

#define WRITABLE_FLOAT_INTERFACE "writable_float"

#ifdef __cplusplus
}
#endif

#endif
