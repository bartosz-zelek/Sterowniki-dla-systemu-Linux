/*
  sorter.c

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <simics/simulator-api.h>

#include "gc-common.h"
#include "gc.h"

typedef enum {
        Trans_Cache_Mem   = 0,
        Trans_Cache_Dev   = 1,
        Trans_Uncache_Mem = 2,
        Trans_Uncache_Dev = 3,
        Trans_Max
} trans_sort_t;

typedef struct trans_sorter {

        conf_object_t obj;

        conf_object_t            *tm[Trans_Max];
        const timing_model_interface_t *ifc[Trans_Max];
        int                       transactions[Trans_Max];

        generic_cache_t          *cache;
} trans_sorter_t;

static conf_object_t *
ts_alloc_object(void *data)
{
        trans_sorter_t *dev = MM_ZALLOC(1, trans_sorter_t);
        return &dev->obj;
}

static void *
ts_init_object(conf_object_t *obj, void *data)
{
        trans_sorter_t *dev= (trans_sorter_t *)obj;
        return dev;
}

static cycles_t
ts_operate(conf_object_t *obj, conf_object_t *space, map_list_t *map, 
           generic_transaction_t *mem_op)
{
        trans_sorter_t *ts = (trans_sorter_t *) obj;
        int stall_time;
        int index = 0;

        if (is_uncacheable(ts->cache, mem_op, map, &ts->obj))
                index = 2;

        if ((SIM_get_mem_op_ini_type(mem_op) & 0xFF00)
            == Sim_Initiator_Device)
                index++;

        if (ts->ifc[index])
                stall_time = ts->ifc[index]->operate(ts->tm[index], space, 
                                                     map, mem_op);
        else 
                stall_time = 0;

        if (!stall_time || !mem_op->reissue)
                ts->transactions[index]++;

        return stall_time;
}

static set_error_t
set_cache(void *dont_care, conf_object_t *obj,
          attr_value_t *val, attr_value_t *idx)
{
        trans_sorter_t *ts = (trans_sorter_t *) obj;

        if (SIM_attr_is_nil(*val))
                ts->cache = NULL;
        else
                ts->cache = (generic_cache_t *) SIM_attr_object(*val);
        
        return Sim_Set_Ok;
}

static attr_value_t                                             
get_cache(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_sorter_t *ts = (trans_sorter_t *) obj;

        if (ts->cache) {
                return SIM_make_attr_object(&ts->cache->obj);
        }
        return SIM_make_attr_nil();
}

static set_error_t
set_tm(trans_sorter_t *ts, attr_value_t *val, int t)
{
        if (SIM_attr_is_nil(*val)) {
                ts->ifc[t] = NULL;
                ts->tm[t] = NULL;
        } else {
                ts->ifc[t] = SIM_c_get_interface(SIM_attr_object(*val), 
                                                 TIMING_MODEL_INTERFACE);
                if (!ts->ifc[t]) {
                        SIM_LOG_ERROR(&ts->obj, 0,
                                      "set_tm: object does not provide the "
                                      "timing model interface.");
                        return Sim_Set_Illegal_Value;
                }
        
                ts->tm[t] = SIM_attr_object(*val);
        }
        
        return Sim_Set_Ok;
}

static set_error_t
set_c_mem_tm(void *dont_care, conf_object_t *obj,
             attr_value_t *val, attr_value_t *idx)
{
        return set_tm((trans_sorter_t *) obj, val, Trans_Cache_Mem);
}

static set_error_t
set_c_dev_tm(void *dont_care, conf_object_t *obj,
             attr_value_t *val, attr_value_t *idx)
{
        return set_tm((trans_sorter_t *) obj, val, Trans_Cache_Dev);
}

static set_error_t
set_u_mem_tm(void *dont_care, conf_object_t *obj,
             attr_value_t *val, attr_value_t *idx)
{
        return set_tm((trans_sorter_t *) obj, val, Trans_Uncache_Mem);
}

static set_error_t
set_u_dev_tm(void *dont_care, conf_object_t *obj,
            attr_value_t *val, attr_value_t *idx)
{
        return set_tm((trans_sorter_t *) obj, val, Trans_Uncache_Dev);
}

static attr_value_t                                             
get_tm(trans_sorter_t *ts, trans_sort_t t)
{
        if (ts->tm[t]) {
                return SIM_make_attr_object(ts->tm[t]);
        }
        return SIM_make_attr_nil();
}

static attr_value_t                                             
get_c_mem_tm(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_tm((trans_sorter_t *) obj, Trans_Cache_Mem);
}

static attr_value_t                                             
get_c_dev_tm(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_tm((trans_sorter_t *) obj, Trans_Cache_Dev);
}

static attr_value_t                                             
get_u_mem_tm(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_tm((trans_sorter_t *) obj, Trans_Uncache_Mem);
}

static attr_value_t                                             
get_u_dev_tm(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_tm((trans_sorter_t *) obj, Trans_Uncache_Dev);
}

static set_error_t
set_stat(trans_sorter_t *ts, attr_value_t *val, int t)
{
        ts->transactions[t] = SIM_attr_integer(*val);

        return Sim_Set_Ok;
}

static set_error_t
set_c_mem_stat(void *dont_care, conf_object_t *obj,
               attr_value_t *val, attr_value_t *idx)
{
        return set_stat((trans_sorter_t *) obj, val, Trans_Cache_Mem);
}

static set_error_t
set_c_dev_stat(void *dont_care, conf_object_t *obj,
               attr_value_t *val, attr_value_t *idx)
{
        return set_stat((trans_sorter_t *) obj, val, Trans_Cache_Dev);
}

static set_error_t
set_u_mem_stat(void *dont_care, conf_object_t *obj,
               attr_value_t *val, attr_value_t *idx)
{
        return set_stat((trans_sorter_t *) obj, val, Trans_Uncache_Mem);
}

static set_error_t
set_u_dev_stat(void *dont_care, conf_object_t *obj,
               attr_value_t *val, attr_value_t *idx)
{
        return set_stat((trans_sorter_t *) obj, val, Trans_Uncache_Dev);
}

static attr_value_t                                             
get_stat(trans_sorter_t *ts, trans_sort_t t)
{
        return SIM_make_attr_uint64(ts->transactions[t]);
}

static attr_value_t                                             
get_c_mem_stat(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_stat((trans_sorter_t *) obj, Trans_Cache_Mem);
}

static attr_value_t                                             
get_c_dev_stat(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_stat((trans_sorter_t *) obj, Trans_Cache_Dev);
}

static attr_value_t                                             
get_u_mem_stat(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_stat((trans_sorter_t *) obj, Trans_Uncache_Mem);
}

static attr_value_t                                             
get_u_dev_stat(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        return get_stat((trans_sorter_t *) obj, Trans_Uncache_Dev);
}

void
sort_init_local()
{
        const class_data_t ts_data = {
                .alloc_object = ts_alloc_object,
                .init_object = ts_init_object,
                .class_desc = "divider of transactions in two flows",
                .description = 
                "A trans-sorter object divides the transactions in "
                "4 transaction flows according to two criteria: are they "
                "cacheable or not? Are they generated by the cpu or by a "
                "device? They are forwarded to the four timing models "
                "attributes corresponding to the four possibilities. "
                "This can be useful to ignore some types of transactions when "
                "tracing or connecting a memory hierarchy."
        };

        conf_class_t *ts_class = SIM_register_class("trans-sorter", &ts_data);
        if (ts_class == NULL) {
                pr_err("Could not create g-cache class\n");
                return;
        }

        /* initialize and register the timing-model interface */
        static const timing_model_interface_t ts_ifc = {
                .operate = ts_operate
        };
        SIM_register_interface(ts_class, "timing_model", &ts_ifc);

        /* initialize attributes */
        SIM_register_typed_attribute(
                ts_class, "cache", 
                get_cache, NULL, 
                set_cache, NULL, 
                Sim_Attr_Required,
                "o|n", NULL,
                "Cache to which the sorter is connected.");

        SIM_register_typed_attribute(
                ts_class, "cacheable_mem_timing_model", 
                get_c_mem_tm, NULL, 
                set_c_mem_tm, NULL, 
                Sim_Attr_Optional,
                "o|n", NULL,
                "Timing model which will receive cacheable "
                "memory transactions.");
        
        SIM_register_typed_attribute(
                ts_class, "cacheable_dev_timing_model", 
                get_c_dev_tm, NULL, 
                set_c_dev_tm, NULL, 
                Sim_Attr_Optional,
                "o|n", NULL,
                "Timing model which will receive cacheable "
                "device transactions.");

        SIM_register_typed_attribute(
                ts_class, "uncacheable_mem_timing_model", 
                get_u_mem_tm, NULL, 
                set_u_mem_tm, NULL, 
                Sim_Attr_Optional,
                "o|n", NULL,
                "Timing model which will receive uncacheable "
                "memory transactions.");
        
        SIM_register_typed_attribute(
                ts_class, "uncacheable_dev_timing_model", 
                get_u_dev_tm, NULL, 
                set_u_dev_tm, NULL, 
                Sim_Attr_Optional,
                "o|n", NULL,
                "Timing model which will receive uncacheable "
                "device transactions."); 

        SIM_register_typed_attribute(
                ts_class, "cacheable_mem_transactions", 
                get_c_mem_stat, NULL, 
                set_c_mem_stat, NULL, 
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable memory transactions.");
        
        SIM_register_typed_attribute(
                ts_class, "cacheable_dev_transactions", 
                get_c_dev_stat, NULL, 
                set_c_dev_stat, NULL, 
                Sim_Attr_Optional,
                "i", NULL,
                "Number of cacheable device transactions.");

        SIM_register_typed_attribute(
                ts_class, "uncacheable_mem_transactions", 
                get_u_mem_stat, NULL, 
                set_u_mem_stat, NULL, 
                Sim_Attr_Optional,
                "i", NULL,
                "Number of uncacheable memory transactions.");

        SIM_register_typed_attribute(
                ts_class, "uncacheable_dev_transactions", 
                get_u_dev_stat, NULL, 
                set_u_dev_stat, NULL, 
                Sim_Attr_Optional,
                "i", NULL,
                "Number of uncacheable device transactions."); 
}

