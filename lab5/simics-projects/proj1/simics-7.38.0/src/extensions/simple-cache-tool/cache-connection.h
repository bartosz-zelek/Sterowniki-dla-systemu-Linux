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

#ifndef CACHE_CONNECTION_H
#define CACHE_CONNECTION_H

#include "cache-tool.h"
#include "simple-cache.h"

#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/model-iface/exception.h>
#include <simics/model-iface/processor-info.h>
#include <simics/arch/x86-instrumentation.h>
#include <simics/arch/x86.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct empty_tool_t;

/* Cache struct */
typedef struct {
        conf_object_t obj;
        conf_object_t *provider;
        conf_object_t *dl1;
        conf_object_t *il1;

        simple_cache_tool_t *tool;

        char *dl1_name; // hierarchical name for dl1 cache
        char *il1_name; // hierarchical name for il1 cache

        bool issue_instructions; // add callback for instruction accesses
        cpu_cb_handle_t *instruction_handle;
        physical_address_t last_instruction_block;
        physical_address_t instruction_block_size; // block size of icache

        /* statistics */
        uint64 uncachable_reads;
        uint64 uncachable_writes;
        uint64 prefetch_instructions;
        uint64 cache_all_flush_instructions;
        uint64 cache_line_flush_instructions;

        const simple_cache_interface_t *dl1_iface;
        const simple_cache_interface_t *il1_iface;

        struct empty_tool *empty_tool;

        /* Interfaces */
	const processor_info_interface_t *pi_iface;
	const cpu_instruction_query_interface_t *iq_iface;
	const cpu_instrumentation_subscribe_interface_t *cis_iface;
        const cpu_cached_instruction_interface_t *ci_iface;
	const cpu_memory_query_interface_t *mq_iface;
        const x86_memory_query_interface_t *xq_iface;

        uint64 new_access;
        FILE *fp;

} connection_t;

FORCE_INLINE connection_t *
obj_to_conn(conf_object_t *obj)
{
        return (connection_t *)obj;
}

void init_conn();
void configure_conn(connection_t *conn);

extern conf_class_t *conn_class;

#if defined(__cplusplus)
}
#endif

#endif
