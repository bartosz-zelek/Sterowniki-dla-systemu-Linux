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

conf_class_t *connection_class;

static conf_object_t *
new_connection(void *arg)
{
        connection_t *c = MM_ZALLOC(1, connection_t);
        return &c->obj;
}

static int
delete_connection(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;
}

//
static void
before_read(conf_object_t *connection,
            bank_before_read_interface_t *before_read,
            bank_access_t *handle,
            lang_void *user_data)
{
        before_read->inquire(handle);
}

static void
before_write(conf_object_t *connection,
             bank_before_write_interface_t *before_write,
             bank_access_t *handle,
             lang_void *user_data)
{
        before_write->suppress(handle);
}

static void
after_read(conf_object_t *connection,
           bank_after_read_interface_t *after_read,
           bank_access_t *handle,
           lang_void *user_data)
{
        connection_t *c = (connection_t *)connection;
        // TODO: we should check that offset/size matches between transaction
        // and connection; if not, mask/shift c->value accordingly, and/or
        // perform non-inquiry reads on bits not covered by the connection.
        after_read->set_value(handle, c->value);
        after_read->set_missed(handle, c->inject ? true : false);
        if (c->inject) {
                SIM_LOG_SPEC_VIOLATION(
                        1, c->bank, 0,
                        "%llu byte read access at offset 0x%llu outside registers"
                        " due to hole injected by %s",
                        after_read->size(handle), after_read->offset(handle),
                        SIM_object_name(&c->obj));
        }
}

static void
after_write(conf_object_t *connection,
            bank_after_write_interface_t *after_write,
            bank_access_t *handle,
            lang_void *user_data)
{
        connection_t *c = (connection_t *)connection;
        after_write->set_missed(handle, c->inject ? true : false);
        if (c->inject) {
                SIM_LOG_SPEC_VIOLATION(
                        1, c->bank, 0,
                        "%llu byte write access at offset 0x%llu outside registers"
                        " due to hole injected by %s",
                        after_write->size(handle), after_write->offset(handle),
                        SIM_object_name(&c->obj));
        }
}

void
subscribe(connection_t *connection)
{
        connection->provider->register_before_read(
                connection->bank, &connection->obj, connection->offset,
                connection->size, before_read, NULL);

        connection->provider->register_after_read(
                connection->bank, &connection->obj, connection->offset,
                connection->size, after_read, NULL);

        connection->provider->register_before_write(
                connection->bank, &connection->obj, connection->offset,
                connection->size, before_write, NULL);

        connection->provider->register_after_write(
                connection->bank, &connection->obj, connection->offset,
                connection->size, after_write, NULL);
}

static void
unsubscribe(connection_t *connection)
{
        connection->provider->remove_connection_callbacks(
                connection->bank, &connection->obj);
}

static void
pre_delete_connection(conf_object_t *obj)
{
        unsubscribe(connection(obj));
}

static void
enable(conf_object_t *obj)
{
        subscribe(connection(obj));
}

static void
disable(conf_object_t *obj)
{
        unsubscribe(connection(obj));
}

#define GET_SET_OPTION(OPTION, DESC, GETTER, SETTER, ATTR_SYMBOL) \
static attr_value_t                                               \
get_##OPTION(void *arg, conf_object_t *obj, attr_value_t *idx)    \
{                                                                 \
        return GETTER(connection(obj)->OPTION);                   \
}                                                                 \
static set_error_t                                                \
set_##OPTION(void *arg, conf_object_t *obj, attr_value_t *val,    \
             attr_value_t *idx)                                   \
{                                                                 \
        connection(obj)->OPTION = SETTER(*val);                   \
        return Sim_Set_Ok;                                        \
}

#define REGISTER_OPTION(OPTION, DESC, GETTER, SETTER, ATTR_SYMBOL) \
        SIM_register_typed_attribute(                              \
                connection_class, #OPTION,                         \
                get_##OPTION, NULL,                                \
                set_##OPTION, NULL,                                \
                Sim_Attr_Optional | Sim_Attr_Read_Only,            \
                ATTR_SYMBOL, NULL,                                 \
                DESC ".");

#define FOR_INTEGERS(OP)                                \
        OP(offset, "The offset address",                \
           SIM_make_attr_uint64, SIM_attr_integer, "i") \
        OP(size, "The range size",                      \
           SIM_make_attr_uint64, SIM_attr_integer, "i") \
        OP(value, "Mocked value",                       \
           SIM_make_attr_uint64, SIM_attr_integer, "i")

#define FOR_BOOLEANS(OP)                                                \
        OP(inject, "Treat read accesses in range like missed accesses", \
           SIM_make_attr_boolean, SIM_attr_boolean, "b")

FOR_INTEGERS(GET_SET_OPTION);
FOR_BOOLEANS(GET_SET_OPTION);

void
init_connection()
{
        const class_data_t class_data = {
                .alloc_object = new_connection,
                .delete_instance = delete_connection,
                .description = "Patch tool connection",
                .kind = Sim_Class_Kind_Pseudo,
                .class_desc = "patch tool connection",
                .pre_delete_instance = pre_delete_connection,
        };
        connection_class = SIM_register_class("bank_patch_tool_connection",
                                              &class_data);

        static const instrumentation_connection_interface_t
                connection_interface = {
                .enable = enable,
                .disable = disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection,
                               &connection_interface);

        FOR_INTEGERS(REGISTER_OPTION);
        FOR_BOOLEANS(REGISTER_OPTION);
}
