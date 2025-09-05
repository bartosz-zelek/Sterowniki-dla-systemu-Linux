/*
  splitter.c

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/*
 * This class implements a transaction splitter. Transactions are split if they
 * are cacheable and span multiple cache lines in the next level cache. The
 * module does not support multiple outstanding transactions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>
#include <simics/model-iface/timing-model.h>

#include "gc-common.h"
#include "gc.h"

#define TS_CL_TAG(ts, addr) ((addr) >> (ts)->next_cache_line_size_log2)
#define TS_CL_ADDR(ts, addr) ((addr) & ~((ts)->next_cache_line_size - 1))

typedef struct trans_splitter {
        conf_object_t obj;

        /* statistics */
        int64 transactions;
        int64 split_transactions;

        conf_object_t *timing_model;
        const timing_model_interface_t *tm_ifc;

        int next_cache_line_size;
        int next_cache_line_size_log2;
        generic_cache_t *cache;
} trans_splitter_t;

static conf_object_t *
ts_alloc_object(void *data)
{
        trans_splitter_t *ts = MM_ZALLOC(1, trans_splitter_t);
        return &ts->obj;
}

static void *
ts_init_object(conf_object_t *obj, void *data)
{
        trans_splitter_t *ts = (trans_splitter_t *)obj;
        return ts;
}

static cycles_t
ts_operate(conf_object_t *mem_hier, conf_object_t *space, 
           map_list_t *map, generic_transaction_t *mem_op)
{
        trans_splitter_t *ts = (trans_splitter_t *) mem_hier;
        int total_stall_time;
        cache_memory_transaction_t trans;
        generic_address_t offset;

        ts->transactions++;

        /* Transactions that don't span multiple cache lines or are
         * uncacheable are not modified */
        if (likely(TS_CL_TAG(ts, SIM_get_mem_op_physical_address(mem_op)
                             == (TS_CL_TAG(
                                         ts,
                                         SIM_get_mem_op_physical_address(mem_op)
                                         + SIM_get_mem_op_size(mem_op) - 1))))
            || unlikely(is_uncacheable(ts->cache, mem_op, map, &ts->obj)))
                return ts->tm_ifc->operate(ts->timing_model, space, 
                                           map, mem_op);

        /* Here we have a cacheable cache line crossing transaction */
        ts->split_transactions++;

        total_stall_time = 0;
        trans.s = *mem_op;
        SIM_set_mem_op_initiator(&trans.s, Sim_Initiator_Cache, &ts->obj);
        
        /* we won't need reissue */
        mem_op->reissue = 0;

        /* update the initial_trans field */
        if ((SIM_get_mem_op_ini_type(mem_op) & 0xFF00)
            == Sim_Initiator_Cache) {
                cache_memory_transaction_t *ct =
                        (cache_memory_transaction_t *) mem_op;
                trans.initial_trans = ct->initial_trans;
        }
        else {
                trans.initial_trans = mem_op;
        }

        offset = 0;
        while (offset < SIM_get_mem_op_size(mem_op)) {
                int stall_time;
                SIM_set_mem_op_physical_address(
                        &trans.s,
                        SIM_get_mem_op_physical_address(mem_op) + offset);
                SIM_set_mem_op_virtual_address(
                        &trans.s,
                        SIM_get_mem_op_virtual_address(mem_op) + offset);
                trans.s.size = MIN(
                        SIM_get_mem_op_size(mem_op) - offset,
                        TS_CL_ADDR(ts,
                                   SIM_get_mem_op_physical_address(&trans.s)
                                   + ts->next_cache_line_size)
                        - SIM_get_mem_op_physical_address(&trans.s));
                stall_time = ts->tm_ifc->operate(ts->timing_model, space, map,
                                                 &trans.s);
                total_stall_time = MAX(total_stall_time, stall_time);
                if (SIM_get_mem_op_exception(&trans.s) != Sim_PE_No_Exception)
                        SIM_set_mem_op_exception(
                                mem_op,
                                SIM_get_mem_op_exception(&trans.s));
                offset += SIM_get_mem_op_size(&trans.s);
        }

        return total_stall_time;
}

/* timing model */
static set_error_t
set_timing_model(void *dont_care, conf_object_t *obj, attr_value_t *val, 
                 attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;

        if (SIM_attr_is_nil(*val)) {
                ts->tm_ifc = NULL;
                ts->timing_model = NULL;

        } else {
                ts->tm_ifc = SIM_c_get_interface(SIM_attr_object(*val), 
                                                 TIMING_MODEL_INTERFACE);
                if (!ts->tm_ifc) {
                        SIM_LOG_ERROR(&ts->obj, 0,
                                      "set_timing_model: object does not "
                                      "provide the timing model interface.");
                        return Sim_Set_Illegal_Value;
                }

                ts->timing_model = SIM_attr_object(*val);
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_timing_model(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        return SIM_make_attr_object(ts->timing_model);
}

/* next_cache_line_size */
static set_error_t
set_nc_line_size(void *dont_care, conf_object_t *obj, attr_value_t *val, 
                 attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        int64 ival = SIM_attr_integer(*val);

        if (((int64)1 << ilog2(ival)) != ival) {
                SIM_LOG_ERROR(&ts->obj, 0,
                              "next_cache_line_size: must be a power of 2");
                return Sim_Set_Illegal_Value;
        }

        ts->next_cache_line_size = ival;
        ts->next_cache_line_size_log2 = ilog2(ts->next_cache_line_size);

        return Sim_Set_Ok;
}

static attr_value_t
get_nc_line_size(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        return SIM_make_attr_uint64(ts->next_cache_line_size);
}

/* transactions */
static set_error_t
set_transactions(void *dont_care, conf_object_t *obj, attr_value_t *val, 
                 attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;

        ts->transactions = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_transactions(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        return SIM_make_attr_uint64(ts->transactions);
}

/* split_transactions */
static set_error_t
set_split_transactions(void *dont_care, conf_object_t *obj, attr_value_t *val, 
                       attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;

        ts->split_transactions = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_split_transactions(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        return SIM_make_attr_uint64(ts->split_transactions);
}

static set_error_t
set_cache(void *dont_care, conf_object_t *obj,
          attr_value_t *val, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;

        if (SIM_attr_is_nil(*val))
                ts->cache = NULL;
        else
                ts->cache = (generic_cache_t *) SIM_attr_object(*val);
        
        return Sim_Set_Ok;
}

static attr_value_t                                             
get_cache(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        trans_splitter_t *ts = (trans_splitter_t *) obj;
        return SIM_make_attr_object(&ts->cache->obj);
}

static void
ts_register(conf_class_t *ts_class)
{
        SIM_register_typed_attribute(
                ts_class, "cache",
                get_cache, 0,
                set_cache, 0,
                Sim_Attr_Required,
                "o|n", NULL,
                "Cache to which the splitter is connected.");

        SIM_register_typed_attribute(
                ts_class, "next_cache_line_size",
                get_nc_line_size, 0,
                set_nc_line_size, 0,
                Sim_Attr_Required,
                "i", NULL,
                "Cache line size used for splitting transactions.");

        SIM_register_typed_attribute(
                ts_class, "transactions",
                get_transactions, 0,
                set_transactions, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Total number of transactions processed.");

        SIM_register_typed_attribute(
                ts_class, "split_transactions",
                get_split_transactions, 0,
                set_split_transactions, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Number of split transactions.");

        SIM_register_typed_attribute(
                ts_class, "timing_model",
                get_timing_model, 0,
                set_timing_model, 0,
                Sim_Attr_Required,
                "o|n", NULL,
                "Object listening on transactions coming "
                "from the splitter.");
}

void
ts_init_local()
{
        const class_data_t ts_data = {
                .alloc_object = ts_alloc_object,
                .init_object = ts_init_object,
                .class_desc = "transaction splitter",
                .description =
                "A trans-splitter object should be inserted between two caches if the higher-level "
                "cache has a larger cache line size than the lower-level cache, or between the "
                "processor and the id-splitter object if you have a processor that can do "
                "unaligned accesses or accesses larger than one cache line. It splits cacheable "
                "transactions that span more than one naturally aligned next-cache-line-size bytes "
                "big lines into multiple transactions that each fit within one line."
        };

        conf_class_t *ts_class = SIM_register_class("trans-splitter", &ts_data);
        if (ts_class == NULL) {
                fputs("Could not create trans-splitter class\n", stderr);
                return;
        }

        /* register the attributes */
        ts_register(ts_class);

        /* register the timing model interface */
        static const timing_model_interface_t ts_ifc = {
                .operate = ts_operate
        };
        SIM_register_interface(ts_class, "timing_model", &ts_ifc);
}
