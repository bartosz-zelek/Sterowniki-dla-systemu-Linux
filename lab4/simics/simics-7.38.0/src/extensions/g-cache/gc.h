/*
  gc.h - g-cache definitions

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef GC_H
#define GC_H

#include <simics/model-iface/timing-model.h>

#include "gc-common.h"

#ifdef HOOK_HEADER
#  include "gc-hook.h"
#else
#  define HOOK_EXTRA_DATA
#  define HOOK_EXTRA_LINE_DATA
#endif

/* cache line definition */
typedef struct cache_line {
        cache_line_status_t status;
        int64 tag;        /* tag of the data contained in the cache line */
        int64 otag;       /* opposite tag (virtual if tag is physical) */

        /* in-order specific info */
        int STC_type;         /* STC that may contain the cache line data */

        HOOK_EXTRA_LINE_DATA  /* hook, see above */
} cache_line_t;

#define FOR_SCALAR_TIME_PORTS(f)                                \
        f(transaction, "transaction")                           \
        f(dev_data_read, "device read")                         \
        f(dev_data_write, "device write")                       \
        f(c_dev_data_read, "cacheable device read")             \
        f(c_dev_data_write, "cacheable device write")           \
        f(uc_data_read, "uncached read")                        \
        f(uc_data_write, "uncached write")                      \
        f(uc_inst_fetch, "uncached instruction fetch")          \
        f(data_read, "read")                                    \
        f(data_read_miss, "read miss")                          \
        f(data_write, "write")                                  \
        f(data_write_miss, "write miss")                        \
        f(inst_fetch, "instruction fetch")                      \
        f(inst_fetch_miss, "instruction fetch miss")            \
        f(copy_back, "copy back")                               \
        f(mesi_exclusive_to_shared, "MESI exclusive to shared") \
        f(mesi_modified_to_shared, "MESI modified to shared")   \
        f(mesi_invalidate, "MESI invalidate")

#define SCALAR_TIME_PORT_ENUM(port, desc) ST_ ## port,
typedef enum {
        FOR_SCALAR_TIME_PORTS(SCALAR_TIME_PORT_ENUM)
        Num_Scalar_Time_Ports
} scalar_time_port_t;

/* generic cache object structure */
struct generic_cache {
        
        conf_object_t obj;

        cache_line_t   *lines;  /* cache lines */

        conf_object_t  **cpus;  /* cpus sending memory transactions */
        int            cpus_count;

        struct {
                int line_number;    /* number of cache lines */
                int line_size;      /* size of a cache line */
                int assoc;          /* associativity */

                int virtual_index;  /* virtual address used for index */
                int virtual_tag;    /* virtual address used for tag */

                int write_allocate; /* allocate on write */
                int write_back;     /* write-through/write-back */

                int block_STC;      /* prevent usage of the STC so the cache
                                       sees all the transactions */

                /* replacement policy system */
                void *repl_data;    /* data allocated by the replacement
                                       policy */
                const repl_interface_t *repl_fun; /* current repl.
                                                     policy functions */
                
                /* precomputed values */
                int mesi;       /* activate the coherence protocol */
                int no_STC;     /* tell the cache whether it should allow STC
                                   usage */

                int line_size_log2;
                int next_assoc;
                int64 tag_mask;
                int64 index_mask;

                /* These two variables check if this cache is receiving direct
                   read/writes from the processor. If 1, the cache should
                   handle the corresponding DSTC counts as hits, otherwise
                   ignore them. */
                int direct_read_listener;
                int direct_write_listener;

                /* support caching devices accesses */
                int cache_device_accesses;
        } config;

        struct {
                int read;
                int read_next;
                int write;
                int write_next;
        } penalty;

        struct {
                int64 transaction;

                /* device transactions (DMA) */
                int64 dev_data_read;
                int64 dev_data_write;

                /* cacheable device transactions */
                int64 c_dev_data_read;
                int64 c_dev_data_write;

                /* uncacheable transactions */
                int64 uc_data_read;
                int64 uc_data_write;
                int64 uc_inst_fetch;

                /* usual transactions */
                int64 data_read;
                int64 data_read_miss;
                int64 data_write;
                int64 data_write_miss;
                int64 inst_fetch;
                int64 inst_fetch_miss;

                int64 copy_back;

                int64 mesi_exclusive_to_shared;
                int64 mesi_modified_to_shared;
                int64 mesi_invalidate;

                int64 lost_stall_cycles;
        } stat;

        st_single_stats_port_t scalar_time_port[Num_Scalar_Time_Ports];

        struct {
                conf_object_t *obj;
                prof_data_t *id;
        } prof[GC_Max_Profilers];

        conf_object_t *timing_model;
        const timing_model_interface_t *timing_ifc;

        /* cacheable devices */
        conf_object_t **cacheable_devices;
        int cacheable_devices_size;

        /* caches listening on the bus - for mesi protocol */
        conf_object_t **snoopers;
        const mesi_listen_interface_t **snoopers_ifc;
        int snoopers_size;

        /* higher level caches - for mesi protocol */
        conf_object_t **hlc;
        const mesi_listen_interface_t **hlc_ifc;
        int hlc_size;

        HOOK_EXTRA_DATA /* hook, see above */
};

int lookup_line(generic_cache_t *gc, generic_transaction_t *mem_op);
cycles_t copy_back(generic_cache_t *gc, int line_num, 
                   generic_transaction_t *mem_op, conf_object_t *space, 
                   map_list_t *map, int write_back);
cycles_t empty_line(generic_cache_t *gc, int line_num, 
                    generic_transaction_t *mem_op, conf_object_t *space, 
                    map_list_t *map);
cycles_t fetch_line(generic_cache_t *gc, int line_num, 
                    generic_transaction_t *mem_op, conf_object_t *space, 
                    map_list_t *map);
cycles_t mesi_snoop_transaction(generic_cache_t *gc, 
                                generic_transaction_t *mem_op);
cycles_t mesi_snoop_transaction_export(conf_object_t *obj,
                                       generic_transaction_t *mem_op);
void flush_STC(generic_cache_t *gc, uint64 tag, uint64 otag, int STC_type, 
               cache_line_status_t status);

void gc_register(conf_class_t *gc_class);
void ts_init_local();
void update_precomputed_values(generic_cache_t *gc);

#endif  /* ! GC_H */
