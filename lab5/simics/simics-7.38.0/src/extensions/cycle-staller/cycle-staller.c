/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

/* This module implements a simple way of adding extra stall cycles to
   processors. An interval can be given that tells how often the processors
   should insert the extra cycles. The stall_cycles_collector interface is used
   to add up the cycles before they are inserted. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/device-api.h>

#include <simics/model-iface/transaction.h>

#include "stall-cycles-collector-interface.h"

typedef struct clock_info {
        conf_object_t *clock;
        cycles_t stall_cycles;
        cycles_t total_cycles;
} clock_info_t;

typedef struct {
        /* Simics configuration object */
        conf_object_t obj;
        cycles_t interval;
        VECT(clock_info_t *) clock_infos;
} cycle_staller_t;

static event_class_t *event;

static void
add_cycles(conf_object_t *obj, conf_object_t *clock, cycles_t cycles)
{
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        VFORP(cs->clock_infos, clock_info_t, ci) {
                if (ci->clock == clock) {
                        SIM_LOG_INFO(2, obj, 0,
                                     "Adding %lld stall cycles for clock %s",
                                     cycles, SIM_object_name(clock));
                        ci->stall_cycles += cycles;
                        return;
                }
        }
        /* New clock */
        clock_info_t *ci = MM_MALLOC(1, clock_info_t);
        ci->clock = clock;
        ci->total_cycles = 0;
        ci->stall_cycles = cycles;

        SIM_LOG_INFO(2, obj, 0,
                     "Adding %lld stall cycles for new clock %s",
                     cycles, SIM_object_name(clock));

        VADD(cs->clock_infos, ci);
        SIM_event_post_cycle(clock, event, obj, cs->interval, ci);
}

static void
stall(conf_object_t *obj, lang_void *_ci)
{
        clock_info_t *ci = _ci;
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        SIM_LOG_INFO(4, obj, 0,
                     "Call to STALL with %lld stall cycles for clock %s",
                     ci->stall_cycles, SIM_object_name(ci->clock));
        if (ci->stall_cycles != 0) {
            SIM_stall_cycle(ci->clock, ci->stall_cycles);
            ci->total_cycles += ci->stall_cycles;
            ci->stall_cycles = 0;
        }

        SIM_event_post_cycle(ci->clock, event, obj, cs->interval, ci);
}

/* Allocate memory for the object. */
static conf_object_t *
alloc_object(void *data)
{
        cycle_staller_t *cs = MM_ZALLOC(1, cycle_staller_t);
        return &cs->obj;
}

/* Initialize the object before any attributes are set. */
static void *
init_object(conf_object_t *obj, void *data)
{
        return obj;
}

/* Finalize the object after attributes have been set, if needed. */
static void
finalize_instance(conf_object_t *obj)
{
}

static void
pre_delete_instance(conf_object_t *obj)
{
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        MM_FREE(cs);
        return 0;
}

static set_error_t
set_interval_attribute(conf_object_t *obj, attr_value_t *val)
{
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        cs->interval = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_interval_attribute(conf_object_t *obj)
{
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        return SIM_make_attr_uint64(cs->interval);
}

static attr_value_t
get_stall_info(conf_object_t *obj)
{
        cycle_staller_t *cs = (cycle_staller_t *)obj;
        attr_value_t ret = SIM_alloc_attr_list(VLEN(cs->clock_infos));
        VFORI(cs->clock_infos, i) {
                attr_value_t clock = SIM_make_attr_object(
                        VGET(cs->clock_infos, i)->clock);
                attr_value_t total = SIM_make_attr_int64(
                        VGET(cs->clock_infos, i)->total_cycles);
                attr_value_t stall = SIM_make_attr_int64(
                        VGET(cs->clock_infos, i)->stall_cycles);
                SIM_attr_list_set_item(&ret, i,
                        SIM_make_attr_list(3, clock, total, stall));
        }
        return ret;
}

/* called once when the device module is loaded into Simics */
void
init_local()
{
        /* Define and register the device class. */
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .init_object = init_object,
                .finalize_instance = finalize_instance,
                .pre_delete_instance = pre_delete_instance,
                .delete_instance = delete_instance,
                .class_desc  = "skeleton C code for devices",
                .description = "This is a long description of this class."
        };

        conf_class_t *class = SIM_register_class("cycle_staller", &funcs);

        static stall_cycles_collector_interface_t scc_iface = {
                .add = add_cycles
        };
        SIM_register_interface(class,
                               STALL_CYCLES_COLLECTOR_INTERFACE, &scc_iface);

        event = SIM_register_event("extra stall", class, 0, stall,
                                   NULL, NULL, NULL, NULL);

        SIM_register_attribute(
                class, "interval",
                get_interval_attribute, set_interval_attribute,
                Sim_Attr_Optional, "i",
                "How frequent extra stall cycles should be inserted.");

        SIM_register_attribute(
                class, "stall_info",
                get_stall_info, NULL,
                Sim_Attr_Pseudo, "[[oii]*]",
                "For each connected clock, [obj, total cycles, left cycles]"
                " is returned in a list.");
}
