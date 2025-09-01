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

#ifndef MEMORY_PROFILER_CONNECTION_H
#define MEMORY_PROFILER_CONNECTION_H

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>
#include <simics/util/interval-set.h>

#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/cpu-instrumentation.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define FOR_OPTIONS(op)                                                  \
        op(read_physical,    "profile of physical memory reads")         \
        op(read_logical,     "profile of logical memory reads")          \
        op(write_physical,   "profile of physical memory writes")        \
        op(write_logical,    "profile of logical memory writes")         \
        op(execute_physical, "profile of physical memory reads")         \
        op(execute_logical,  "profile of logical memory reads")
        
typedef enum {
        View_Read_Physical,
        View_Read_Logical,
        View_Write_Physical,
        View_Write_Logical,
        View_Execute_Physical,
        View_Execute_Logical,
        View_Num
} profile_view_t;
        
struct memory_profiler;

#define DECLARE(option, desc)                   \
        bool option;

typedef struct stat_page {
        uint64 start;
        uint64 end;
        unsigned granularity;
        unsigned len; // number of stat entries below
        uint64 *counters;
} stat_page_t;
        
typedef struct {
        conf_object_t obj;
        conf_object_t *cpu;
        struct memory_profiler *mp;

        /* data counters for each page */
        interval_set_t *view[View_Num];

        /* Last accessed page */       
        stat_page_t *cache_page[View_Num];
        
        /* Cached interfaces */
        const cpu_instrumentation_subscribe_interface_t *is_iface;
        const cpu_instruction_query_interface_t *iq_iface;
        const cpu_memory_query_interface_t *mq_iface;
        const cpu_cached_instruction_interface_t *ci_iface;

        FOR_OPTIONS(DECLARE)
} connection_t;

FORCE_INLINE connection_t *
obj_to_con(conf_object_t *obj)
{
        return (connection_t *)obj;
}

void configure_connection(connection_t *con);
void init_connection();

extern conf_class_t *connection_class;

#if defined(__cplusplus)
}
#endif
#endif
