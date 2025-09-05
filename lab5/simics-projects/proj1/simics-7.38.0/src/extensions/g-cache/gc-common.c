/*
  gc-common.c - common functions (all versions)

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

#include "gc-common.h"
#include "gc.h"

const char *const gc_log_groups[] = {
        "Read Hit Path", 
        "Read Miss Path", 
        "Write Hit Path", 
        "Write Miss Path", 
        "Misc. Cache Activities", 
        "Replacement Policies", 
        "MESI Protocol", 
        "Attributes",
        "Outstanding Transactions",
        NULL
};

static VECT(const repl_interface_t *) repl_policies = VNULL;

/* register a new replacement policy */
void
add_repl_policy(const repl_interface_t *nw)
{
        VADD(repl_policies, nw);
}

uint32
ilog2(uint32 new_value)
{
        int i;
        for (i = 0; i < 32; i++) {
                new_value >>= 1;
                if (new_value == 0)
                        break;
        }
        return i;
}

/* called when a new cache is created */
conf_object_t *
gc_alloc_object(void *data)
{
        generic_cache_t *gc = MM_ZALLOC(1, generic_cache_t);
        return &gc->obj;
}

void *
gc_init_object(conf_object_t *obj, void *data)
{
        generic_cache_t *gc = (generic_cache_t *)obj;

        /* set default values in the cache */
        gc_init_cache(gc);

        return gc;
}

void
gc_finalize_instance(conf_object_t *cache)
{
        generic_cache_t *gc = (generic_cache_t *) cache;
        update_precomputed_values(gc);
}


void
gc_set_config_line_number(generic_cache_t *gc, int line_number)
{
        gc->config.line_number = line_number;

        /* re-allocate the lines */
        MM_FREE(gc->lines);
        gc->lines = MM_ZALLOC(gc->config.line_number, cache_line_t);
        
        update_precomputed_values(gc);
}

int
gc_set_config_repl(generic_cache_t *gc, const char *repl)
{
        if (!VLEN(repl_policies)) {
                SIM_LOG_ERROR(&gc->obj, GC_Log_Repl, 
                              "Cache has no replacement policies registered.");
                return -1;
        }
        
        const repl_interface_t *ri;
        int i = 0;
        do {
                ri = VGET(repl_policies, i);
                if (strcmp(repl, ri->get_name()) == 0) {
                        MM_FREE(gc->config.repl_data);
                        gc->config.repl_fun = ri;
                        gc->config.repl_data = 
                                gc->config.repl_fun->new_instance(gc);
                        gc->config.repl_fun->update_config(
                                gc->config.repl_data, gc);
                        return 0;
                }
                i++;
        } while (i < VLEN(repl_policies));

        SIM_LOG_INFO(1, &gc->obj, GC_Log_Repl,
                     "replacement: possible values are :");

        const repl_interface_t **r;
        VFOREACH(repl_policies, r) {
                SIM_LOG_INFO(1, &gc->obj, GC_Log_Repl, "   %s",
                             (*r)->get_name());
        }
        return -1;
}
