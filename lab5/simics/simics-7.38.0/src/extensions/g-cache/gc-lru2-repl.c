/*
  gc-lru2-repl.c

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

#include "gc-lru2-repl.h"

static const char *
lru2_get_name()
{
        return "lru2";
}

static void
lru2_update_repl(void *data, generic_cache_t *gc, generic_transaction_t *gt, int line_num)
{
        int i;
        lru2_data_t *d = (lru2_data_t *)data;
        int index = GC_INDEX(gc, gt);
        uint64 *state_transitions = 
                &d->state_transitions[d->states[line_num] * gc->config.assoc];

        for (i = 0; i < gc->config.assoc; i++) {
                uint64 *states = 
                        &d->states[index * gc->config.assoc + i];
                if (*states >= gc->config.assoc) {
                        SIM_LOG_ERROR(&gc->obj, GC_Log_Repl,
                                      "Illegal line LRU2 state, setting it "
                                      "to least recently used.");
                        *states = gc->config.assoc - 1;
                }
                else {
                        if (*states == 0)
                                flush_STC(gc, 
                                          gc->lines[i].tag, 
                                          gc->lines[i].otag,
                                          gc->lines[i].STC_type, 
                                          gc->lines[i].status);
                        *states = state_transitions[*states];
                }
        }
        d->states[line_num] = 0;
}

static int
lru2_get_line(void *data, generic_cache_t *gc, generic_transaction_t *gt)
{
        int i;
        lru2_data_t *d = (lru2_data_t *)data;
        int line_num = GC_INDEX(gc, gt);
        uint64 lru_state = 0;

        for (i = GC_INDEX(gc, gt) * gc->config.assoc; i < (GC_INDEX(gc, gt) + 1) * gc->config.assoc; i++)
                if (d->states[i] >= lru_state) {
                        line_num = i;
                        lru_state = d->states[i];
                }

        SIM_LOG_INFO(3, &gc->obj, GC_Log_Repl,
                     "get_line::lru2: got line %d", line_num);
        return line_num;
}

static void lru2_update_config(void *data, generic_cache_t *gc);

static void *
lru2_new_instance(generic_cache_t *gc)
{
        lru2_data_t *d;

        d = MM_MALLOC(1, lru2_data_t);
        d->states = NULL;
        d->state_transitions = NULL;
        lru2_update_config(d, gc);
        return d;
}

static void
lru2_update_config(void *data, generic_cache_t *gc)
{
        int i;
        lru2_data_t *d = (lru2_data_t *) data;

        /* Initialize the LRU states. */
        d->states = MM_REALLOC(d->states, gc->config.line_number, uint64);

        for (i = 0; i < gc->config.next_assoc; i++) {
                int j = i * gc->config.assoc;
                int k;
                for (k = 0; k < gc->config.assoc; k++)
                        d->states[j + k] = k;
        }

        /* Initialize the state transition table. */
        d->state_transitions = MM_REALLOC(d->state_transitions, gc->config.assoc * gc->config.assoc, uint64);

        for (i = 0; i < gc->config.assoc; i++) {
                int j;
                for (j = 0; j < gc->config.assoc; j++)
                        d->state_transitions[i * gc->config.assoc + j] = j < i ? j + 1 : j;
        }
}

static set_error_t
set_lines_states(void *dont_care, conf_object_t *obj, attr_value_t *val, 
                 attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (strcmp(gc->config.repl_fun->get_name(), lru2_get_name()) == 0) {
                lru2_data_t *d = (lru2_data_t *) gc->config.repl_data;

                if (SIM_attr_list_size(*val) != gc->config.line_number) {
                        SIM_LOG_ERROR(&gc->obj, GC_Log_Repl, 
                                      "Cache line number mismatch "
                                      "when setting LRU states.");
                        return Sim_Set_Illegal_Value;
                }

                for (unsigned i = 0; i < gc->config.line_number; i++)
                        d->states[i] = SIM_attr_integer(
                                SIM_attr_list_item(*val, i));

                return Sim_Set_Ok;
        }
        
        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "LRU2 replacement policy is not in use.");
        return Sim_Set_Illegal_Value;
}

static attr_value_t
get_lines_states(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        generic_cache_t *gc = (generic_cache_t *) obj;

        if (strcmp(gc->config.repl_fun->get_name(), lru2_get_name()) == 0) {
                lru2_data_t *d = (lru2_data_t *) gc->config.repl_data;

                attr_value_t ret = SIM_alloc_attr_list(gc->config.line_number);
                for (unsigned i = 0; i < gc->config.line_number; i++)
                        SIM_attr_list_set_item(
                                &ret, i,
                                SIM_make_attr_uint64(d->states[i]));

                return ret;
        }

        SIM_LOG_INFO(4, &gc->obj, GC_Log_Repl,
                     "LRU2 replacement policy is not in use.");
        return SIM_make_attr_invalid();
}

const repl_interface_t *
init_lru2_repl(conf_class_t *gc_class)
{
        const repl_interface_t *lru2_i = MM_ZALLOC(1, repl_interface_t);

        SIM_register_typed_attribute(
                gc_class, "lines_states",
                get_lines_states, 0,
                set_lines_states, 0,
                Sim_Attr_Optional | Sim_Init_Phase_1,
                "[i*]", NULL,
                "LRU states of the cache lines.");

        lru2_i->new_instance = lru2_new_instance;
        lru2_i->update_config = lru2_update_config;
        lru2_i->get_name = lru2_get_name;
        lru2_i->update_repl = lru2_update_repl;
        lru2_i->get_line = lru2_get_line;

        return lru2_i;
}
