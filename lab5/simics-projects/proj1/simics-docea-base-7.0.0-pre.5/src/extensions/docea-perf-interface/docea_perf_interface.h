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

#ifndef DOCEA_PERF_INTERFACE_H
#define DOCEA_PERF_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <stddef.h>

#include "docea_ptm_types.h"

#ifdef __cplusplus
extern "C" {
#endif

SIM_INTERFACE(docea_perf) {
  /* Update the cdyn of a core with the given value until the given time.

   obj: The object implementing this interface.

   time: the instant, in seconds, until when the cdyn had the given value.

   id: the unique identifier of the core.

   value: the cdyn value, in F. */
  docea_status_result_t (*update_past_cdyn)(conf_object_t * obj, double time,
                                            docea_id_t id, double value);
};

#define DOCEA_PERF_INTERFACE "docea_perf"

#ifdef __cplusplus
}
#endif

#endif /* DOCEA_PERF_INTERFACE_H */
