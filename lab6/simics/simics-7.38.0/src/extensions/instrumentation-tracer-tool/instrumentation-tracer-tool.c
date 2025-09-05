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

#include <simics/simulator-api.h>
#include "instrumentation-tracer-tool.h"
#include "connection.h"

#include <stdio.h>

FORCE_INLINE tracer_tool_t *
obj_to_tracer(conf_object_t *obj)
{
        return (tracer_tool_t *)obj;
}

#define ENABLE(option, desc)                         \
        tool-> option = true;

static conf_object_t *
alloc_object(void *arg)
{
        tracer_tool_t *tool = MM_ZALLOC(1, tracer_tool_t);
        return &tool->obj;
}

static connection_t *
new_connection(tracer_tool_t *tt, conf_object_t *cpu, attr_value_t attr)
{
        strbuf_t sb = SB_INIT;
        sb_addfmt(&sb, "%s.connection_%d", SIM_object_name(&tt->obj),
                  tt->next_connection_number);
        conf_object_t *c = SIM_create_object(connection_class, sb_str(&sb),
                                             attr);
        sb_free(&sb);
        
        if (!c)
                return NULL;

        tt->next_connection_number++;
        connection_t *con = obj_to_con(c);
        con->cpu = cpu;
        con->tracer = tt;
        
        con->pi_iface = SIM_C_GET_INTERFACE(cpu, processor_info);
        con->ci_iface = SIM_C_GET_INTERFACE(cpu, cpu_instrumentation_subscribe);
        con->iq_iface = SIM_C_GET_INTERFACE(cpu, cpu_instruction_query);
        con->mq_iface = SIM_C_GET_INTERFACE(cpu, cpu_memory_query);
        con->eq_iface = SIM_C_GET_INTERFACE(cpu, cpu_exception_query);        
        con->x86_iq_iface = SIM_C_GET_INTERFACE(cpu, x86_instruction_query);
        con->x86_mq_iface = SIM_C_GET_INTERFACE(cpu, x86_memory_query);
        con->x86_eq_iface = SIM_C_GET_INTERFACE(cpu, x86_exception_query);
        con->x86_at_iface = SIM_C_GET_INTERFACE(cpu, x86_access_type);
        con->ex_iface = SIM_C_GET_INTERFACE(cpu, exception);
        con->ir_iface = SIM_C_GET_INTERFACE(cpu, int_register);

        con->pa_digits = (con->pi_iface->get_physical_address_width(cpu)+3) >> 2;
        con->va_digits = (con->pi_iface->get_logical_address_width(cpu)+3) >> 2;
        sb_init(&con->last_line);

        // disable linear address printing for non-x86 cpus
        if (!con->x86_iq_iface)
                con->print_linear_address = false; 
        if (!con->x86_mq_iface) {
                con->print_memory_type = false;
                con->print_access_type = false;
        }
        
        con->id = SIM_get_processor_number(cpu);
        return con;
}

static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *cpu, attr_value_t attr)
{
        tracer_tool_t *tt = obj_to_tracer(obj);
        connection_t *con = new_connection(tt, cpu, attr);

        if (!con)
                return NULL;
        
        VADD(tt->connections, &con->obj);
        configure_connection(con);
        return &con->obj;
}

static void
it_disconnect(conf_object_t *obj, conf_object_t *con_obj)
{
        tracer_tool_t *tool = obj_to_tracer(obj);
        VREMOVE_FIRST_MATCH(tool->connections, con_obj);
        SIM_delete_object(con_obj);
}

static int
it_delete_instance(conf_object_t *obj)
{
        tracer_tool_t *tool = obj_to_tracer(obj);
        if (tool->fh)
                fclose(tool->fh);

        ASSERT_FMT(VEMPTY(tool->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(obj);
        return 0;
}

static attr_value_t
get_file(void *param, conf_object_t *obj, attr_value_t *idx)
{
        tracer_tool_t *tool = obj_to_tracer(obj);
        return SIM_make_attr_string(tool->file);
}


static void
close_file(tracer_tool_t *tool)
{
        if (tool->fh) {
                fclose(tool->fh);
                tool->fh = NULL;
        }
}

static set_error_t
set_file(void *param, conf_object_t *obj, attr_value_t *val,
           attr_value_t *idx)
{
        tracer_tool_t *tool = obj_to_tracer(obj);
        if (SIM_attr_is_nil(*val)) {
                close_file(tool);
                MM_FREE(tool->file);
                tool->file = NULL;
        } else {
                close_file(tool);
                tool->file = MM_STRDUP(SIM_attr_string(*val));
                tool->fh = fopen(tool->file, "w");
                if (!tool->fh) {
                        printf("Cannot open file");
                        return Sim_Set_Illegal_Value;
                }
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_msg_fmt(conf_object_t *obj, int type, char *msg, int level,
            uint64 groups)
{
        attr_value_t args = SIM_make_attr_list(
                6,
                SIM_make_attr_object(obj),
                SIM_make_attr_int64(type),
                SIM_make_attr_string(msg),
                SIM_make_attr_int64(level),
                SIM_make_attr_int64(groups),
                SIM_make_attr_boolean(false)); // log print type
        attr_value_t ret = VT_call_python_module_function(
                "log_commands", "get_log_string_exported", &args);
        SIM_attr_free(&args);
        return ret;
}

static bool
prevents_logging(trace_log_info_t *li, int type, int level, uint64 group_ids)
{
        uint32 li_types = li->types == 0 ? ~0 : li->types;
        uint64 li_groups = li->groups == 0 ? ~0ULL : li->groups;

        return level > li->level
                || !((1 << type) & li_types)
                || !(group_ids & li_groups);
}

static void
log_cb(lang_void *callback_data, conf_object_t *obj, int type, char *message,
       int level, int64 group_ids)
{
        trace_log_info_t *li = callback_data;
        if (prevents_logging(li, type, level, group_ids))
                return;

        tracer_tool_t *tt = li->tool;
        strbuf_t sb = SB_INIT;
        attr_value_t msg = get_msg_fmt(obj, type, message, level, group_ids);
        sb_addfmt(&sb, "log:  [%9llu] %s\n",
                  tt->log_cnt++, SIM_attr_string(msg));
        SIM_attr_free(&msg);
        if (tt->fh) {
                sb_write(&sb, tt->fh);
        } else {
                SIM_printf("%s", sb_str(&sb));
        }
}

static set_error_t
set_logging(conf_object_t *obj, attr_value_t *attr)
{
        tracer_tool_t *tool = obj_to_tracer(obj);

        VFOREACH_T(tool->logging, trace_log_info_t, li)
                SIM_hap_delete_callback_obj_id(
                        "Core_Log_Message_Always", li->obj, li->hap_id);
        VCLEAR(tool->logging);

        int len = SIM_attr_list_size(*attr);
        for (int i = 0; i < len; i++) {
                attr_value_t li = SIM_attr_list_item(*attr, i);
                conf_object_t *lo = SIM_attr_object(SIM_attr_list_item(li, 0));
                trace_log_info_t tli = {
                        .obj = lo,
                        .level = SIM_attr_integer(SIM_attr_list_item(li, 1)),
                        .types = SIM_attr_integer(SIM_attr_list_item(li, 2)),
                        .groups = SIM_attr_integer(SIM_attr_list_item(li, 3)),
                        .tool = tool
                };
                VADD(tool->logging, tli);
        }
        VFOREACH_T(tool->logging, trace_log_info_t, li) {
                li->hap_id = SIM_hap_add_callback_obj(
                        "Core_Log_Message_Always", li->obj, 0,
                        (obj_hap_func_t)log_cb, li);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_logging(conf_object_t *obj)
{
        tracer_tool_t *tool = obj_to_tracer(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(tool->logging));
        int i = 0;
        VFOREACH_T(tool->logging, trace_log_info_t, li) {
                attr_value_t item = SIM_make_attr_list(
                        4,
                        SIM_make_attr_object(li->obj),
                        SIM_make_attr_int64(li->level),
                        SIM_make_attr_int64(li->types),
                        SIM_make_attr_int64(li->groups));
                SIM_attr_list_set_item(&ret, i, item);
                i++;
        }
        return ret;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = it_delete_instance,
                .class_desc = "instr, data, and exception tracer",
                .description = "The tracer tool prints every executed"
                " instruction, all data accesses, and also all the exceptions"
                " that occurs in the connected processors. The data that is"
                " printed can be controlled by various arguments to the"
                " new-tracer-tool. The output can also be written to a file.",
                .kind = Sim_Class_Kind_Pseudo
        };
        conf_class_t *cl = SIM_register_class("tracer_tool", &funcs);

        static const instrumentation_tool_interface_t it = {
                .connect = it_connect,
                .disconnect = it_disconnect,
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it);

        SIM_register_typed_attribute(
                cl, "file",
                get_file, NULL,
                set_file, NULL,
                Sim_Attr_Optional,
                "s|n", NULL,
                "Output file");

        SIM_register_attribute(cl, "logging",
                               get_logging,
                               set_logging,
                               Sim_Attr_Pseudo,
                               "[[oiii]*]", "logging");

        init_connection();
}


