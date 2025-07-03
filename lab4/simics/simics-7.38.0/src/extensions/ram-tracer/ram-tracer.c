/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.


  Simple instrumentation tool that just counts how many time we execute a
  instruction with a specific length. Uses the low-cost add_counter method of
  the instrumentation_first_seen_interface.
*/

#include <simics/base/attr-value.h>
#include <simics/simulator/conf-object.h>
#include <simics/simulator-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/ram-instrumentation.h>

conf_class_t *connection_class;

typedef struct ram_tracer ram_tracer_t;

/* A connection */
typedef struct {
        conf_object_t obj;
        conf_object_t *ram;

        ram_cb_handle_t *after_handle;
        ram_cb_handle_t *filter_handle;

        access_t access; /* what is being traced */
        bool inquiry; /* handle inquiry accesses as well */
        bool block; /* only blocking using access, no tracing */
        
        /* Cached Interfaces */
        const ram_access_subscribe_interface_t *ras_if;
        ram_tracer_t *tracer_tool;
} connection_t;

/* The Simics object */
struct ram_tracer {
        conf_object_t obj;
        VECT(connection_t *) connections;
        int next_connection;
        char *filename;
        FILE *file;
};

FORCE_INLINE ram_tracer_t *
ram_tracer_of_obj(conf_object_t *obj) { return (ram_tracer_t *)obj; }

FORCE_INLINE connection_t *
conn_of_obj(conf_object_t *obj) { return (connection_t *)obj; }

/*************************** Connection Object ********************************/

// class_data_t(connection_t)::alloc_object
static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *conn = MM_ZALLOC(1, connection_t);
        return &conn->obj;
}

// class_data_t(connection_t)::pre_delete_instance
static void
ic_pre_delete_connection(conf_object_t *obj)
{
}

// class_data_t(connection_t)::delete_instance
static int
ic_delete_connection(conf_object_t *obj)
{
        MM_FREE(obj);
        return 0;
}

// instrumentation_connection::enable
static void
ic_enable(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        conn->ras_if->enable_callback(conn->ram, conn->after_handle);
}

// instrumentation_connection::disable
static void
ic_disable(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        conn->ras_if->disable_callback(conn->ram, conn->after_handle);
}

static access_t
filter_cb(conf_object_t *obj, conf_object_t *ram, conf_object_t *initiator,
         uint64 offset, unsigned size, access_t access,
         lang_void *user_data)
{
        connection_t *conn = conn_of_obj(obj);

        // allow caching of what we are not interested in
        return (~conn->access & 0x7);
}

FORCE_INLINE bool
trace_execute(connection_t *conn)
{
        return conn->access & Sim_Access_Execute;
}

FORCE_INLINE bool
trace_read(connection_t *conn)
{
        return conn->access & Sim_Access_Read;
}

FORCE_INLINE bool
trace_write(connection_t *conn)
{
        return conn->access & Sim_Access_Write;
}


static void
access_cb(conf_object_t *conn_obj, conf_object_t *ram, uint64 ram_offset,
          transaction_t *t,
          lang_void *user_data)
{
        connection_t *c = conn_of_obj(conn_obj);
        bool inquiry = SIM_transaction_flags(t) & Sim_Transaction_Inquiry;
        if (inquiry && !c->inquiry)
                return;

        const char *a = "?";
        if (SIM_transaction_is_fetch(t) && trace_execute(c)) {
                a = "Execute";
        } else if (SIM_transaction_is_read(t) && trace_read(c)) {
                a = "Read   ";
        } else  if (SIM_transaction_is_write(t) && trace_write(c)) {
                a = "Write  ";
        } else {
                return;
        }
        int size = SIM_transaction_size(t);
        uint8 bytes[size];
        buffer_t buf = { .len = size, .data = bytes };
        conf_object_t *initiator = SIM_transaction_initiator(t);

        SIM_get_transaction_bytes(t, buf);
        strbuf_t sb = SB_INIT;
        sb_addfmt(&sb, "[%s] <- %s %s %s 0x%016llx %4zd ",
               SIM_object_name(ram),
               initiator ? SIM_object_name(initiator): "User",
               inquiry ? "I" : " ",
               a,
               ram_offset,
               buf.len);

        for (int i = 0; i < buf.len; i++)
                sb_addfmt(&sb, "%02x ", buf.data[i]);
        sb_addfmt(&sb, "\n");
        if (c->tracer_tool->file) {
                sb_write(&sb, c->tracer_tool->file);
        } else {
                SIM_printf("%s", sb_str(&sb));
        }
        sb_free(&sb);
}

/* Help function which creates a new connection and returns it. */
static connection_t *
new_connection(ram_tracer_t *ram_tracer, conf_object_t *ram, attr_value_t args)
{
        strbuf_t sb = SB_INIT;
        
        sb_addfmt(&sb, "%s.con%d",
                  SIM_object_name(&ram_tracer->obj),
                  ram_tracer->next_connection);
        conf_object_t *conn_obj = SIM_create_object(
                connection_class, sb_str(&sb), args);
        sb_free(&sb);
        
        if (!conn_obj)
                return NULL;

        ram_tracer->next_connection++;
        connection_t *conn = conn_of_obj(conn_obj);
        conn->tracer_tool = ram_tracer;

        return conn;
}

static attr_value_t
get_ram(void *param, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_object(conn->ram);
}

static set_error_t
set_ram(void *arg, conf_object_t *obj, attr_value_t *val,
        attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        conn->ram = SIM_attr_object(*val);
        conn->ras_if = SIM_C_GET_INTERFACE(conn->ram, ram_access_subscribe);
        ASSERT(conn->ras_if);

        conn->filter_handle = conn->ras_if->register_access_filter_cb(
                conn->ram, obj, filter_cb, NULL);
        if (!conn->block)
                conn->after_handle = conn->ras_if->register_access_after_cb(
                        conn->ram, obj, access_cb, NULL);
        return Sim_Set_Ok;
}

static attr_value_t
get_access(void *param, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_uint64(conn->access);
}

static set_error_t
set_access(void *arg, conf_object_t *obj, attr_value_t *val,
           attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        conn->access = SIM_attr_integer(*val);
        if (conn->access > 7)
                return Sim_Set_Illegal_Value;
        return Sim_Set_Ok;
}

static attr_value_t
get_block(void *param, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_boolean(conn->block);
}

static set_error_t
set_block(void *arg, conf_object_t *obj, attr_value_t *val,
           attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        conn->block = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_inquiry(void *param, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_uint64(conn->inquiry);
}

static set_error_t
set_inquiry(void *arg, conf_object_t *obj, attr_value_t *val,
            attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        conn->inquiry = SIM_attr_boolean(*val);
        return Sim_Set_Ok;
}

/********************* Sample Ram_Tracer Object ***************************/

// ram_tracer::alloc
static conf_object_t *
it_alloc(void *arg)
{
        ram_tracer_t *ram_tracer = MM_ZALLOC(1, ram_tracer_t);
        VINIT(ram_tracer->connections);
        return &ram_tracer->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);
        ASSERT_FMT(VEMPTY(ram_tracer->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(ram_tracer);
        return 0;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *ram, attr_value_t args)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);
        connection_t *conn = new_connection(ram_tracer, ram, args);
        if (!conn)
                return NULL;
        
        VADD(ram_tracer->connections, conn);
        
        return &conn->obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);
        connection_t *conn = conn_of_obj(conn_obj);

        if (conn->filter_handle)
                conn->ras_if->remove_callback(conn->ram, conn->filter_handle);
        if (conn->after_handle)
                conn->ras_if->remove_callback(conn->ram, conn->after_handle);
        
        VREMOVE_FIRST_MATCH(ram_tracer->connections, conn_of_obj(conn_obj));
        SIM_delete_object(conn_obj);
}

static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);
        
        attr_value_t ret = SIM_alloc_attr_list(VLEN(ram_tracer->connections));
        VFORI(ram_tracer->connections, i) {
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_object(
                                &VGET(ram_tracer->connections, i)->obj));
        }
        return ret;
}

static attr_value_t
get_file(void *param, conf_object_t *obj, attr_value_t *idx)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);

        return SIM_make_attr_string(ram_tracer->filename);
}

static set_error_t
set_file(void *arg, conf_object_t *obj, attr_value_t *val,
            attr_value_t *idx)
{
        ram_tracer_t *ram_tracer = ram_tracer_of_obj(obj);

        if (ram_tracer->filename)
                MM_FREE(ram_tracer->filename);
        if (ram_tracer->file)
                fclose(ram_tracer->file);

        if (SIM_attr_is_nil(*val)) {
                ram_tracer->filename = NULL;
                ram_tracer->file = NULL;
        } else {
                ram_tracer->file = fopen(SIM_attr_string(*val), "a");
                if (!ram_tracer->file) {
                        ram_tracer->filename = NULL;
                        return Sim_Set_Illegal_Value;
                }
                ram_tracer->filename = strdup(SIM_attr_string(*val));
        }

        return Sim_Set_Ok;
}

static void
init_ram_tracer_class()
{
        const class_data_t funcs = {
                .alloc_object = it_alloc,
                .delete_instance = it_delete_connection,
                .class_desc = "tracing ram/rom accesses",
                .description = "The ram/rom tracer tool prints all monitored"
                " accesses to a ram or ram object to be printed. This blocks"
                " all caching of ram/rom pages in all processor so every access"
                " can be displayed. The new-ram-tracer command controls if"
                " read, write, or executed accesses should be printed. "
                " Inquiry accesses can also be added. The trace can be saved"
                " to a file as well.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("ram_tracer", &funcs);
        
        static const instrumentation_tool_interface_t it_iface = {
                .connect = it_connect,
                .disconnect = it_disconnect
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it_iface);        

        SIM_register_typed_attribute(
               cl, "connections",
               get_connections, NULL,
               NULL, NULL,
               Sim_Attr_Pseudo,
               "[o*]", NULL,
               "Example attribute, returns the connections");

        SIM_register_typed_attribute(
               cl, "file",
               get_file, NULL,
               set_file, NULL,
               Sim_Attr_Pseudo,
               "s|n", NULL,
               "File to write to if set.");
}

static void
init_connection_class()
{
        const class_data_t funcs = {
                .alloc_object = ic_alloc,
                .class_desc = "instrumentation connection",
                .description = "Instrumentation connection.",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };
        connection_class = SIM_register_class("ram_tracer_connection",
                                              &funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);

        SIM_register_typed_attribute(
                connection_class, "block",
                get_block, NULL,
                set_block, NULL,
                Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                "b", NULL,
                "Block caching of accesses, disable tracing");

        SIM_register_typed_attribute(
                connection_class, "access",
                get_access, NULL,
                set_access, NULL,
                Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                "i", NULL,
                "The types of accesses to trace from the ram");

        SIM_register_typed_attribute(
                connection_class, "ram",
                get_ram, NULL,
                set_ram, NULL,
                Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                "o", NULL,
                "The ram object connected to the connection");

        SIM_register_typed_attribute(
                connection_class, "inquiry",
                get_inquiry, NULL,
                set_inquiry, NULL,
                Sim_Attr_Pseudo | Sim_Attr_Read_Only,
                "b", NULL,
                "Handle inquiry accesses as well as normal ones.");
}

void
init_local()
{
        init_ram_tracer_class();
        init_connection_class();
}

