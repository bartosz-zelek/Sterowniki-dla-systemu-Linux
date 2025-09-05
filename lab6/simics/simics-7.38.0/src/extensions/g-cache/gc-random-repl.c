/*
  gc-random-repl.c - random replacement implementation for all caches

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

typedef struct rand_data {
        uint32 seed;
} rand_data_t;

static int
rand_val(uint32 *seed_addr)
{
        uint32 tmp;
        tmp = (*seed_addr + 13) * 61147;
        *seed_addr = tmp;
        return (tmp >> 16);
}

/* return a random cache line in a set */
#define GC_RAND_REPLACE(gc, data) \
        (rand_val(&(data)->seed) % (gc)->config.assoc)

static const char *
rand_get_name()
{
        return "random";
}

static void
rand_update_repl(void *data, generic_cache_t *gc, generic_transaction_t *gt, 
                 int line_num)
{
}

static int
rand_get_line(void *data, generic_cache_t *gc, generic_transaction_t *gt)
{
        rand_data_t *d = (rand_data_t *) data;

        int line_num =  (GC_INDEX(gc, gt) * gc->config.assoc) + GC_RAND_REPLACE(gc, d);
        SIM_LOG_INFO(3, &gc->obj, GC_Log_Repl,
                     "get_line::random: got line %d", line_num);
        return line_num;
}

static void *
rand_new_instance(generic_cache_t *gc)
{
        rand_data_t *d;

        d = MM_ZALLOC(1, rand_data_t);
        return d;
}

static void
rand_update_config(void *data, generic_cache_t *gc)
{
}

static set_error_t                                              
set_config_seed(void *dont_care, conf_object_t *obj,       
                attr_value_t *val, attr_value_t *idx)      
{                                                               
        generic_cache_t *gc = (generic_cache_t *) obj;          

        if (strcmp(gc->config.repl_fun->get_name(), rand_get_name()) == 0) {
                rand_data_t *d = (rand_data_t *) gc->config.repl_data;
                d->seed = SIM_attr_integer(*val);
                return Sim_Set_Ok;                   
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "Random replacement policy is not in use.");
        return Sim_Set_Illegal_Value;
}

static attr_value_t                                         
get_config_seed(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{                                                          
        generic_cache_t *gc = (generic_cache_t *) obj;     

        if (strcmp(gc->config.repl_fun->get_name(), rand_get_name()) == 0) {
                rand_data_t *d = (rand_data_t *) gc->config.repl_data;
                return SIM_make_attr_uint64(d->seed);
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "Random replacement policy is not in use.");
        return SIM_make_attr_invalid();
}

const repl_interface_t *
init_rand_repl(conf_class_t *gc_class)
{
        /* config_seed */
        SIM_register_typed_attribute(
                gc_class, "config_seed",
                get_config_seed, 0,
                set_config_seed, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Seed for random replacement policy, default is 0).");

        static const repl_interface_t rand_i = {
                .new_instance  = rand_new_instance,
                .update_config = rand_update_config,
                .get_name      = rand_get_name,
                .update_repl   = rand_update_repl,
                .get_line      = rand_get_line
        };

        return &rand_i;
}
