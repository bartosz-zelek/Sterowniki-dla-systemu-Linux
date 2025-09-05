/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "sample-core-timing.h"
#include <simics/simulator-iface/instrumentation-tool.h>

FORCE_INLINE sample_core_timing_tool_t *
tool_of_obj(conf_object_t *obj) { return (sample_core_timing_tool_t *)obj; }

static conf_object_t *
it_alloc(void *arg)
{
        sample_core_timing_tool_t *tool = MM_ZALLOC(1, sample_core_timing_tool_t);
        VINIT(tool->connections);
        VINIT(tool->instruction_classes);

        // Set default values for cycles and activity settings
        tool->background_activity_per_cycle = 0.0;
        tool->base_cycles_per_instruction = 1.0;
        tool->base_activity_per_instruction = 0.0;
        tool->read_extra_cycles = 0.0;
        tool->write_extra_cycles = 0.0;
        tool->read_extra_activity = 0.0;
        tool->write_extra_activity = 0.0;

        return &tool->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);
        ASSERT_FMT(VEMPTY(tool->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(tool);
        return 0;
}

static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *cpu, attr_value_t args)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);
        conf_object_t *conn_obj = sct_new_connection(tool, cpu, args);
        if (!conn_obj)
                return NULL;

        VADD(tool->connections, conn_obj);

        return conn_obj;
}

static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);

        sct_shutdown_connection(conn_obj);

        VREMOVE_FIRST_MATCH(tool->connections, conn_obj);
        SIM_delete_object(conn_obj);
}

static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);

        attr_value_t ret = SIM_alloc_attr_list(VLEN(tool->connections));
        VFORI(tool->connections, i) {
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_object(VGET(tool->connections, i)));
        }
        return ret;
}


static attr_value_t
get_instruction_class(instruction_class_t const *ic)
{
        return SIM_make_attr_list(3,
                                  SIM_make_attr_string(ic->pattern),
                                  SIM_make_attr_floating(ic->extra_cycles),
                                  SIM_make_attr_floating(ic->extra_activity));
}

static attr_value_t
get_instruction_classes(conf_object_t *obj)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);
        int nr_events = sct_get_nr_dynamic_events(tool);
        attr_value_t value = SIM_alloc_attr_list(nr_events);
        VFORI(tool->instruction_classes, i) {
                instruction_class_t ic = VGET(tool->instruction_classes, i);
                SIM_attr_list_set_item(&value, i,
                                       get_instruction_class(&ic));
        }
        return value;
}

static set_error_t
set_instruction_classes(conf_object_t *obj, attr_value_t *value)
{
        sample_core_timing_tool_t *tool = tool_of_obj(obj);

        VFORT(tool->instruction_classes, instruction_class_t, icls) {
                MM_FREE(icls.pattern);
        }
        VCLEAR(tool->instruction_classes);

        int nr_classes = SIM_attr_list_size(*value);
        for (int i = 0; i < nr_classes; ++i) {
                attr_value_t classitem = SIM_attr_list_item(*value, i);
                const char *pattern
                        = SIM_attr_string(SIM_attr_list_item(classitem, 0));
                double extra_cycles
                        = SIM_attr_floating(SIM_attr_list_item(classitem, 1));
                double extra_activity
                        = SIM_attr_floating(SIM_attr_list_item(classitem, 2));

                instruction_class_t icls;
                icls.pattern = MM_STRDUP(pattern);
                icls.extra_cycles = extra_cycles;
                icls.extra_activity = extra_activity;

                VADD(tool->instruction_classes, icls);
        }

        VFORP(tool->connections, conf_object_t, conn_obj) {
                sct_allocate_dynamic_events(conn_obj);
        }

        return Sim_Set_Ok;
}

static attr_value_t
get_background_activity(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      background_activity_per_cycle);
}

static set_error_t
set_background_activity(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->background_activity_per_cycle =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_base_cycles(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      base_cycles_per_instruction);
}

static set_error_t
set_base_cycles(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->base_cycles_per_instruction =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_base_activity(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      base_activity_per_instruction);
}

static set_error_t
set_base_activity(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->base_activity_per_instruction =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_read_cycles(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      read_extra_cycles);
}

static set_error_t
set_read_cycles(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->read_extra_cycles =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_read_activity(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      read_extra_activity);
}

static set_error_t
set_read_activity(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->read_extra_activity =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_write_cycles(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      write_extra_cycles);
}

static set_error_t
set_write_cycles(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->write_extra_cycles =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}

static attr_value_t
get_write_activity(conf_object_t *obj)
{
        return SIM_make_attr_floating(tool_of_obj(obj)->
                                      write_extra_activity);
}

static set_error_t
set_write_activity(conf_object_t *obj, attr_value_t *value)
{
        tool_of_obj(obj)->write_extra_activity =
                SIM_attr_floating(*value);
        return Sim_Set_Ok;
}


static void
init_tool_class()
{
        const class_data_t funcs = {
                .alloc_object = it_alloc,
                .delete_instance = it_delete_connection,
                .description = "Sample x86 performance tool.",
                .class_desc = "sample x86 performance tool",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("sample_core_timing_tool", &funcs);

        static const instrumentation_tool_interface_t it_iface = {
                .connect = it_connect,
                .disconnect = it_disconnect
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it_iface);

        /* USER-TODO: Register tool specific attributes */
        SIM_register_typed_attribute(
               cl, "connections",
               get_connections, NULL,
               NULL, NULL,
               Sim_Attr_Pseudo,
               "[o*]", NULL,
               "Example attribute, returns the connections");

        SIM_register_attribute(
               cl, "instruction_classes",
               get_instruction_classes,
               set_instruction_classes,
               Sim_Attr_Pseudo,
               "[[sff]*]",
               "List of instruction classes."
               " Each class consists of a string to look for,"
               " fraction of extra cycles per instance and"
               " extra activity per instance");

        SIM_register_attribute(
                cl, "background_activity",
                get_background_activity,
                set_background_activity,
                Sim_Attr_Pseudo,
                "f",
                "Background activity per cycle");

        SIM_register_attribute(
               cl, "base_cycles",
               get_base_cycles,
               set_base_cycles,
               Sim_Attr_Pseudo,
               "f",
               "Baseline cycles per instruction");

        SIM_register_attribute(
               cl, "base_activity",
               get_base_activity,
               set_base_activity,
               Sim_Attr_Pseudo,
               "f",
               "Baseline activity per instruction");

        SIM_register_attribute(
               cl, "read_cycles",
               get_read_cycles,
               set_read_cycles,
               Sim_Attr_Pseudo,
               "f",
               "Extra cycles per explicit read access");

        SIM_register_attribute(
               cl, "write_cycles",
               get_write_cycles,
               set_write_cycles,
               Sim_Attr_Pseudo,
               "f",
               "Extra cycles per explicit write access");

        SIM_register_attribute(
               cl, "read_activity",
               get_read_activity,
               set_read_activity,
               Sim_Attr_Pseudo,
               "f",
               "Extra activity per explicit read access");

        SIM_register_attribute(
               cl, "write_activity",
               get_write_activity,
               set_write_activity,
               Sim_Attr_Pseudo,
               "f",
               "Extra activity per explicit write access");
}

void
init_local()
{
        init_tool_class();
        sct_init_connection_class();
}

