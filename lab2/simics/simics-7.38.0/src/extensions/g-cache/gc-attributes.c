/*
  gc-attributes.c - g-cache specific attributes

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>
#include <simics/simulator-iface/scalar-time.h>

/* cache includes */
#include "gc-common.h"
#include "gc.h"

#ifndef HOOK_DEFAULT_REPL_POLICY
#define HOOK_DEFAULT_REPL_POLICY "random"
#endif

/* hap callback for DSTC hit count */
static void
dstc_hit_count_callback(void *data, conf_object_t *cpu, uint64 type, 
                        uint64 va, uint64 pa, uint64 cnt)
{
        generic_cache_t *gc = (generic_cache_t *) data;

        if (type == Sim_RW_Read && gc->config.direct_read_listener) {
                /* first level cache, read */
#ifdef GC_DEBUG
                pr("[%-3s] [0x%016llx, 0x%016llx, %lld] total += %lld\n", 
                   SIM_object_name(&gc->obj), va, pa, type, cnt);
#endif
                gc->stat.transaction += cnt;
                gc->stat.data_read += cnt;
        }
        else if (type == Sim_RW_Write && gc->config.direct_write_listener) {
                
                /* if the line is in the cache, we're counting hits, otherwise
                   misses (in a NWA cache for example) */
                generic_transaction_t mop;
                int ln;
                SIM_set_mem_op_virtual_address(&mop, va);
                SIM_set_mem_op_physical_address(&mop, pa);
                SIM_set_mem_op_initiator(&mop,
                                         Sim_Initiator_Other,
                                         &gc->obj);
                ln = lookup_line(gc, &mop);
                
                /* we're an allocating cache and we match the line */
                if (gc->config.write_allocate && ln != -1) {
                        
                        /* If STC_type is set, it means we set this in
                           the STC, so we should count it as hit 
                           If not, it means we were getting this transaction 
                           through a cache transaction so we don't care about 
                           the DSTC flushing. */
                        if (gc->lines[ln].STC_type) {
#ifdef GC_DEBUG
                                pr("[%-3s] [0x%016llx, 0x%016llx, %lld] "
                                   "total += %lld\n", 
                                   SIM_object_name(&gc->obj), va, pa, type, cnt);
#endif
                                gc->stat.data_write += cnt;
                                gc->stat.transaction += cnt;
                        }
                }
                /* we're a non-allocating cache */
                else if (!gc->config.write_allocate) {
                        
                        /* we don't have the line in cache, so we count it
                           as a miss */
                        if (ln == -1) {
#ifdef GC_DEBUG
                                pr("[%-3s] [0x%016llx, 0x%016llx, %lld] "
                                   "miss  += %lld\n", 
                                   SIM_object_name(&gc->obj), va, pa, type, cnt);
#endif
                                gc->stat.data_write_miss += cnt;
                                gc->stat.data_write += cnt;
                                gc->stat.transaction += cnt;

                                /* update profilers too, but not the PC based
                                   profiler since it shouldn't have allowed a
                                   miss to be cached in the STC--it would get
                                   no PC to use when the line is flushed */
                                if (gc->prof[V_DWM].id)
                                        prof_data_add(gc->prof[V_DWM].id,
                                                      va, cnt);

                                if (gc->prof[P_DWM].id)
                                        prof_data_add(gc->prof[P_DWM].id, 
                                                      pa, cnt);
                        }
                        else if (gc->lines[ln].STC_type) {
#ifdef GC_DEBUG
                                pr("[%-3s] [0x%016llx, 0x%016llx, %lld] "
                                   "total += %lld\n", 
                                   SIM_object_name(&gc->obj), va, pa, type, cnt);
#endif
                                gc->stat.data_write += cnt;
                                gc->stat.transaction += cnt;
                        }
                        /* else we didn't handle this line via DSTC so there's
                           no need to count */
                }
        }
}

/* attribute manipulation functions */
void
update_precomputed_values(generic_cache_t *gc)
{
        int i;

        gc->config.no_STC =
                (gc->penalty.read > 0)
                || (gc->penalty.write > 0)
                || (gc->config.block_STC);
        
        /* remove the STC callbacks */
        SIM_hap_delete_callback(
                "Core_DSTC_Flush_Counter",
                (obj_hap_func_t) dstc_hit_count_callback,
                gc);
        (void)SIM_clear_exception();

        if (!gc->config.no_STC) {
                /* add a callback for each cpu */
                for (i=0; i<gc->cpus_count; i++)
                        SIM_hap_add_callback_obj(
                                "Core_DSTC_Flush_Counter", 
                                gc->cpus[i],
                                0,
                                (obj_hap_func_t) dstc_hit_count_callback,
                                gc);
        }

        gc->config.line_size_log2 = ilog2(gc->config.line_size);
        gc->config.next_assoc = (gc->config.assoc) > 0 
                ? gc->config.line_number / gc->config.assoc : -1;
                
        gc->config.tag_mask = ~(gc->config.line_size - 1);
        gc->config.index_mask = gc->config.next_assoc - 1;

        /* don't call if repl_data is not allocated yet */
        if (gc->config.repl_data)
                gc->config.repl_fun->update_config(gc->config.repl_data, gc);
}

/* config_block_STC */
GC_INT_ATTR_UPDATE_SET(config, block_STC)
GC_INT_ATTR_GET(config, block_STC)

/* penalty_read_next */
GC_INT_ATTR_SET(penalty, read_next)
GC_INT_ATTR_GET(penalty, read_next)

/* penalty_write_next */
GC_INT_ATTR_SET(penalty, write_next)
GC_INT_ATTR_GET(penalty, write_next)

/* stat_mesi_exclusive_to_shared */
GC_INT_ATTR_SET(stat, mesi_exclusive_to_shared)
GC_INT_ATTR_GET(stat, mesi_exclusive_to_shared)

/* stat_mesi_modified_to_shared */
GC_INT_ATTR_SET(stat, mesi_modified_to_shared)
GC_INT_ATTR_GET(stat, mesi_modified_to_shared)

/* stat_mesi_invalidate */
GC_INT_ATTR_SET(stat, mesi_invalidate)
GC_INT_ATTR_GET(stat, mesi_invalidate)

/* stat_lost_stall_cycles */
GC_INT_ATTR_SET(stat, lost_stall_cycles)
GC_INT_ATTR_GET(stat, lost_stall_cycles)


/* called from set_lines only */
static set_error_t
set_single_line(generic_cache_t *gc, cache_line_t *cl, attr_value_t line)
{
        int64 line_zero = SIM_attr_integer(SIM_attr_list_item(line, 0));
        if (line_zero < 0 || line_zero > 3) {
               SIM_LOG_ERROR(&gc->obj, GC_Log_Attr,
                              "Line %lld status is incorrect",
                              line_zero);
                return Sim_Set_Illegal_Value;
        }

        int64 line_three = SIM_attr_integer(SIM_attr_list_item(line, 3));
        if (line_three < 0 || line_three > 3) {
                SIM_LOG_ERROR(&gc->obj, GC_Log_Attr,
                              "Line %lld STC_type is incorrect",
                              line_three);
                return Sim_Set_Illegal_Value;
        }

        cl->status    = line_zero;
        cl->tag       = SIM_attr_integer(SIM_attr_list_item(line, 1));
        cl->otag      = SIM_attr_integer(SIM_attr_list_item(line, 2));
        cl->STC_type  = line_three;

        return Sim_Set_Ok;
}

/* lines */
static set_error_t
set_lines(void *dont_care, conf_object_t *obj,
          attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (!gc->lines) {
                SIM_LOG_ERROR(
                        &gc->obj, GC_Log_Attr,
                        "generic-cache::set_lines: lines is a NULL pointer");
                return Sim_Set_Illegal_Value;
        }

        if (SIM_attr_list_size(*val) != gc->config.line_number) {
                SIM_LOG_ERROR(&gc->obj, GC_Log_Attr,
                              "generic-cache::set_lines: "
                              "this has cache doesn't have the right "
                              "number of lines for loading the lines.");
                return Sim_Set_Illegal_Value;
        }

        for (unsigned i = 0; i < gc->config.line_number; i++) {
                set_error_t diag = set_single_line(
                        gc, &gc->lines[i], SIM_attr_list_item(*val, i));

                if (diag != Sim_Set_Ok)
                        return diag;
        }
        return Sim_Set_Ok;
}

static attr_value_t // only used by get_lines
get_single_line(generic_cache_t *gc, int i)
{
        cache_line_t *cl = &gc->lines[i];
        return SIM_make_attr_list(
                4,
                SIM_make_attr_uint64(cl->status),
                SIM_make_attr_uint64(cl->tag),
                SIM_make_attr_uint64(cl->otag),
                SIM_make_attr_uint64(cl->STC_type));
}

static attr_value_t
get_lines(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (!gc->lines) {
                SIM_LOG_ERROR(
                        &gc->obj, GC_Log_Attr,
                        "generic-cache::get_lines: lines is a NULL pointer");
                return SIM_make_attr_invalid();
        }

        attr_value_t ret = SIM_alloc_attr_list(gc->config.line_number);
        for (int i = 0; i < gc->config.line_number; i++)
                SIM_attr_list_set_item(&ret, i, get_single_line(gc, i));
        return ret;
}

/* cacheable devices */
static set_error_t
set_cacheable_devices(void *dont_care, conf_object_t *obj,
                      attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (gc->cacheable_devices_size) {
                MM_FREE(gc->cacheable_devices);
        }

        if (SIM_attr_is_nil(*val)) {
                gc->cacheable_devices_size = 0;
                gc->config.cache_device_accesses = 0;
                return Sim_Set_Ok;
        }

        unsigned size = SIM_attr_list_size(*val);
        gc->cacheable_devices = MM_MALLOC(size, conf_object_t *);
        gc->cacheable_devices_size = size;

        if (size > 0)
                gc->config.cache_device_accesses = 1;

        for (unsigned i = 0; i < size; i++) {
                attr_value_t cd = SIM_attr_list_item(*val, i);
                gc->cacheable_devices[i] = SIM_attr_object(cd);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_cacheable_devices(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (gc->cacheable_devices) {
                attr_value_t ret = SIM_alloc_attr_list(gc->cacheable_devices_size);
                for (unsigned i = 0; i < gc->cacheable_devices_size; i++) {
                        SIM_attr_list_set_item(
                                &ret, i, SIM_make_attr_object(gc->cacheable_devices[i]));
                }
                return ret;
        }
        return SIM_make_attr_nil();
}

/* snoopers */
static set_error_t
set_snoopers(void *dont_care, conf_object_t *obj,
             attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;
        
        if (gc->snoopers_size) {
                MM_FREE(gc->snoopers);
                MM_FREE(gc->snoopers_ifc);
        }

        if (SIM_attr_is_nil(*val)) {
                gc->snoopers_size = 0;
                gc->config.mesi = 0;
                return Sim_Set_Ok;
        }

        unsigned size = SIM_attr_list_size(*val);
        gc->snoopers = MM_MALLOC(size, conf_object_t *);
        gc->snoopers_ifc = MM_MALLOC(size, const mesi_listen_interface_t *);
        gc->snoopers_size = size;

        for (unsigned i = 0; i < size; i++) {

                attr_value_t sn = SIM_attr_list_item(*val, i);

                gc->snoopers_ifc[i] = SIM_c_get_interface(SIM_attr_object(sn),
                                                          MESI_LISTEN_INTERFACE);
                if (!gc->snoopers_ifc[i]) {
                        SIM_LOG_ERROR(&gc->obj, GC_Log_Attr,
                                      "set_snoopers: "
                                      "object %d does not provide the mesi "
                                      "listen interface.", i);
                        gc->snoopers_size = 0;
                        MM_FREE(gc->snoopers);
                        MM_FREE(gc->snoopers_ifc);
                        gc->snoopers = NULL;
                        gc->snoopers_ifc = NULL;
                        return Sim_Set_Interface_Not_Found;
                }

                gc->snoopers[i] = SIM_attr_object(sn);
        }

        /* enable MESI */
        if (gc->snoopers_size > 0)
                gc->config.mesi = 1;
        else
                gc->config.mesi = 0;

        return Sim_Set_Ok;
}

static attr_value_t
get_snoopers(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (gc->snoopers) {
                attr_value_t ret = SIM_alloc_attr_list(gc->snoopers_size);

                for (unsigned i = 0; i < gc->snoopers_size; i++) {
                        SIM_attr_list_set_item(
                                &ret, i, SIM_make_attr_object(gc->snoopers[i]));
                }

                return ret;
        }

        return SIM_make_attr_nil();
}

/* higher_level_caches */
static set_error_t
set_hlc(void *dont_care, conf_object_t *obj,
        attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (gc->hlc_size) {
                MM_FREE(gc->hlc);
                MM_FREE(gc->hlc_ifc);
        }

        if (SIM_attr_is_nil(*val)) {
                gc->hlc_size = 0;
                return Sim_Set_Ok;
        }

        unsigned size = SIM_attr_list_size(*val);
        gc->hlc = MM_MALLOC(size, conf_object_t *);
        gc->hlc_ifc = MM_MALLOC(size, const mesi_listen_interface_t *);
        gc->hlc_size = size;

        for (unsigned i = 0; i < size; i++) {
                attr_value_t sn = SIM_attr_list_item(*val, i);

                gc->hlc_ifc[i] = SIM_c_get_interface(SIM_attr_object(sn),
                                                     MESI_LISTEN_INTERFACE);
                if (!gc->hlc_ifc[i]) {
                        SIM_LOG_ERROR(&gc->obj, GC_Log_Attr,
                                      "set_hlc: "
                                      "object %d does not provide the mesi "
                                      "listen interface.", i);
                        gc->hlc_size = 0;
                        MM_FREE(gc->hlc);
                        MM_FREE(gc->hlc_ifc);
                        gc->hlc = NULL;
                        gc->hlc_ifc = NULL;
                        return Sim_Set_Interface_Not_Found;
                }

                gc->hlc[i] = SIM_attr_object(sn);
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_hlc(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (gc->hlc) {
                attr_value_t ret = SIM_alloc_attr_list(gc->hlc_size);

                for (unsigned i = 0; i < gc->hlc_size; i++) {
                        SIM_attr_list_set_item(
                                &ret, i, SIM_make_attr_object(gc->hlc[i]));
                }

                return ret;
        }

        return SIM_make_attr_nil();
}


#define SCALAR_TIME_INTERFACE_FUNCTIONS(port, desc)                           \
static int                                                                    \
port ## _add_consumer(conf_object_t *obj)                                     \
{                                                                             \
        generic_cache_t *gc = (generic_cache_t *)obj;                         \
        return st_add_consumer(&gc->scalar_time_port[ST_ ## port].sp, obj);   \
}                                                                             \
static void                                                                   \
port ## _remove_consumer(conf_object_t *obj, int consumer)                    \
{                                                                             \
        generic_cache_t *gc = (generic_cache_t *)obj;                         \
        st_remove_consumer(&gc->scalar_time_port[ST_ ## port].sp,             \
                           consumer, obj);                                    \
}                                                                             \
static attr_value_t                                                           \
port ## _poll(conf_object_t *obj, int consumer)                               \
{                                                                             \
        generic_cache_t *gc = (generic_cache_t *)obj;                         \
        return st_poll(&gc->scalar_time_port[ST_ ## port].sp, consumer,       \
                       SIM_time(obj));                                        \
}

FOR_SCALAR_TIME_PORTS(SCALAR_TIME_INTERFACE_FUNCTIONS)

#define REGISTER_SCALAR_TIME_INTERFACE(port, desc) do {                 \
        static const scalar_time_interface_t iface = {                  \
                .add_consumer = port ## _add_consumer,                  \
                .remove_consumer = port ## _remove_consumer,            \
                .poll = port ## _poll                                   \
        };                                                              \
        SIM_register_port_interface(gc_class, SCALAR_TIME_INTERFACE,    \
                                    &iface, #port, desc);               \
} while (0);

/* registering attributes */
void
gc_register(conf_class_t *gc_class)
{
        static const mesi_listen_interface_t mesi_ifc = {
                .snoop_transaction = mesi_snoop_transaction_export
        };
        SIM_register_interface(gc_class, MESI_LISTEN_INTERFACE, &mesi_ifc);

        st_init(gc_class);
        FOR_SCALAR_TIME_PORTS(REGISTER_SCALAR_TIME_INTERFACE);

        SIM_register_typed_attribute(
                gc_class, "config_block_STC",
                get_config_block_STC, 0,
                set_config_block_STC, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Prevent the cache from using the STCs. Read the"
                " 'Cache Simulation' chapter in Simics Analyzer Guide"
                " for more information"
                " (0: STC usage allowed, 1: STC usage blocked;"
                " default is 0).");

        SIM_register_typed_attribute(
                gc_class, "penalty_read_next",
                get_penalty_read_next, 0,
                set_penalty_read_next, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall penalty (in cycles) for a read transaction issued by "
                "the cache to "
                "the next level cache. A cache miss on read will have "
                "a penalty for 'read' (incoming transaction) + 'read-next' "
                "(line fetch transaction) + any penalty set by the next "
                "caches. (default is 0)");

        SIM_register_typed_attribute(
                gc_class, "penalty_write_next",
                get_penalty_write_next, 0,
                set_penalty_write_next, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall penalty (in cycles) for a write transactions issued by"
                " the cache to the next level cache. In a write-back cache, a "
                "cache miss on read "
                "triggering a copy-back transaction will have a penalty for "
                "'read', 'write-next' (copy-back transaction) and 'read-next' "
                "(line fetch transaction). In write-through cache, a "
                "write transaction will always have at least a penalty for "
                "'write' and 'write-next' (write-through transaction). "
                "(default is 0).");

        SIM_register_typed_attribute(
                gc_class, "lines",
                get_lines, 0,
                set_lines, 0,
                Sim_Attr_Optional | Sim_Init_Phase_1,
                "[[iiii]*]", NULL,
                "Content of the cache lines");

        SIM_register_typed_attribute(
                gc_class, "cacheable_devices",
                get_cacheable_devices, 0,
                set_cacheable_devices, 0,
                Sim_Attr_Optional,
                "n|[o*]", NULL,
                "List of devices that can be cached.");

        SIM_register_typed_attribute(
                gc_class, "snoopers",
                get_snoopers, 0,
                set_snoopers, 0,
                Sim_Attr_Optional,
                "n|[o*]", NULL,
                "Caches listening on the bus (MESI protocol).");

        SIM_register_typed_attribute(
                gc_class, "higher_level_caches",
                get_hlc, 0,
                set_hlc, 0,
                Sim_Attr_Optional,
                "n|[o*]", NULL,
                "Higher level caches that need to receive invalidates during "
                "MESI snooping (MESI protocol).");

        SIM_register_typed_attribute(
                gc_class, "stat_mesi_exclusive_to_shared",
                get_stat_mesi_exclusive_to_shared, 0,
                set_stat_mesi_exclusive_to_shared, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of Exclusive to Shared transitions in MESI protocol.");

        SIM_register_typed_attribute(
                gc_class, "stat_mesi_modified_to_shared",
                get_stat_mesi_modified_to_shared, 0,
                set_stat_mesi_modified_to_shared, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of Modified to Shared transitions in MESI protocol.");

        SIM_register_typed_attribute(
                gc_class, "stat_mesi_invalidate",
                get_stat_mesi_invalidate, 0,
                set_stat_mesi_invalidate, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of lines invalidated in MESI protocol.");

        SIM_register_typed_attribute(
                gc_class, "stat_lost_stall_cycles",
                get_stat_lost_stall_cycles, 0,
                set_stat_lost_stall_cycles, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall cycles lost due to non-stallable transactions.");
}

#define DEFAULT_SAMPLE_RATE 1e-2 /* seconds */

void
gc_init_cache(generic_cache_t *gc)
{
        /* Set block_STC to 0 since the cache will behave properly in most
           cases. */
        gc->config.block_STC = 0;

        gc->config.line_size = 32;
        gc->config.assoc = 4;
        
        gc_set_config_repl(gc, HOOK_DEFAULT_REPL_POLICY);
        gc_set_config_line_number(gc, 128);

        for (int i = 0; i < Num_Scalar_Time_Ports; i++)
                st_ssp_init(&gc->scalar_time_port[i], 1, DEFAULT_SAMPLE_RATE);
}
