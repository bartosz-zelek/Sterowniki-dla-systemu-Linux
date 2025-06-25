/*
  id-splitter.c - Instruction-Data transaction splitter.

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
  <add id="simics sparc module short">
  <name index="true">id splitter</name>
  The id splitter module splits up memory operations into separate
  data and instruction streams. Data operations are forwarded to
  the timing interface of the object specified by the dbranch attribute and,
  in the same manner, instruction operations are forwarded to the ibranch.
  </add>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simics/simulator-api.h>
#include <simics/model-iface/timing-model.h>

typedef struct {
        conf_object_t obj;

        conf_object_t *ibranch;
        conf_object_t *dbranch;

        timing_model_interface_t dbranch_if, ibranch_if;
} id_splitter_t;

static cycles_t
id_splitter_operate(conf_object_t *mem_hier, conf_object_t *space,
                    map_list_t *map, generic_transaction_t *mem_trans)
{
        id_splitter_t *ids = (id_splitter_t *)mem_hier;

        if (SIM_mem_op_is_data(mem_trans)) {
                if (ids->dbranch)
                        return ids->dbranch_if.operate(
                                ids->dbranch, space, map, mem_trans);
        } else {
                if (ids->ibranch)
                        return ids->ibranch_if.operate(
                                ids->ibranch, space, map, mem_trans);
        }
        return 0;
}

static set_error_t
set_dbranch(void *dont_care, conf_object_t *obj, attr_value_t *val, 
            attr_value_t *idx)
{
        id_splitter_t *ids = (id_splitter_t *)obj;

        ids->dbranch = SIM_attr_object(*val);
        
        const timing_model_interface_t *timing_interface =
                SIM_c_get_interface(ids->dbranch, TIMING_MODEL_INTERFACE);
        if (!timing_interface) {
                pr("object `%s' has no timing interface\n",
                   SIM_object_name(ids->dbranch));
                return Sim_Set_Interface_Not_Found;
        }
        ids->dbranch_if = *timing_interface;
        
        if (ids->dbranch_if.operate == NULL) {
                pr("object `%s' doesn't export the operate function!\n",
                   SIM_object_name(ids->dbranch));
                return Sim_Set_Interface_Not_Found;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_dbranch(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        id_splitter_t *ids = (id_splitter_t *)obj;
        return SIM_make_attr_object(ids->dbranch);
}

static set_error_t
set_ibranch(void *dont_care, conf_object_t *obj,
            attr_value_t *val, attr_value_t *idx)
{
        id_splitter_t *ids = (id_splitter_t *)obj;

        ids->ibranch = SIM_attr_object(*val);
        
        const timing_model_interface_t *timing_interface =
                SIM_c_get_interface(ids->ibranch, TIMING_MODEL_INTERFACE);
        if (!timing_interface) {
                pr("object `%s' has no timing interface\n",
                   SIM_object_name(ids->ibranch));
                return Sim_Set_Interface_Not_Found;
        }
        ids->ibranch_if = *timing_interface;
        
        if (ids->ibranch_if.operate == NULL) {
                pr("object `%s' doesn't export the operate function!\n",
                   SIM_object_name(ids->ibranch));
                return Sim_Set_Interface_Not_Found;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_ibranch(void *dont_care, conf_object_t *obj, attr_value_t *idx)
{
        id_splitter_t *ids = (id_splitter_t *)obj;
        return SIM_make_attr_object(ids->ibranch);
}

static conf_object_t *
id_splitter_alloc_object(void *data)
{
        id_splitter_t *ids = MM_ZALLOC(1, id_splitter_t);
        return &ids->obj;
}

static void *
id_splitter_init_object(conf_object_t *obj, void *data)
{
        id_splitter_t *ids = (id_splitter_t *)obj;

        return ids;
}

void
init_local()
{         
        const class_data_t funcs = {
                .alloc_object = id_splitter_alloc_object,
                .init_object = id_splitter_init_object,
                .class_desc =
                "splits mem-ops into data and instr streams",
                .description = "The id splitter module splits up memory"
                " operations into separate data and instruction streams. Data"
                " operations are forwarded to the timing interface of the"
                " object specified by the dbranch attribute and, in the same"
                " manner, instruction operations are forwarded to the ibranch."
        };
        conf_class_t *id_splitter_class =
                SIM_register_class("id-splitter", &funcs);

        /* set up custom interfaces */
        static const timing_model_interface_t timing_interface = {
                .operate = id_splitter_operate
        };
        SIM_register_interface(id_splitter_class, "timing_model",
                               &timing_interface);

        SIM_register_typed_attribute(id_splitter_class, "ibranch",
                                     get_ibranch, NULL, set_ibranch, NULL,
                                     Sim_Attr_Optional, "o", NULL,
                                     "Object to receive instruction "
                                     "transactions.");

        SIM_register_typed_attribute(id_splitter_class, "dbranch",
                                     get_dbranch, NULL, set_dbranch, NULL,
                                     Sim_Attr_Optional, "o", NULL,
                                     "Object to receive data transactions.");

}
