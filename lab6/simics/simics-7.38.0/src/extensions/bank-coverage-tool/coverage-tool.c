/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <connection.h>
#include <coverage-tool.h>

#include <simics/simulator-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>

static coverage_tool_t *
tool(conf_object_t *obj)
{
        return (coverage_tool_t *)obj;
}

static conf_object_t *
new_tool(void *arg)
{
        coverage_tool_t *tool = MM_ZALLOC(1, coverage_tool_t);
        return &tool->obj;
}

static int
delete_tool(conf_object_t *obj)
{
        coverage_tool_t *t = tool(obj);
        ASSERT(VEMPTY(t->connections));
        MM_FREE(obj);
        return 0;
}

static conf_object_t *
create_connection(coverage_tool_t *t, attr_value_t attr) {
        const char *tool_name = SIM_object_name(&t->obj);

        strbuf_t connection_name = SB_INIT;
        sb_addfmt(&connection_name, "%s_%d", tool_name, t->next_connection_num++);
        conf_object_t *connection = SIM_create_object(
                connection_class, sb_str(&connection_name), attr);

        sb_free(&connection_name);
        return connection;
}

static connection_t *
new_connection(coverage_tool_t *t, conf_object_t *bank, attr_value_t attr)
{
        conf_object_t *obj = create_connection(t, attr);
        if (!obj) {
                return NULL;
        }

        connection_t *c = connection(obj);
        c->bank = bank;
        c->subscribe = SIM_C_GET_INTERFACE(bank,
                                           bank_instrumentation_subscribe);
        return c;
}

static conf_object_t *
connect(conf_object_t *obj, conf_object_t *bank, attr_value_t attr)
{
        coverage_tool_t *t = tool(obj);
        connection_t *connection = new_connection(t, bank, attr);
        if (!connection) {
                return NULL;
        }

        VADD(t->connections, &connection->obj);
        subscribe(connection);
        return &connection->obj;
}

static void
disconnect(conf_object_t *obj, conf_object_t *connection)
{
        coverage_tool_t *t = tool(obj);
        VREMOVE_FIRST_MATCH(t->connections, connection);
        SIM_delete_object(connection);
}

static attr_value_t
get_connections(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        attr_value_t connections = SIM_alloc_attr_list(
                VLEN(tool(obj)->connections));
        VFORI(tool(obj)->connections, i) {
                SIM_attr_list_set_item(
                        &connections, i,
                        SIM_make_attr_object(VGET(tool(obj)->connections, i)));
        }
        return connections;
}

void
init_local()
{
        const class_data_t data = {
                .alloc_object = new_tool,
                .delete_instance = delete_tool,
                .class_desc = "register bank coverage tool",
                .description = "The bank coverage tool collects statistics on"
                " accessed registers in connected banks and compiles a device"
                " register coverage report. The tool may also display the"
                " number of accesses to registers on a per-bank basis.",
                .kind = Sim_Class_Kind_Pseudo
        };
        conf_class_t *class = SIM_register_class("bank_coverage_tool", &data);

        static const instrumentation_tool_interface_t tool_interface = {
                .connect = connect,
                .disconnect = disconnect,
        };
        SIM_REGISTER_INTERFACE(class, instrumentation_tool, &tool_interface);

        SIM_register_typed_attribute(class, "connections",
                                     get_connections, NULL,
                                     NULL, NULL,
                                     Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                                     "[o*]", NULL, "Connections.");

        init_connection();
}
