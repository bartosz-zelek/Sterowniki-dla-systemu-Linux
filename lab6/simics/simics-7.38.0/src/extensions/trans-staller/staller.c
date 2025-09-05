/*
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
 * This class implements a simple staller. If a transactions is
 * stallable, it will return a fixed stall time. It can be used for a
 * simple memory simulation at the end of a cache hierarchy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>

#include <simics/model-iface/timing-model.h>

typedef struct simple_staller {
        conf_object_t obj;
        int stall_time;
} simple_staller_t;

static conf_object_t *
st_alloc_object(void *data)
{
        simple_staller_t *st = MM_ZALLOC(1, simple_staller_t);
        return &st->obj;
}

static void *
st_init_object(conf_object_t *obj, void *data)
{
        simple_staller_t *st = (simple_staller_t *)obj;
        return st;
}


static cycles_t
st_operate(conf_object_t *mem_hier, conf_object_t *space, 
           map_list_t *map, generic_transaction_t *mem_op)
{
        simple_staller_t *st = (simple_staller_t *) mem_hier;
        
        mem_op->reissue = 0;
        mem_op->block_STC = 1;
        if (SIM_mem_op_may_stall(mem_op)) {
//                pr("Stalling\n");
                return st->stall_time;
        } else
                return 0;
}

/* cpu  */
static set_error_t
set_stall_time(void *dont_care, conf_object_t *obj,
               attr_value_t *val, attr_value_t *idx)
{
        simple_staller_t *st = (simple_staller_t *) obj;
        
        st->stall_time = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t                                             
get_stall_time(void *dont_care, conf_object_t *obj,       
               attr_value_t *idx)                         
{                                                               
        simple_staller_t *st = (simple_staller_t *) obj;          
        return SIM_make_attr_uint64(st->stall_time);
}

static void
st_register(conf_class_t *st_class)
{
        SIM_register_typed_attribute(
                st_class, "stall_time",
                get_stall_time, 0,
                set_stall_time, 0,
                Sim_Attr_Optional,
                "i", NULL,
                "Stall time returned when accessed");
}

void
init_local()
{
        const class_data_t st_data = {
                .alloc_object = st_alloc_object,
                .init_object = st_init_object,
                .class_desc = "simple transaction staller",
                .description =
                "This class implements a simple transaction staller. If a"
                " transactions is stallable, it will return a fixed stall"
                " time. It can be used for a simple memory simulation at the"
                " end of a cache hierarchy." };

        conf_class_t *st_class;
        if (!(st_class = SIM_register_class("trans-staller",
                                            &st_data))) {
                pr_err("Could not create trans-staller class\n");
                return;
        }

        /* register the attributes */
        st_register(st_class);

        /* register the timing model interface */
        static const timing_model_interface_t st_ifc = {
                .operate = st_operate };
        SIM_register_interface(st_class, "timing_model",
                               (void *) &st_ifc);
}
