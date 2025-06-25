/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SIMULATOR_IFACE_OSA_TYPES_H
#define SIMICS_SIMULATOR_IFACE_OSA_TYPES_H

#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef uint64 cancel_id_t;
typedef uint64 entity_id_t;
typedef uint64 node_id_t;
typedef uint64 transaction_id_t;
typedef uint64 request_id_t;

typedef enum {
        Cancel_Error_ID = 0,
} cancel_id_error_t;

typedef struct maybe_node_id {
        bool valid;
        node_id_t id;
} maybe_node_id_t;

#if defined(__cplusplus)
}
#endif
#endif /* ! SIMICS_SIMULATOR_IFACE_OSA_TYPES_H */
