/*
  gc-common-attributes.c - common attributes for caches

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
#include <simics/model-iface/data-profiler.h>

#include "gc-common.h"
#include "gc.h"

/* emits attribute-error and returns false if the argument does not
   implement STC interface */
static bool
cpu_implements_stc(attr_value_t v)
{
        if (!SIM_c_get_interface(SIM_attr_object(v), "stc")) {
                SIM_c_attribute_error("%s does not provide the STC interface.",
                                      SIM_object_name(SIM_attr_object(v)));
                return false;
        }
        return true;
}

/* cpus */
static set_error_t
set_cpus(void *dont_care, conf_object_t *obj,
         attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        MM_FREE(gc->cpus);

        if (SIM_attr_is_nil(*val)) {
                gc->cpus = NULL;
                gc->cpus_count = 0;

        } else if (SIM_attr_is_object(*val)) {
                if (!cpu_implements_stc(*val))
                        return Sim_Set_Illegal_Value;

                gc->cpus = MM_MALLOC(1, conf_object_t *);
                gc->cpus[0] = SIM_attr_object(*val);
                gc->cpus_count = 1;

        } else {
                unsigned ls = SIM_attr_list_size(*val);
                for (unsigned i = 0; i < ls; i++)
                        if (!cpu_implements_stc(SIM_attr_list_item(*val, i)))
                                return Sim_Set_Illegal_Value;

                gc->cpus = MM_MALLOC(SIM_attr_list_size(*val), conf_object_t *);
                gc->cpus_count = ls;
                for (unsigned i = 0; i < gc->cpus_count; i++)
                        gc->cpus[i] = SIM_attr_object(
                                SIM_attr_list_item(*val, i));
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_cpus(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        attr_value_t ret = SIM_alloc_attr_list(gc->cpus_count);
        for (unsigned i = 0; i < SIM_attr_list_size(ret); i++) {
                SIM_attr_list_set_item(
                        &ret, i, SIM_make_attr_object(gc->cpus[i]));
        }
        
        return ret;
}

/* config_line_number */
static set_error_t
set_config_line_number(void *dont_care, conf_object_t *obj,
                       attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        if (!SIM_attr_integer(*val))
                return Sim_Set_Illegal_Value;

        gc_set_config_line_number(gc, SIM_attr_integer(*val));
        return Sim_Set_Ok;
}

GC_INT_ATTR_GET(config, line_number)

/* config_line_size */
GC_INT_ATTR_UPDATE_SET(config, line_size)
GC_INT_ATTR_GET(config, line_size)

/* config_assoc */
GC_INT_ATTR_UPDATE_SET(config, assoc)
GC_INT_ATTR_GET(config, assoc)

/* config_virtual_index */
GC_INT_ATTR_SET(config, virtual_index)
GC_INT_ATTR_GET(config, virtual_index)

/* config_virtual_tag */
GC_INT_ATTR_SET(config, virtual_tag)
GC_INT_ATTR_GET(config, virtual_tag)

/* config_write_allocate */
GC_INT_ATTR_SET(config, write_allocate)
GC_INT_ATTR_GET(config, write_allocate)

/* config_write_back */
GC_INT_ATTR_SET(config, write_back)
GC_INT_ATTR_GET(config, write_back)

/* config_repl */
static set_error_t
set_config_repl(void *dont_care, conf_object_t *obj,
                attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        if (gc_set_config_repl(gc, SIM_attr_string(*val)) == 0)
                return Sim_Set_Ok;
        else
                return Sim_Set_Illegal_Value;
}

static attr_value_t
get_config_repl(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        return SIM_make_attr_string(gc->config.repl_fun->get_name());
}

/* penalty_read */
GC_INT_ATTR_UPDATE_SET(penalty, read)
GC_INT_ATTR_GET(penalty, read)

/* penalty_write */
GC_INT_ATTR_UPDATE_SET(penalty, write)
GC_INT_ATTR_GET(penalty, write)

/* stat_transaction */
GC_INT_ATTR_SET(stat, transaction)
GC_INT_ATTR_GET(stat, transaction)

/* stat_dev_data_read */
GC_INT_ATTR_SET(stat, dev_data_read)
GC_INT_ATTR_GET(stat, dev_data_read)

/* stat_dev_data_write */
GC_INT_ATTR_SET(stat, dev_data_write)
GC_INT_ATTR_GET(stat, dev_data_write)

/* stat_c_dev_data_read */
GC_INT_ATTR_SET(stat, c_dev_data_read)
GC_INT_ATTR_GET(stat, c_dev_data_read)

/* stat_c_dev_data_write */
GC_INT_ATTR_SET(stat, c_dev_data_write)
GC_INT_ATTR_GET(stat, c_dev_data_write)

/* stat_uc_data_read */
GC_INT_ATTR_SET(stat, uc_data_read)
GC_INT_ATTR_GET(stat, uc_data_read)

/* stat_uc_data_write */
GC_INT_ATTR_SET(stat, uc_data_write)
GC_INT_ATTR_GET(stat, uc_data_write)

/* stat_uc_inst_fetch */
GC_INT_ATTR_SET(stat, uc_inst_fetch)
GC_INT_ATTR_GET(stat, uc_inst_fetch)

/* stat_data_read */
GC_INT_ATTR_SET(stat, data_read)
GC_INT_ATTR_GET(stat, data_read)

/* stat_data_read_miss */
GC_INT_ATTR_SET(stat, data_read_miss)
GC_INT_ATTR_GET(stat, data_read_miss)

/* stat_data_write */
GC_INT_ATTR_SET(stat, data_write)
GC_INT_ATTR_GET(stat, data_write)

/* stat_data_write_miss */
GC_INT_ATTR_SET(stat, data_write_miss)
GC_INT_ATTR_GET(stat, data_write_miss)

/* stat_inst_fetch */
GC_INT_ATTR_SET(stat, inst_fetch)
GC_INT_ATTR_GET(stat, inst_fetch)

/* stat_ins_fetch_miss */
GC_INT_ATTR_SET(stat, inst_fetch_miss)
GC_INT_ATTR_GET(stat, inst_fetch_miss)

/* stat_copy_back */
GC_INT_ATTR_SET(stat, copy_back)
GC_INT_ATTR_GET(stat, copy_back)

/* timing_model */
static set_error_t
set_timing_model(void *dont_care, conf_object_t *obj,
                 attr_value_t *val, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        if (SIM_attr_is_nil(*val)) {
                gc->timing_model = NULL;
                gc->timing_ifc = NULL;

        } else {
                gc->timing_ifc = SIM_c_get_interface(SIM_attr_object(*val),
                                                     TIMING_MODEL_INTERFACE);
                if (!gc->timing_ifc) {
                        SIM_LOG_ERROR(
                                &gc->obj, GC_Log_Attr,
                                "generic-cache::set_timing_model: object '%s' "
                                "does not provide the timing model interface.",
                                SIM_object_name(SIM_attr_object(*val)));
                        return Sim_Set_Interface_Not_Found;
                }

                gc->timing_model = SIM_attr_object(*val);
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_timing_model(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *)obj;
        
        return SIM_make_attr_object(gc->timing_model);
}

static void
set_single_prof(generic_cache_t *gc, int i, attr_value_t *val)
{
        if (SIM_attr_is_nil(*val))
                goto prof_nil;
        
        conf_object_t *oval = SIM_attr_object(*val);
        const data_profiler_interface_t *dpi;
        dpi = SIM_c_get_interface(oval, DATA_PROFILER_INTERFACE);
        if (!dpi)
                goto prof_nil;

        gc->prof[i].obj = oval;
        gc->prof[i].id = dpi->get_prof_data(oval);
        return;

  prof_nil:
        gc->prof[i].obj = 0;
        gc->prof[i].id = 0;
}

static set_error_t                                              
set_prof(void *dont_care, conf_object_t *obj,       
              attr_value_t *val, attr_value_t *idx)      
{                                                               
        generic_cache_t *gc = (generic_cache_t *) obj;          

        if (!idx || !SIM_attr_is_integer(*idx)) {
                
                if (SIM_attr_list_size(*val) != GC_Max_Profilers)
                        return Sim_Set_Illegal_Value;

                for (unsigned i = 0; i < GC_Max_Profilers; i++) {
                        attr_value_t e = SIM_attr_list_item(*val, i);
                        set_single_prof(gc, i, &e);
                }

        } else {
                set_single_prof(gc, SIM_attr_integer(*idx), val);
        }

        return Sim_Set_Ok;                   
}

static attr_value_t                                         
get_prof(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{                                                          
        generic_cache_t *gc = (generic_cache_t *) obj;     

        if (!idx || !SIM_attr_is_integer(*idx)) {

                attr_value_t ret = SIM_alloc_attr_list(GC_Max_Profilers);

                for (unsigned i = 0; i < GC_Max_Profilers; i++) {
                        
                        if (gc->prof[i].obj) {
                                SIM_attr_list_set_item(
                                        &ret, i,
                                        SIM_make_attr_object(gc->prof[i].obj));

                        } else
                                SIM_attr_list_set_item(
                                        &ret, i, SIM_make_attr_nil());
                }
                
                return ret;
        }

        /* just one element */
        if (gc->prof[SIM_attr_integer(*idx)].obj) {
                return SIM_make_attr_object(
                        gc->prof[SIM_attr_integer(*idx)].obj);
        }

        return SIM_make_attr_nil();
}

/* registering attributes */
void
gc_common_register(conf_class_t *gc_class)
{
        SIM_register_typed_attribute(
                gc_class, "cpus",
                get_cpus, 0,
                set_cpus, 0,
                Sim_Attr_Required,
                "n|o|[o*]", NULL,
                "cpus that can send transactions to the cache.");

        SIM_register_typed_attribute(
                gc_class, "config_line_number",
                get_config_line_number, 0,
                set_config_line_number, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of lines in the cache (default is 128)");

        SIM_register_typed_attribute(
                gc_class, "config_line_size",
                get_config_line_size, 0,
                set_config_line_size, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Cache line size (must be a power of 2, default is 32). "
                "If you plan to use the STC in combination to "
                "improve the cache performance, this value "
                "must be greater or equal to the "
                "instruction_profile_line_size.");

        SIM_register_typed_attribute(
                gc_class, "config_assoc",
                get_config_assoc, 0,
                set_config_assoc, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Cache associativity. Note that the total number of lines "
                "divided by the associativity must be a power of 2. "
                "(default is 4).");

        SIM_register_typed_attribute(
                gc_class, "config_virtual_index",
                get_config_virtual_index, 0,
                set_config_virtual_index, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Address used to compute the set in the cache "
                "(0: physical, 1: virtual; default is 0).");

        SIM_register_typed_attribute(
                gc_class, "config_virtual_tag",
                get_config_virtual_tag, 0,
                set_config_virtual_tag, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Address used to compute the tag of the cache line "
                "(0: physical, 1: virtual; default is 0).");

        SIM_register_typed_attribute(
                gc_class, "config_write_allocate",
                get_config_write_allocate, 0,
                set_config_write_allocate, 0,
                Sim_Attr_Optional,
                "i", 0,
                "Write allocation policy "
                "(0: non-write allocate, 1: write-allocate, "
                "default is 0).");

        SIM_register_typed_attribute(
                gc_class, "config_write_back",
                get_config_write_back, 0,
                set_config_write_back, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Write policy (0: write-through, 1: write-back, "
                "default is 0).");

        SIM_register_typed_attribute(
                gc_class, "config_replacement_policy",
                get_config_repl, 0,
                set_config_repl, 0,
                Sim_Attr_Optional,
                "s", NULL,
                "Replacement policy (\"random\", \"lru\" or \"cyclic\", "
                "default is \"random\").");

        SIM_register_typed_attribute(
                gc_class, "penalty_read",
                get_penalty_read, 0,
                set_penalty_read, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall penalty (in cycles) for any incoming read transaction."
                " A cache-hit on read will only suffer"
                " a 'read' penalty (default is 0). Note that if you set this"
                " to a non-zero value, the simulation won't be able to use"
                " the STCs.");

        SIM_register_typed_attribute(
                gc_class, "penalty_write",
                get_penalty_write, 0,
                set_penalty_write, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall penalty (in cycles) for any incoming write transaction. "
                "A cache-hit on write (in a write-back cache) will only suffer "
                "a 'write' penalty (default is 0). Note that if you set this "
                "to a non-zero value, the simulation won't be able to use the "
                "STCs.");

        SIM_register_typed_attribute(
                gc_class, "stat_transaction",
                get_stat_transaction, 0,
                set_stat_transaction, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Total number of transactions seen by the cache.");

        SIM_register_typed_attribute(
                gc_class, "stat_dev_data_read",
                get_stat_dev_data_read, 0,
                set_stat_dev_data_read, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of device data read transactions (DMA).");

        SIM_register_typed_attribute(
                gc_class, "stat_dev_data_write",
                get_stat_dev_data_write, 0,
                set_stat_dev_data_write, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of device data write transactions (DMA).");

        SIM_register_typed_attribute(
                gc_class, "stat_c_dev_data_read",
                get_stat_c_dev_data_read, 0,
                set_stat_c_dev_data_read, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cached device data read transactions (DMA).");

        SIM_register_typed_attribute(
                gc_class, "stat_c_dev_data_write",
                get_stat_c_dev_data_write, 0,
                set_stat_c_dev_data_write, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cached device data write transactions (DMA).");

        SIM_register_typed_attribute(
                gc_class, "stat_uc_data_read",
                get_stat_uc_data_read, 0,
                set_stat_uc_data_read, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of uncacheable data read transactions.");

        SIM_register_typed_attribute(
                gc_class, "stat_uc_data_write",
                get_stat_uc_data_write, 0,
                set_stat_uc_data_write, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of uncacheable data write transactions.");

        SIM_register_typed_attribute(
                gc_class, "stat_uc_inst_fetch",
                get_stat_uc_inst_fetch, 0,
                set_stat_uc_inst_fetch, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of uncacheable inst fetch transactions.");

        SIM_register_typed_attribute(
                gc_class, "stat_data_read",
                get_stat_data_read, 0,
                set_stat_data_read, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable data read transactions "
                "(may be underestimated if the STCs are used, see "
                "Simics Analyzer Guide for more information).");

        SIM_register_typed_attribute(
                gc_class, "stat_data_read_miss",
                get_stat_data_read_miss, 0,
                set_stat_data_read_miss, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable data read transactions "
                "that missed in the cache.");

        SIM_register_typed_attribute(
                gc_class, "stat_data_write",
                get_stat_data_write, 0,
                set_stat_data_write, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable data write transactions "
                "(may be underestimated if the STCs are used, see "
                "Simics Analyzer Guide for more information).");

        SIM_register_typed_attribute(
                gc_class, "stat_data_write_miss",
                get_stat_data_write_miss, 0,
                set_stat_data_write_miss, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable data write transactions "
                "that missed in the cache (write-through caches report "
                "a correct miss value but all writes are sent to the "
                "next level).");
                
        SIM_register_typed_attribute(
                gc_class, "stat_inst_fetch",
                get_stat_inst_fetch, 0,
                set_stat_inst_fetch, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable instruction fetches "
                "(may be underestimated if the STCs are used, see "
                "Simics Analyzer Guide for more information).");

        SIM_register_typed_attribute(
                gc_class, "stat_inst_fetch_miss",
                get_stat_inst_fetch_miss, 0,
                set_stat_inst_fetch_miss, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable instruction fetches "
                "that missed in the cache.");

        SIM_register_typed_attribute(
                gc_class, "stat_copy_back",
                get_stat_copy_back, 0,
                set_stat_copy_back, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of copy-back transactions initiated by the cache.");

        SIM_register_typed_attribute(
                gc_class, "timing_model",
                get_timing_model, 0,
                set_timing_model, 0,
                Sim_Attr_Optional,
                "o|n", NULL,
                "Object listening on transactions coming from "
                "the cache (line fetch, copy-back).");

        SIM_register_typed_attribute(
                gc_class, "profilers",
                get_prof, 0,
                set_prof, 0,
                Sim_Attr_Pseudo | Sim_Attr_Integer_Indexed,
                "[o|n*]", "o|n",
                "Profilers connected to the cache.");
}
