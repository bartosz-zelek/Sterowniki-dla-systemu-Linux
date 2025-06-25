/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef STALL_CYCLES_COLLECTOR_INTERFACE_H
#define STALL_CYCLES_COLLECTOR_INTERFACE_H

#include <simics/device-api.h>
#include <simics/pywrap.h>
#include <simics/base/time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This interface should be used to accumulate stall times across different
   sources. And then take the penalty at a proper time. */
SIM_INTERFACE(stall_cycles_collector) {
        void (*add)(conf_object_t *obj, conf_object_t *clock, cycles_t cycles);
};

#define STALL_CYCLES_COLLECTOR_INTERFACE "stall_cycles_collector"

#ifdef __cplusplus
}
#endif

#endif /* ! STALL_CYCLES_COLLECTOR_INTERFACE_H */
