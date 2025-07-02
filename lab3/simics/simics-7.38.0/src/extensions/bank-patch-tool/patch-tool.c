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
#include <patch-tool.h>

#include <simics/simulator-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>

FORCE_INLINE patch_tool_t *
tool(conf_object_t *obj)
{
        return (patch_tool_t *)obj;
}

static conf_object_t *
new_tool(void *arg)
{
        patch_tool_t *tool = MM_ZALLOC(1, patch_tool_t);
        return &tool->obj;
}

static int
delete_tool(conf_object_t *obj)
{
        patch_tool_t *tracer = tool(obj);
        ASSERT_FMT(VEMPTY(tracer->connections),
                   "%s deleted with active connections",
                   SIM_object_name(obj));
        MM_FREE(obj);
        return 0;
}

static connection_t *
new_connection(patch_tool_t *tool, conf_object_t *bank, attr_value_t attr)
{
        strbuf_t name = SB_INIT;
        sb_addfmt(&name, "%s_%d",
                  SIM_object_name(&tool->obj), VLEN(tool->connections));
        conf_object_t *connection_obj = SIM_create_object(
                connection_class, sb_str(&name), attr);
        sb_free(&name);

        if (!connection_obj) {
                return NULL;
        }

        connection_t *c = connection(connection_obj);
        c->bank = bank;
        c->provider = SIM_C_GET_INTERFACE(bank, bank_instrumentation_subscribe);
        return c;
}

static conf_object_t *
connect(conf_object_t *obj, conf_object_t *bank, attr_value_t attr)
{
        patch_tool_t *tracer = tool(obj);
        connection_t *connection = new_connection(tracer, bank, attr);
        if (!connection) {
                return NULL;
        }

        VADD(tracer->connections, &connection->obj);
        subscribe(connection);
        return &connection->obj;
}

static void
disconnect(conf_object_t *obj, conf_object_t *connection)
{
        patch_tool_t *tracer = tool(obj);
        VREMOVE_FIRST_MATCH(tracer->connections, connection);
        SIM_delete_object(connection);
}

void
init_local()
{
        const class_data_t class_data = {
                .alloc_object = new_tool,
                .delete_instance = delete_tool,
                .class_desc = "device access miss repair tool",
                .description =
                "The bank patch tool is used to 'patch' bank regions and"
                " forgive accesses to those regions that would otherwise miss.",
                .kind = Sim_Class_Kind_Pseudo
        };
        conf_class_t *class = SIM_register_class("bank_patch_tool",
                                                 &class_data);

        static const instrumentation_tool_interface_t tool_interface = {
                .connect = connect,
                .disconnect = disconnect,
        };
        SIM_REGISTER_INTERFACE(class, instrumentation_tool, &tool_interface);

        init_connection();
}
