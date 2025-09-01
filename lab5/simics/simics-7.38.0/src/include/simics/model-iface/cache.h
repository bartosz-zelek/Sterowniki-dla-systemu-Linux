/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_MODEL_IFACE_CACHE_H
#define SIMICS_MODEL_IFACE_CACHE_H

#include <simics/base/types.h>
#include <simics/base/time.h>
#include <simics/base/memory-transaction.h>
#include <simics/pywrap.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum {
        Cache_Control_Nop,
        Cache_Control_Fetch_Line,
        Cache_Control_Invalidate_Line,
        Cache_Control_Copyback_Line,
        Cache_Control_Invalidate_Cache
} cache_control_operation_t;

/* Interface for controlling a cache. (internal use only) */
SIM_INTERFACE(cache_control) {
        /* Returns the number of cycles until the operation is complete. */
        cycles_t (*cache_control)(conf_object_t *cache,
                                  cache_control_operation_t op,
                                  generic_transaction_t *mem_op);
};
#define CACHE_CONTROL_INTERFACE "cache_control"
// ADD INTERFACE cache_control_interface

#if defined(__cplusplus)
}
#endif

#endif
