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

#ifndef MEMORY_PROFILER_H
#define MEMORY_PROFILER_H

#include "memory-profiler-connection.h"

#include <simics/base/types.h>
#include <simics/base/conf-object.h>
#include <simics/util/alloc.h>
#include <simics/util/hashtab.h>
#include <simics/simulator-iface/address-profiler.h>

#if defined(__cplusplus)
extern "C" {
#endif
        
typedef struct memory_profiler {
        conf_object_t obj;

        unsigned granularity; // page stat granularity, power of 2
        uint64 page_size;
        uint64 page_addr_mask;
        uint64 page_offs_mask;

        int next_connection_number;
        VECT(connection_t *) connections;
} memory_profiler_t;

typedef VECT(stat_page_t *) stat_page_vect_t;

typedef struct memory_profiler_iter {
        addr_prof_iter_t super; // must be first
        memory_profiler_t *mp;
        uint64 start;
        uint64 stop;
        stat_page_vect_t pages;
        unsigned current_page;
        unsigned current_page_offs_idx;
        bool first;
} memory_profiler_iter_t;

#if defined(__cplusplus)
}
#endif         
#endif
