/*
  gc-lru-repl.c - perfect LRU replacement implementation for all caches

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

/* cache includes */
#include "gc-common.h"
#include "gc.h"

typedef struct lru_data {
        int64 *last_used;
} lru_data_t;

static const char *
lru_get_name()
{
        return "lru";
}

static void
lru_update_repl(void *data, generic_cache_t *gc, generic_transaction_t *gt, 
                 int line_num)
{
        lru_data_t *d = (lru_data_t *) data;

#ifdef GC_USE_STC
        /* empty other lines from STC if we are using the STC from this cache */
        if (!gc->config.no_STC 
            && (gc->config.direct_read_listener
                || gc->config.direct_write_listener)) {
                /* Remove the previously most recent line from the STC */
                int k,j;
                uint64 newest = (uint64) -1;

                for (k = j = GC_INDEX(gc, gt) * gc->config.assoc;
                     k < (GC_INDEX(gc, gt) + 1) * gc->config.assoc; k++) {

                        if (newest < d->last_used[k]) {
                                newest = d->last_used[k];
                                j = k;
                        }
                }

                /* flush STC only if the line is there */
                if (gc->lines[j].STC_type)
                        flush_STC(gc, gc->lines[j].tag, gc->lines[j].otag,
                                  gc->lines[j].STC_type, gc->lines[j].status);
        }
#endif

         
        d->last_used[line_num] = SIM_cycle_count(GC_CPU(gt));

#ifdef GC_DEBUG
        if (SIM_clear_exception()) {
                SIM_LOG_ERROR(&gc->obj, GC_Log_Repl,
                              "generic-cache::lru_update_replacement:"
                              " got an exception in SIM_cycle_count()");
        }
#endif
}

static int
lru_get_line(void *data, generic_cache_t *gc, generic_transaction_t *gt)
{
        lru_data_t *d = (lru_data_t *) data;
        int i;
        int lru_line = GC_INDEX(gc, gt) * gc->config.assoc;
        uint64 lru_time = (uint64)-1;

        for (i = lru_line; 
             i < (GC_INDEX(gc, gt) + 1) * gc->config.assoc; i++)
                if (d->last_used[i] <= lru_time) {
                        lru_line = i;
                        lru_time = d->last_used[i];
                }
        
        SIM_LOG_INFO(3, &gc->obj, GC_Log_Repl,
                     "get_line::lru: got line %d", lru_line);
        return lru_line;
}

static void *
lru_new_instance(generic_cache_t *gc)
{
        lru_data_t *d;

        d = MM_ZALLOC(1, lru_data_t);
        d->last_used = MM_ZALLOC(gc->config.line_number, int64);
        return d;
}

static void
lru_update_config(void *data, generic_cache_t *gc)
{
        lru_data_t *d = (lru_data_t *) data;

        MM_FREE(d->last_used);
        d->last_used = MM_ZALLOC(gc->config.line_number, int64);
}

static void
lru_set_timestamp(void *data, generic_cache_t *gc,
                  int line_num, int64 timestamp)
{
        lru_data_t *d = (lru_data_t *) data;
        d->last_used[line_num] = timestamp;
}

static int64
lru_get_timestamp(void *data, generic_cache_t *gc,
                  int line_num)
{
        lru_data_t *d = (lru_data_t *) data;
        return d->last_used[line_num];
}


static set_error_t                                              
set_lines_last_used(void *dont_care, conf_object_t *obj,       
                    attr_value_t *val, attr_value_t *idx)      
{                                                               
        generic_cache_t *gc = (generic_cache_t *) obj;          

        if (strcmp(gc->config.repl_fun->get_name(), lru_get_name()) == 0) {
                lru_data_t *d = (lru_data_t *) gc->config.repl_data;

                if (SIM_attr_list_size(*val) != gc->config.line_number) {
                        SIM_LOG_ERROR(
                                &gc->obj, GC_Log_Attr,
                                "generic-cache::set_lines: this cache"
                                " does not have the right number of"
                                " lines for loading the lines.");
                        return Sim_Set_Illegal_Value;
                }

                for (unsigned i = 0; i < gc->config.line_number; i++)
                        d->last_used[i] = SIM_attr_integer(
                                SIM_attr_list_item(*val, i));

                return Sim_Set_Ok;                   
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "LRU replacement policy is not in use.");
        return Sim_Set_Ok;
}

static attr_value_t                                         
get_lines_last_used(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{                                                          
        generic_cache_t *gc = (generic_cache_t *) obj;     

        if (strcmp(gc->config.repl_fun->get_name(), lru_get_name()) == 0) {
                lru_data_t *d = (lru_data_t *) gc->config.repl_data;

                attr_value_t ret = SIM_alloc_attr_list(gc->config.line_number);
                for (unsigned i = 0; i < gc->config.line_number; i++)
                        SIM_attr_list_set_item(
                                &ret, i,
                                SIM_make_attr_uint64(d->last_used[i]));

                return ret;
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "LRU replacement policy is not in use.");
        return SIM_make_attr_list(0);
}

const repl_interface_t *
init_lru_repl(conf_class_t *gc_class)
{
        /* config_seed */
        SIM_register_typed_attribute(
                gc_class, "lines_last_used",
                get_lines_last_used, 0,
                set_lines_last_used, 0,
                Sim_Attr_Optional | Sim_Init_Phase_1,
                "[i*]", NULL,
                "Last used timestamp for the cache lines");
        
        static const repl_interface_t lru_i = {
                .new_instance  = lru_new_instance,
                .update_config = lru_update_config,
                .get_name      = lru_get_name,
                .update_repl   = lru_update_repl,
                .get_line      = lru_get_line,
                .set_timestamp = lru_set_timestamp,
                .get_timestamp = lru_get_timestamp
        };
        
        return &lru_i;
}
