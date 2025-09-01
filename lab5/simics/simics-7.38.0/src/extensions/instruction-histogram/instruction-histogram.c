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

#include "instruction-histogram.h"
#include "generic-view.h"
#include <simics/base/log.h>
#include <simics/simulator/conf-object.h>

/* The tool object only implements some interfaces and attributes. 
   The actual data is kept in the sub-objects created when the tool is
   connected to providers. */
typedef struct {
        conf_object_t obj;

        int conn_counter;       /* increments each connection */
        view_type_t view;       /* Used view (sticky per tool) */
        /* All connections currently established */
        VECT(conf_object_t *) connections;
} instruction_histogram_t;

/* String names for the view_type_t enum */
static const char *view_names[] = {
        FOR_VIEWS(STRING)
};

static instruction_histogram_t *
instruction_histogram_of_obj(conf_object_t *obj)
{
        return (instruction_histogram_t *) obj;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *provider, attr_value_t args)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);        
        conf_object_t *sub_obj = new_connection(
                &ih->obj, provider, ih->conn_counter, ih->view, args);

        if (!sub_obj)
                return NULL;

        VADD(ih->connections, sub_obj);
        ih->conn_counter++;
        return sub_obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *con_obj)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);
        VREMOVE_FIRST_MATCH(ih->connections, con_obj);
        SIM_delete_object(con_obj);        
}

static conf_object_t *
alloc_object(lang_void *arg)
{
        instruction_histogram_t *ih = MM_ZALLOC(
                1, instruction_histogram_t);

        VINIT(ih->connections);
        return &ih->obj;
}

/* Cleanup of deleted object instance */
static int
delete_instance(conf_object_t *obj)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);
        ASSERT_FMT(VEMPTY(ih->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(ih);
        return 0;
}

static attr_value_t
get_view_attr(void *param, conf_object_t *obj, attr_value_t *idx)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);
        return SIM_make_attr_string(view_names[ih->view]);
}


static set_error_t
set_view_attr(void *arg, conf_object_t *obj, attr_value_t *val,
              attr_value_t *idx)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);
        const char *v = SIM_attr_string(*val);
        for (int i = 0; i < ALEN(view_names); i++) {
                if (strcmp(v, view_names[i]) == 0) {
                        ih->view = i;
                        return Sim_Set_Ok;
                }
        }
        return Sim_Set_Illegal_Value;
}

static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        instruction_histogram_t *ih = instruction_histogram_of_obj(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(ih->connections));
        for (int i = 0; i < VLEN(ih->connections); i++) {
                conf_object_t *o = VGET(ih->connections, i);
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_object(o));
        }
        return ret;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,
                .class_desc = "creation of instruction histograms",
                .description = "The instruction histogram tool can be used "
                " to collect information about all instructions executed in"
                " the system. You can choose between"
                " different views: mnemonic, size, x86-normalized"
                " (x86) and xed-iform (x86). See the view argument to the"
                " new-instruction-histogram command. The gathered information"
                " is displayed by the histogram command in the tool.",
                .kind = Sim_Class_Kind_Pseudo, };

        conf_class_t *cl = SIM_register_class(
                "instruction_histogram",
                &funcs);
        
        static const instrumentation_tool_interface_t it = {
                .connect = it_connect,
                .disconnect = it_disconnect
        };
        SIM_register_interface(cl, INSTRUMENTATION_TOOL_INTERFACE, &it);

        SIM_register_typed_attribute(
                cl, "view",
                get_view_attr, NULL,
                set_view_attr, NULL,
                Sim_Attr_Pseudo,
                "s", NULL,
                "The 'view' used for the instruction histogram.");
        
        SIM_register_typed_attribute(
                cl, "connections",
                get_connections, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "[o*]", NULL,
                "List of connection sub-objects used.");

        init_generic_view();
}
