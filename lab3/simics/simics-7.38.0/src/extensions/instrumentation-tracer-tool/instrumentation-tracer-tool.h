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

#ifndef INSTRUMENTATION_TRACER_TOOL_H
#define INSTRUMENTATION_TRACER_TOOL_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/simulator/conf-object.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>

#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/simulator/output.h>

#include <simics/base/memory-transaction.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        conf_object_t *obj;
        uint64 types;
        uint64 groups;
        int level;
        int hap_id;
        struct tracer_tool *tool;
} trace_log_info_t;

typedef VECT(trace_log_info_t) trace_log_info_vect_t;

typedef struct tracer_tool {
        conf_object_t obj;
        char *file;
        FILE *fh;
        unsigned cpu_id;
        int next_connection_number;
        VECT(conf_object_t *) connections;
        trace_log_info_vect_t logging;
        uint64 log_cnt;
} tracer_tool_t;

#if defined(__cplusplus)
}
#endif        
#endif
