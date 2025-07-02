/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.


  Simple instrumentation tool that just counts how many instructions we execute
  in different x86 execution modes.
*/

#include <simics/simulator/conf-object.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/arch/x86-instrumentation.h>
#include <simics/model-iface/int-register.h>
#include <simics/base/event.h>

conf_class_t *connection_class;

typedef struct state {
        x86_detailed_exec_mode_t exe_mode;
        vmx_mode_t vmx_mode;
        bool smm_mode;
} state_t;

typedef struct {
        state_t state;
        pc_step_t steps;
} histogram_element_t;

/* A connection */
typedef struct {
        conf_object_t obj;
        conf_object_t *cpu;
        conf_object_t *tool;
        
        /* Cached Interfaces */
        const x86_instrumentation_subscribe_v2_interface_t *x86_iface;
        const vmx_instrumentation_subscribe_interface_t *vmx_iface;
        const smm_instrumentation_subscribe_interface_t *smm_iface;
        const cpu_instrumentation_subscribe_interface_t *cpu_iface;

        pc_step_t mode_change_step;
        state_t state;
        VECT(histogram_element_t) histogram;
} connection_t;

/* The Simics object */
typedef struct {
        conf_object_t obj;
        int next_connection_number;
        VECT(connection_t *) connections;        
} x86_mode_histogram_t;

FORCE_INLINE x86_mode_histogram_t *
tool_of_obj(conf_object_t *obj) { return (x86_mode_histogram_t *)obj; }

FORCE_INLINE connection_t *
conn_of_obj(conf_object_t *obj) { return (connection_t *)obj; }

static x86_detailed_exec_mode_t
get_detailed_mode(conf_object_t *cpu)
{
        const int_register_interface_t *iface =
                SIM_C_GET_INTERFACE(cpu, int_register);

        int num = iface->get_number(cpu, "cpu_mode");
        return iface->read(cpu, num);
}

static vmx_mode_t
get_vmx_mode(conf_object_t *cpu)
{
        attr_value_t am = SIM_get_attribute(cpu, "vmx_mode");
        return SIM_attr_integer(am);
}

static bool
get_smm_mode(conf_object_t *cpu)
{
        attr_value_t am = SIM_get_attribute(cpu, "in_smm");
        return SIM_attr_integer(am);
}

static bool
mode_match(state_t *a, state_t *b)
{
        return (a->exe_mode == b->exe_mode
                && a->vmx_mode == b->vmx_mode
                && a->smm_mode == b->smm_mode);
}

static void
update_state(connection_t *conn)
{
        pc_step_t now_steps = SIM_step_count(conn->cpu);
        pc_step_t delta_steps = now_steps - conn->mode_change_step;
        conn->mode_change_step = now_steps;

        VFOREACH_T(conn->histogram, histogram_element_t, he) {
                if (mode_match(&he->state, &conn->state)) {
                        he->steps += delta_steps;
                        return;
                }
        }
        histogram_element_t he = {
                .state = conn->state,
                .steps = delta_steps
        };
        VADD(conn->histogram, he);
}

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
        connection_t *conn = conn_of_obj(obj);
        conn->cpu_iface->remove_connection_callbacks(conn->cpu, obj);
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
        conn->cpu_iface->enable_connection_callbacks(conn->cpu, obj);
}

// instrumentation_connection::disable
static void
ic_disable(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        conn->cpu_iface->disable_connection_callbacks(conn->cpu, obj);
}

static void
enter_exe_cb(conf_object_t *obj, conf_object_t *cpu,
             x86_detailed_exec_mode_t mode,
             lang_void *user_data)
{
        connection_t *conn = conn_of_obj(obj);
        update_state(conn);
        conn->state.exe_mode = mode;
}

static void
enter_vmx_cb(conf_object_t *obj, conf_object_t *cpu, vmx_mode_t mode,
             physical_address_t vmcs, lang_void *data)
{
        connection_t *conn = conn_of_obj(obj);
        update_state(conn);
        conn->state.vmx_mode = mode;
}

static void
enter_smm_cb(conf_object_t *obj, conf_object_t *cpu, lang_void *data)
{
        connection_t *conn = conn_of_obj(obj);
        update_state(conn);
        conn->state.smm_mode = true;
}

static void
leave_smm_cb(conf_object_t *obj, conf_object_t *cpu, lang_void *data)
{
        connection_t *conn = conn_of_obj(obj);
        update_state(conn);
        conn->state.smm_mode = false;
}

/* Help function which creates a new connection and returns it. */
static connection_t *
new_connection(x86_mode_histogram_t *tool, conf_object_t *cpu, attr_value_t args)
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
        conn->cpu = cpu;
        
        conn->x86_iface = SIM_C_GET_INTERFACE(
                cpu, x86_instrumentation_subscribe_v2);
        conn->vmx_iface = SIM_C_GET_INTERFACE(
                cpu, vmx_instrumentation_subscribe);
        conn->smm_iface = SIM_C_GET_INTERFACE(
                cpu, smm_instrumentation_subscribe);
        conn->cpu_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_instrumentation_subscribe);

        conn->x86_iface->register_mode_enter_cb(cpu, conn_obj, enter_exe_cb, NULL);
        conn->vmx_iface->register_vmx_mode_enter_cb(cpu, conn_obj, enter_vmx_cb, NULL);
        conn->smm_iface->register_smm_enter_after_cb(cpu, conn_obj, enter_smm_cb, NULL);
        conn->smm_iface->register_smm_leave_after_cb(cpu, conn_obj, leave_smm_cb, NULL);

        conn->state.exe_mode = get_detailed_mode(cpu);
        conn->state.vmx_mode = get_vmx_mode(cpu);
        conn->state.smm_mode = get_smm_mode(cpu);

        conn->mode_change_step = SIM_step_count(cpu);
        return conn;
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
        return SIM_make_attr_object(conn->tool);
}

static set_error_t
set_tool(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = conn_of_obj(obj);
        conn->tool = SIM_attr_object(*val);
        return Sim_Set_Ok;
}

static attr_value_t
state_as_attr(state_t *s)
{
        return SIM_make_attr_list(
                3,
                SIM_make_attr_boolean(s->smm_mode),
                SIM_make_attr_int64(s->vmx_mode),
                SIM_make_attr_int64(s->exe_mode));
}

static attr_value_t
element_as_attr(histogram_element_t *he)
{
        return SIM_make_attr_list(
                2,
                state_as_attr(&he->state),
                SIM_make_attr_int64(he->steps));
}

static attr_value_t
get_histogram(conf_object_t *obj)
{
        connection_t *conn = conn_of_obj(obj);
        update_state(conn); // sync any additional steps from mode change
        attr_value_t ret = SIM_alloc_attr_list(VLEN(conn->histogram));
        VFORI(conn->histogram, i) {
                SIM_attr_list_set_item(
                        &ret, i, element_as_attr(&VGET(conn->histogram, i)));
        }
        return ret;
}

static set_error_t
set_histogram(conf_object_t *obj, attr_value_t *ignore)
{
        connection_t *conn = conn_of_obj(obj);
        VCLEAR(conn->histogram);
        return Sim_Set_Ok;
}


/************************** Tool Object ********************************/

// x86_mode_histogram::alloc
static conf_object_t *
it_alloc(void *arg)
{
        x86_mode_histogram_t *tool = MM_ZALLOC(1, x86_mode_histogram_t);
        VINIT(tool->connections);
        return &tool->obj;
}

static int
it_delete_connection(conf_object_t *obj)
{
        x86_mode_histogram_t *tool = tool_of_obj(obj);
        ASSERT_FMT(VEMPTY(tool->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(tool);
        return 0;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *cpu, attr_value_t args)
{
        x86_mode_histogram_t *tool = tool_of_obj(obj);
        connection_t *conn = new_connection(tool, cpu, args);
        if (!conn)
                return NULL;
        
        VADD(tool->connections, conn);
        
        return &conn->obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        x86_mode_histogram_t *tool = tool_of_obj(obj);
        VREMOVE_FIRST_MATCH(tool->connections, conn_of_obj(conn_obj));
        SIM_delete_object(conn_obj);
}

static attr_value_t
get_connections(void *param, conf_object_t *obj, attr_value_t *idx)
{
        x86_mode_histogram_t *tool = tool_of_obj(obj);
        
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
                .class_desc = "x86 mode histogram tool",
                .description =
                "This tool collects different execution modes in the IA"
                " processors connected to the tool and displays a histogram"
                " of how many instructions was executed in each mode.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("x86_mode_histogram", &funcs);
        
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
        connection_class = SIM_register_class("x86_mode_histogram_connection",
                                              &funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);

        SIM_register_attribute(
                connection_class, "cpu",
                get_cpu,
                set_cpu,
                Sim_Attr_Pseudo,
                "o",
               "The connected cpu");

        SIM_register_attribute(
                connection_class, "tool",
                get_tool,
                set_tool,
                Sim_Attr_Pseudo, "o",
                "The tool owning this connection");

        SIM_register_attribute(
                connection_class, "histogram",
                get_histogram,
                set_histogram,
                Sim_Attr_Pseudo,
                "[[[bii]i]*]",
                "The execution mode histogram. Set to any value will clear the"
                " data.");
        
}

void
init_local()
{
        init_tool_class();
        init_connection_class();
}

