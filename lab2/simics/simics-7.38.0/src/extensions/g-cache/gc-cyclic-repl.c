/*
  gc-cyclic-repl.c - cyclic replacement implementation for all caches

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

typedef struct cycle_data {
        uint32 *next_line_in_set;
} cyclic_data_t;

static const char *
cyclic_get_name()
{
        return "cyclic";
}

static void
cyclic_update_repl(void *data, generic_cache_t *gc, generic_transaction_t *gt, 
                   int line_num)
{
}

static int
cyclic_get_line(void *data, generic_cache_t *gc, generic_transaction_t *gt)
{
        cyclic_data_t *d = (cyclic_data_t *) data;
        int index = GC_INDEX(gc, gt);
        int line_num =  d->next_line_in_set[index];

        /* increase to next line in set */
        d->next_line_in_set[index] = 
                ((line_num + 1) >= ((index + 1) * gc->config.assoc))
                ? index * gc->config.assoc
                : (line_num + 1);
        
        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl, 
                     "cyclic::get_line: got line %d", line_num);
        return line_num;
}

static void *
cyclic_new_instance(generic_cache_t *gc)
{
        cyclic_data_t *d;
        int i;

        d = MM_MALLOC(1, cyclic_data_t);
        d->next_line_in_set = MM_MALLOC(gc->config.next_assoc, uint32);
        for (i=0; i<gc->config.next_assoc; i++)
                d->next_line_in_set[i] = i * gc->config.assoc;
        return d;
}

static void
cyclic_update_config(void *data, generic_cache_t *gc) 
{
        cyclic_data_t *d = (cyclic_data_t *) data;

        MM_FREE(d->next_line_in_set);
        d->next_line_in_set = MM_MALLOC(gc->config.next_assoc, uint32);
        for (unsigned i = 0; i < gc->config.next_assoc; i++)
                d->next_line_in_set[i] = i * gc->config.assoc;
}

static set_error_t                                              
set_next_line_in_set(void *dont_care, conf_object_t *obj,       
                      attr_value_t *val, attr_value_t *idx)      
{                                                               
        generic_cache_t *gc = (generic_cache_t *) obj;          

        if (strcmp(gc->config.repl_fun->get_name(), cyclic_get_name()) == 0) {
                cyclic_data_t *d = (cyclic_data_t *) gc->config.repl_data;

                if (SIM_attr_list_size(*val) != gc->config.next_assoc) {
                        SIM_LOG_ERROR(&gc->obj, GC_Log_Repl,
                                      "generic-cache::next_line: "
                                      "this cache does not have the "
                                      "right number of set for loading "
                                      "the cyclic values.");
                        return Sim_Set_Illegal_Value;
                }

                for (unsigned i = 0; i < gc->config.next_assoc; i++)
                        d->next_line_in_set[i] = SIM_attr_integer(
                                SIM_attr_list_item(*val, i));

                return Sim_Set_Ok;                   
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "Cyclic replacement policy is not in use.");
        return Sim_Set_Ok;
}

static attr_value_t                                         
get_next_line_in_set(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{                                                          
        generic_cache_t *gc = (generic_cache_t *) obj;     

        if (strcmp(gc->config.repl_fun->get_name(), cyclic_get_name()) == 0) {
                cyclic_data_t *d = (cyclic_data_t *) gc->config.repl_data;

                attr_value_t ret = SIM_alloc_attr_list(gc->config.next_assoc);
                for (unsigned i = 0; i < gc->config.next_assoc; i++)
                        SIM_attr_list_set_item(
                                &ret, i,
                                SIM_make_attr_uint64(
                                        d->next_line_in_set[i]));
                return ret;
        }

        SIM_LOG_INFO(2, &gc->obj, GC_Log_Repl,
                     "Cyclic replacement policy is not in use.");
        return SIM_make_attr_list(0);
}

const repl_interface_t *
init_cyclic_repl(conf_class_t *gc_class)
{
        /* config_seed */
        SIM_register_typed_attribute(
                gc_class, "next_line_in_set",
                get_next_line_in_set, 0,
                set_next_line_in_set, 0,
                Sim_Attr_Optional,
                "[i*]", NULL,
                "Next line used for replacement in a given set.");

        static const repl_interface_t cyclic_i = {
                .new_instance  = cyclic_new_instance,
                .update_config = cyclic_update_config,
                .get_name      = cyclic_get_name,
                .update_repl   = cyclic_update_repl,
                .get_line      = cyclic_get_line
        };

        return &cyclic_i;
}
