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
#ifndef READABLE_FLOAT_INTERFACE_H
#define READABLE_FLOAT_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>

#include "docea_rw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(readable_float) {
  // Read the value of the parameter identified by the object at the given time.
  //
  // time: the time, in picoseconds, when the value is requested. The result may
  // be not computable up to the exact given time, but the computation may
  // accept some time delay (configured outside the scope of this interface),
  // hence the result may be returned for a time lesser than the one requested.
  // The time in the returned result gives the exact time for which the value
  // could be computed.
  docea_rw_double_timed_result_t (*read)(conf_object_t * obj, uint64 time_ps);
};

#define READABLE_FLOAT_INTERFACE "readable_float"

#ifdef __cplusplus
}
#endif

#endif
