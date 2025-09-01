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

#ifndef SIMPLE_CACHE_H
#define SIMPLE_CACHE_H

#include <simics/model-iface/int-register.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/simulator-api.h>
#include <simics/arch/x86.h>
#include "simple-cache-interface.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* MESI states */
typedef enum {
        CL_Invalid   = 0,
        CL_Shared    = 1,
        CL_Exclusive = 2,
        CL_Modified  = 3
} cache_line_state_t;

typedef enum {
        Policy_NINE,
        Policy_Inclusive,
        Policy_Exclusive
} cache_policy_t;

#define CL_IS_PRIVATE(s) ((s) >= CL_Exclusive)

/* Structure of a simple cache to be configured */
typedef struct cache_line {
	uint64 tag;
        struct cache *owner;
        uint32 access;
        uint8 state;
        bool ip_prefetched;
        bool prefetched;
} cache_line_t;

// Holds information of another cache in the hierarchy and its interface
typedef struct {
        conf_object_t *obj;
        char *name;
        const simple_cache_interface_t *iface;
} cache_info_t;

// Keeps data for a stride in the prefetcher
typedef struct {
        cache_line_t *blocks[3];
} prefetch_entry_t;

 // Main data for a cache object
typedef struct cache {
        conf_object_t obj;

        /* first connection this cache is called from */
        conf_object_t *conn;

        /* The first cpu (thread) connected to the cpu, not set for caches shared
           among several cores */
        conf_object_t *cpu_thread;

        /* Next cache in hierarchy */
        cache_info_t next;

        /* Previous cache(s) in hierarchy */
        VECT(cache_info_t) prev;

        /* Cache level */
        unsigned level;

        /* The directory for this cache level, shared between coherent caches
           on the same level */
        struct directory *directory;
        char *directory_name;

        /* Info about last cpu doing a stride prefetching, needed for pc (rip) */
        struct {
                conf_object_t *obj;
                int rip;
                const int_register_interface_t *ir_iface;
        } cpu;

        /* Ip prefetcher info */
        struct {
                unsigned size;
                unsigned mask;
                prefetch_entry_t *read_entries;
                prefetch_entry_t *write_entries;
        } prefetcher;

        /* All the meta data for the cache lines for all ways are stored in this
           array, set0way0, set0way1..set0wayN, set1way0, set1way1 etc. */
        cache_line_t *lines; /* sets * ways */

        /* To implement RSU algorithm efficiently, the lookup order for the
           corresponding ways in the lines member above are stored in this
           array, e.g., for a cache with 4 ways:
           { 1,3,0,2,   check way 1 (MRU) then 3, 0, and finaly 2 (LRU) in set 0
             3,0,2,1,   check way 3 (MRU) then 0, 2, and finaly 1 (LRU) in set 1
             ... }      ...

           So for each set the most recently used index for that way is stored
           first, then the next most recently used index is stored, down to the
           least recently used index, which is last. On a cache hit, the index
           of the accessed way is moved to the first location in the way and
           the others are shifted to the right, thereby maintaining the
           recently used order. This is more efficient than linearly scanning
           the ways in the set for a time-stamp.
        */
        uint8 *lookup_order; /* sets * ways */

	struct {
                unsigned block_size;     // cache block size
                unsigned block_size_log2;// log2 block size
                unsigned ways;           // number of ways
                unsigned line_number;    // number of lines in all ways together
                unsigned tag_shift;      // tag bits, includes index part as well
                unsigned number_of_sets; // number of indexes
                bool write_allocate;     // write allocate blocks
                bool write_back;         // write back cache
                bool ip_read_prefetcher; // use a ip prefetcher for reads
                bool ip_write_prefetcher;// use a ip prefetcher for writes */

                // always prefetch a block of block_size * 2
                bool prefetch_adjacent;

                // fetch this many additional cache lines in a row after current
                unsigned prefetch_additional;

                // inclusion pollicy, not implemented yet
                cache_policy_t policy;

                uint64 read_penalty;      // penalty for reaching the cache
                uint64 write_penalty;     // penalty for reaching the cache
                uint64 read_miss_penalty; // penalty for accessing next level
                uint64 write_miss_penalty;// penalty for accessing next level

                uint64 snoop_penalty;     // extra penalty to snooping

                // add statistics when we have both a STLB miss and a cache miss
                bool last_level_tlb_stat;
	} config;

        // statistics
        struct {
                uint64 access_total[Access_Type_Num];
                uint64 access_miss[Access_Type_Num];
                uint64 evicted_modified;// evicted number of lines in M state
                uint64 evicted_total;   // evicted total num of lines
                uint64 prefetches_used; // num of lines prefetched actually used
	} stat;
} cache_t;

extern const char *miss_bin_str[];

FORCE_INLINE uint64
tag_from_pa(cache_t *self, uint64 pa)
{
	return pa >> self->config.tag_shift;
}

FORCE_INLINE int
index_from_pa(cache_t *self, uint64 pa)
{
	return (pa >> self->config.block_size_log2)
                & (self->config.number_of_sets -1);
}

FORCE_INLINE uint64
pa_from_line(cache_line_t *line)
{
        // tag contains index
        return line->tag << line->owner->config.block_size_log2;
}

FORCE_INLINE cache_t *
obj_to_cache(conf_object_t *obj)
{
        return (cache_t *)obj;
}

bool object_exists(char *name);
void init_cache();
cycles_t handle_read(conf_object_t *obj, conf_object_t *initiator, ini_type_t t,
                     physical_address_t pa, unsigned size, access_type_t type,
                     cache_line_t **line);
cycles_t handle_write(conf_object_t *obj, conf_object_t *initiator,
                      ini_type_t t, physical_address_t pa, unsigned size,
                      access_type_t access, cache_line_t **line);

extern conf_class_t *simple_cache;

#if defined(__cplusplus)
}
#endif

#endif
