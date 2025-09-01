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

/* Define an empty tool for the connection */

#ifndef CACHE_TOOL_H
#define CACHE_TOOL_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/simulator/conf-object.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/simulator/output.h>
#include <simics/base/memory-transaction.h>

#include "stall-cycles-collector-interface.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct simple_cache_tool {
        conf_object_t obj;
        unsigned cpu_id;
        int next_conn_cache_number;
        struct {
                conf_object_t *obj;
                const stall_cycles_collector_interface_t *iface;
        } cycle_staller;
        VECT(conf_object_t *) connections;
} simple_cache_tool_t;

#if defined(__cplusplus)
}
#endif

#endif
