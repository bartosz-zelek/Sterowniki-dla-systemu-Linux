/*
  © 2017 Intel Corporation
*/

#include <simics/simulator/conf-object.h>
#include <simics/simulator-iface/instrumentation-tool.h>

conf_class_t *connection_class;

/* A connection */
typedef struct {
        conf_object_t obj;
        conf_object_t *provider;
        /* USER-TODO: add data-members that is stored in the connection */
        
        /* Cached Interfaces */
        /* USER-TODO: cache all interface pointers used */
} connection_t;

/* The Simics object */
typedef struct {
        conf_object_t obj;
        int next_connection_number;
        VECT(connection_t *) connections;        
        /* USER-TODO: add data-mebers global for the tool */
} empty_tool_t;

FORCE_INLINE empty_tool_t *
tool_of_obj(conf_object_t *obj) { return (empty_tool_t *)obj; }

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
        /* USER-TODO: Possibly unregister with the provider, if still
           connected. */
}

// class_data_t(connection_t)::delete_instance
static int
ic_delete_connection(conf_object_t *obj)
{
        /* USER-TODO: Free any other memory associated with the connection */
        MM_FREE(obj);
        return 0;
}

// instrumentation_connection::enable
static void
ic_enable(conf_object_t *obj)
{
        /* USER-TODO: enable the connection in the provider */
}

// instrumentation_connection::disable
static void
ic_disable(conf_object_t *obj)
{
        /* USER-TODO: disable the connection in the provider */
}

/* Help function which creates a new connection and returns it. */
static connection_t *
new_connection(empty_tool_t *tool, conf_object_t *provider, attr_value_t args)
{
        strbuf_t sb = SB_INIT;
        
        sb_addfmt(&sb, "%s.con%d",
                  SIM_object_name(&tool->obj),
                  tool->next_connection_number);
        conf_object_t *conn_obj = SIM_create_object(
                connection_class, sb_str(&sb), args);
        sb_free(&sb);
        
        if (!conn_obj)
                return NULL;

        tool->next_connection_number++;
        connection_t *conn = conn_of_obj(conn_obj);
        conn->provider = provider;
        /* USER-TODO: Initialize connection data */
        
        /* USER-TODO: Create cached interfaces */
        // conn->empty_iface = SIM_C_GET_INTERFACE(provider, empty)

        return conn;
}

/* USER-TODO: This is just a sample attribute (on the connection),
   replace with anything */
static attr_value_t
get_provider(void *param, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_object(conn->provider);
}

/************************** Sample Tool Object ********************************/

// empty_tool::alloc
static conf_object_t *
it_alloc(void *arg)
{
        empty_tool_t *tool = MM_ZALLOC(1, empty_tool_t);
        VINIT(tool->connections);
        return &tool->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        empty_tool_t *tool = tool_of_obj(obj);
        ASSERT_FMT(VEMPTY(tool->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(tool);
        return 0;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *provider, attr_value_t args)
{
        empty_tool_t *tool = tool_of_obj(obj);
        connection_t *conn = new_connection(tool, provider, args);
        if (!conn)
                return NULL;
        
        VADD(tool->connections, conn);
        
        return &conn->obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        empty_tool_t *tool = tool_of_obj(obj);
        VREMOVE_FIRST_MATCH(tool->connections, conn_of_obj(conn_obj));
        SIM_delete_object(conn_obj);
}

/* USER-TODO: This is just a sample attribute (on the tool),
   replace with anything. */
static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        empty_tool_t *tool = tool_of_obj(obj);
        
        attr_value_t ret = SIM_alloc_attr_list(VLEN(tool->connections));
        VFORI(tool->connections, i) {
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_object(&VGET(tool->connections, i)->obj));
        }
        return ret;
}

static void
init_tool_class()
{
        const class_data_t funcs = {
                .alloc_object = it_alloc,
                .delete_instance = it_delete_connection,
                .class_desc = "empty instrumentation tool",
                .description = "Sample tool - long description.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("empty_tool", &funcs);
        
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
}

static void
init_connection_class()
{
        const class_data_t funcs = {
                .alloc_object = ic_alloc,
                .description = "Instrumentation connection",
                .class_desc = "instrumentation connection",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };
        connection_class = SIM_register_class("empty_tool_connection",
                                              &funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);

        /* USER-TODO: Register connection specific attributes */
        SIM_register_typed_attribute(
                connection_class, "provider",
                get_provider, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "o", NULL,
               "Example attribute, returns the provider");
        
}

void
init_local()
{
        init_tool_class();
        init_connection_class();
}

