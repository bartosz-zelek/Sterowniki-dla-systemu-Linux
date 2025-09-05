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

#include "cache-tool.h"
#include "cache-connection.h"

#include <simics/simulator-api.h>
#include <stdio.h>

FORCE_INLINE simple_cache_tool_t *
obj_to_tool(conf_object_t *obj)
{
  return (simple_cache_tool_t *)obj;
}

/* Allocate the object */
static conf_object_t *
alloc_object(void *arg)
{
	simple_cache_tool_t *tool = MM_ZALLOC(1, simple_cache_tool_t);
	return &tool->obj;
}

/* Create a new connection - cache */
static connection_t *
new_cache_conn(simple_cache_tool_t *ct, conf_object_t *cpu, attr_value_t attr)
{
	strbuf_t sb = SB_INIT;
	sb_addfmt(&sb, "%s.con%d", SIM_object_name(&ct->obj),
			ct->next_conn_cache_number);
	conf_object_t *c = SIM_create_object(conn_class, sb_str(&sb), attr);
	sb_free(&sb);

	if (!c)
                return NULL;

	ct->next_conn_cache_number++;
	connection_t *conn = obj_to_conn(c);
	conn->provider = cpu;
        conn->tool = ct;

	conn->pi_iface = SIM_C_GET_INTERFACE(
                cpu, processor_info);

        conn->cis_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_instrumentation_subscribe);

        conn->ci_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_cached_instruction);

        conn->iq_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_instruction_query);

        conn->mq_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_memory_query);

        conn->xq_iface = SIM_C_GET_INTERFACE(
                cpu, x86_memory_query);
	return conn;
}

/* Initialize the connection */
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *cpu, attr_value_t attr)
{
	simple_cache_tool_t *et = obj_to_tool(obj);
	connection_t *conn = new_cache_conn(et, cpu, attr);

	if(!conn)
		return NULL;

	VADD(et->connections, &conn->obj);
	configure_conn(conn);
	return &conn->obj;
}

static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
	simple_cache_tool_t *tool = obj_to_tool(obj);
	VREMOVE_FIRST_MATCH(tool->connections, conn_obj);
	SIM_delete_object(conn_obj);
}

static int
it_delete_instance(conf_object_t *obj)
{
	simple_cache_tool_t *tool = obj_to_tool(obj);
	ASSERT_FMT(VEMPTY(tool->connections),
                   "%s deleted with active connections/caches",
                   SIM_object_name(obj));

        MM_FREE(obj);
	return 0;
}

static attr_value_t
get_connections(conf_object_t *obj)
{
        simple_cache_tool_t *tool = obj_to_tool(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(tool->connections));
        for (int i = 0; i < VLEN(tool->connections); i++) {
                conf_object_t *o = VGET(tool->connections, i);
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_object(o));
        }
        return ret;
}

static attr_value_t
get_cycle_staller(conf_object_t *obj)
{
        simple_cache_tool_t *tool = obj_to_tool(obj);
        return SIM_make_attr_object(tool->cycle_staller.obj);
}

static set_error_t
set_cycle_staller(conf_object_t *obj, attr_value_t *val)
{
        simple_cache_tool_t *tool = obj_to_tool(obj);
        tool->cycle_staller.obj = SIM_attr_object_or_nil(*val);
        if (!tool->cycle_staller.obj)
                return Sim_Set_Ok;

        tool->cycle_staller.iface = SIM_C_GET_INTERFACE(
                tool->cycle_staller.obj, stall_cycles_collector);

        if (!tool->cycle_staller.iface)
                return Sim_Set_Interface_Not_Found;
        return Sim_Set_Ok;
}

void
init_local()
{
	const class_data_t funcs = {
			.alloc_object = alloc_object,
			.delete_instance = it_delete_instance,
			.description = "Empty tool to create a connection",
                        .class_desc = "tool to create a connection",
			.kind = Sim_Class_Kind_Pseudo
	};

	conf_class_t *cl = SIM_register_class("simple_cache_tool", &funcs);

	static const instrumentation_tool_interface_t it = {
			.connect = it_connect,
			.disconnect = it_disconnect,
	};

	SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it);

        SIM_register_attribute(
                cl, "connections",
                get_connections,
                NULL,
                Sim_Attr_Pseudo,
                "[o*]",
                "List of connection sub-objects used.");

        SIM_register_attribute(
                cl, "cycle_staller",
                get_cycle_staller,
                set_cycle_staller,
                Sim_Attr_Pseudo,
                "o|n",
                "Cycle staller object.");

	init_conn();
        init_cache();

}
