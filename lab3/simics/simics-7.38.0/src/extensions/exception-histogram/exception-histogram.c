/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.


  Simple instrumentation tool that counts how many time an exception
  occurs.
*/

#include <simics/base/conf-object.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/simulator/conf-object.h>
#include <simics/model-iface/exception.h>
#include <simics/util/hashtab.h>

conf_class_t *connection_class;
typedef struct exception_histogram exception_histogram_t;

/* A connection */
typedef struct {
        conf_object_t obj;
        conf_object_t *cpu;
        exception_histogram_t *tool;

        /* Interfaces */
        const cpu_instrumentation_subscribe_interface_t *cpu_iface;
        const cpu_exception_query_interface_t *ceq_iface;
        const exception_interface_t *exc_iface;

        ht_int_table_t exception_counters;
        uint64 exception_count;
} connection_t;

/* The Simics object */
struct exception_histogram {
        conf_object_t obj;
        int next_connection_number;        
        VECT(connection_t *) connections;
};

FORCE_INLINE exception_histogram_t *
tool_of_obj(conf_object_t *obj) { return (exception_histogram_t *)obj; }

FORCE_INLINE connection_t *
conn_of_obj(conf_object_t *obj) { return (connection_t *)obj; }

static connection_t *
new_connection(exception_histogram_t *eh, conf_object_t *cpu, attr_value_t args)
{
        strbuf_t sb = SB_INIT;
        sb_addfmt(&sb, "%s.con%d", SIM_object_name(&eh->obj),
                  eh->next_connection_number);
        conf_object_t *cobj = SIM_create_object(
                connection_class, sb_str(&sb), args);
        sb_free(&sb);
        
        if (!cobj)
                return NULL;

        eh->next_connection_number++;
        
        connection_t *conn = conn_of_obj(cobj);
                
        /* Create cached interfaces for this provider */                
        conn->cpu_iface = SIM_C_GET_INTERFACE(cpu,
                                              cpu_instrumentation_subscribe);
        conn->ceq_iface = SIM_C_GET_INTERFACE(cpu, cpu_exception_query);
        conn->exc_iface = SIM_C_GET_INTERFACE(cpu, exception);
        return conn;
}

/* For a given instruction name, pick up a counter to be incremented when this
   instruction is being executed. */
static uint64 *
get_counter_for_exception(connection_t *conn, int exception_number)
{       
        uint64 *counter = ht_lookup_int(
                &conn->exception_counters, exception_number);
        if (counter)
                return counter;

        counter = MM_ZALLOC(1, uint64);
        ht_insert_int(&conn->exception_counters, exception_number, counter);
        return counter;
}        

static void
exception_before_cb(conf_object_t *obj, conf_object_t *cpu,
                    exception_handle_t *eq_handle,
                    lang_void *unused)
{
        connection_t *conn = conn_of_obj(obj);
        int exc_num = conn->ceq_iface->exception_number(cpu, eq_handle);
        uint64 *counter = get_counter_for_exception(conn, exc_num);
        (*counter)++;
        conn->exception_count++;
}


// exception_histogram::alloc
static conf_object_t *
it_alloc(void *arg)
{
        exception_histogram_t *tool = MM_ZALLOC(1, exception_histogram_t);
        VINIT(tool->connections);
        return &tool->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        exception_histogram_t *eh = tool_of_obj(obj);
        ASSERT_FMT(VEMPTY(eh->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(eh);
        return 0;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *provider, attr_value_t args)
{
        exception_histogram_t *tool = tool_of_obj(obj);
        connection_t *conn = new_connection(tool, provider, args);
        if (!conn)
                return NULL;
        
        VADD(tool->connections, conn);
        conn->cpu_iface->register_exception_before_cb(
                provider, &conn->obj, CPU_Exception_All,
                exception_before_cb, NULL);
        return &conn->obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *con_obj)
{
        exception_histogram_t *tool = tool_of_obj(obj);
        VREMOVE_FIRST_MATCH(tool->connections, conn_of_obj(con_obj));
        SIM_delete_object(con_obj);
}

static attr_value_t
get_histogram(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);

        attr_value_t ret = SIM_alloc_attr_list(
                ht_num_entries_int(&conn->exception_counters));
        int i = 0;
        HT_FOREACH_INT(&conn->exception_counters, hit) {
                uint64 *counter = ht_iter_int_value(hit);
                uint64 exc_num = ht_iter_int_key(hit);
                const char *exc = conn->exc_iface->get_name(conn->cpu, exc_num);
                SIM_attr_list_set_item(
                        &ret, i,
                        SIM_make_attr_list(
                                2,
                                SIM_make_attr_string(exc),
                                SIM_make_attr_uint64(*counter)));
                i++;
        }
        return ret;        
}

static attr_value_t
get_cpu(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_object(conn->cpu);
}

static set_error_t
set_cpu(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = conn_of_obj(obj);
        conn->cpu = SIM_attr_object(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_tool(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_object(&conn->tool->obj);
}

static set_error_t
set_tool(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = conn_of_obj(obj);
        conn->tool = tool_of_obj(SIM_attr_object(*val));
        return Sim_Set_Ok;
}

static attr_value_t
get_exception_count(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        return SIM_make_attr_uint64(conn->exception_count);
}

static set_error_t
set_clear(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = conn_of_obj(obj);
        HT_FOREACH_INT(&conn->exception_counters, hit) {
                uint64 *counter = ht_iter_int_value(hit);
                *counter = 0;
        }
        return Sim_Set_Ok;        
}

static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *conn = MM_ZALLOC(1, connection_t);
        return &conn->obj;
}

// instrumentation_connection::pre_delete_instance
static void
ic_pre_delete_connection(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);

        if (conn->cpu)
                conn->cpu_iface->remove_connection_callbacks(conn->cpu, obj);
}

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
        conn->cpu_iface->enable_connection_callbacks(conn->cpu, obj);
}

// instrumentation_connection::disable
static void
ic_disable(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        conn->cpu_iface->disable_connection_callbacks(conn->cpu, obj);
}

static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        exception_histogram_t *eh = tool_of_obj(obj);
        attr_value_t ret = SIM_alloc_attr_list(VLEN(eh->connections));
        for (int i = 0; i < VLEN(eh->connections); i++) {
                connection_t *conn = VGET(eh->connections, i);
                conf_object_t *o = &conn->obj;
                SIM_attr_list_set_item(&ret, i, SIM_make_attr_object(o));
        }
        return ret;
}

void
init_local()
{
        /* Tool class */
        const class_data_t funcs = {
                .alloc_object = it_alloc,
                .delete_instance = it_delete_connection,
                .class_desc = "presents raised exceptions in a histogram",
                .description = "This tool subscribes to all exceptions in all"
                " connected processors and presents the information in a"
                " histogram with exception type and the number of occurrences"
                " of each exception. Use the histogram command in the tool to"
                " display the result.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("exception_histogram",
                                              &funcs);
        
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
                "List of connection sub-objects used.");

        /* The connection class */
        const class_data_t ic_funcs = {
                .alloc_object = ic_alloc,
                .description = "instrumentation connection",
                .class_desc = "instrumentation connection",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };

        connection_class = SIM_register_class("exception_histogram_connection",
                                              &ic_funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);
        
        SIM_register_attribute(
               connection_class, "histogram",
               get_histogram,
               NULL,
               Sim_Attr_Pseudo, "[[si]*]",
               "((exception_name, counter)*)*"
               " exception histogram for a connection");

        SIM_register_attribute(
                connection_class, "cpu",
                get_cpu,
                set_cpu,
                Sim_Attr_Pseudo, "o",
                "the processor being monitored");

        SIM_register_attribute(
                connection_class, "tool",
                get_tool,
                set_tool,
                Sim_Attr_Pseudo, "o",
                "the tool owning this connection");

        SIM_register_attribute(
                connection_class, "clear",
                NULL,
                set_clear,
                Sim_Attr_Pseudo, "b",
                "When this attribute is set, the current histogram statistics"
                " is removed.");

        SIM_register_attribute(
                connection_class, "exception_count",
                get_exception_count,
                NULL,
                Sim_Attr_Pseudo, "i",
                "Total number of exceptions seen");
}
