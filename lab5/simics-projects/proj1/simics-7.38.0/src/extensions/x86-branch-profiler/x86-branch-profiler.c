/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/simulator/conf-object.h>
#include <simics/model-iface/cpu-instrumentation.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/model-iface/processor-info.h>
#include <simics/device-api.h>

typedef struct taken_non_taken {
        uint64 taken;
        uint64 non_taken;
} taken_non_taken_t;

static void call_cb(conf_object_t *obj,  conf_object_t *cpu,
                    instruction_handle_t *handle,
                    void *user_data);

static void ret_cb(conf_object_t *obj,  conf_object_t *cpu,
                   instruction_handle_t *handle,
                   void *user_data);

static void jmp_cb(conf_object_t *obj,  conf_object_t *cpu,
                   instruction_handle_t *handle,
                   void *user_data);

static void jcc_cb(conf_object_t *obj,  conf_object_t *cpu,
                   instruction_handle_t *handle,
                   void *user_data);

typedef struct branch_info {
        const char *type;
        cpu_instruction_cb_t cb;
} branch_info_t;

branch_info_t branch_infos[] =
{
        { "call", call_cb },
        { "ret", ret_cb },
        { "jmp", jmp_cb },
        { "ja", jcc_cb },
        { "jae", jcc_cb },
        { "jb", jcc_cb },
        { "jbe", jcc_cb },
//      { "jc", jcc_cb }, == jb
        { "jcxz", jcc_cb },
        { "jecxz", jcc_cb },
        { "jrcxz", jcc_cb },
        { "je", jcc_cb },
        { "jg", jcc_cb },
        { "jge", jcc_cb },
        { "jl", jcc_cb },
        { "jle", jcc_cb },
//      { "jna", jcc_cb }, == jbe
//      { "jnae", jcc_cb }, == jb
//      { "jnb", jcc_cb }, == jae
//      { "jnbe", jcc_cb }, == ja
//      { "jnc", jcc_cb }, == jae
        { "jne", jcc_cb },
//      { "jng", jcc_cb }, == jle
//      { "jnge", jcc_cb }, == jl
//      { "jnl", jcc_cb }, == jge
//      { "jnle", jcc_cb }, == jg
        { "jno", jcc_cb },
//      { "jnp", jcc_cb }, == jpo
        { "jns", jcc_cb },
//      { "jnz", jcc_cb }, == jne
        { "jo", jcc_cb },
        { "jp", jcc_cb },
//      { "jpe", jcc_cb }, == jp
        { "jpo", jcc_cb },
        { "js", jcc_cb },
//      { "jz", jcc_cb }, == je
};

/* Cached state specific to a certain connection. */
typedef struct {
        conf_object_t obj;
        conf_object_t *cpu; /* connected cpu */
        
        /* Interfaces */
        const cpu_instrumentation_subscribe_interface_t *cpu_iface;
        const cpu_instruction_query_interface_t *iq_iface;
        const cpu_cached_instruction_interface_t *ci_iface;
        const processor_info_v2_interface_t *pi_iface;

        taken_non_taken_t taken_non_taken[ALEN(branch_infos)];
} connection_t;

FORCE_INLINE connection_t *
conn_of_obj(conf_object_t *obj) { return (connection_t *)obj; }

static void
call_cb(conf_object_t *obj,  conf_object_t *cpu,
        instruction_handle_t *handle,
        void *user_data)
{
        taken_non_taken_t *tnt = user_data;
        tnt->taken++;
}

static void
ret_cb(conf_object_t *obj,  conf_object_t *cpu,
       instruction_handle_t *handle,
       void *user_data)
{
        taken_non_taken_t *tnt = user_data;
        tnt->taken++;
}

static void
jmp_cb(conf_object_t *obj,  conf_object_t *cpu,
       instruction_handle_t *handle,
       void *user_data)
{
        taken_non_taken_t *tnt= user_data;
        tnt->taken++;
}


static void
jcc_cb(conf_object_t *obj,  conf_object_t *cpu,
       instruction_handle_t *handle,
       void *user_data)
{
        connection_t *conn = conn_of_obj(obj);
        logical_address_t pc = conn->pi_iface->get_program_counter(cpu);
        logical_address_t la = conn->iq_iface->logical_address(cpu, handle);
        cpu_bytes_t bytes = conn->iq_iface->get_instruction_bytes(cpu, handle);
        taken_non_taken_t *tnt = user_data;

        /* Check PC and the length of the instruction to check if
           we branched or falled-through */
        if (pc == la + bytes.size) 
                tnt->non_taken++;
        else
                tnt->taken++;
}

static void
cached_instruction_cb(
        conf_object_t *obj, conf_object_t *cpu,
        cached_instruction_handle_t *ci_handle,
        instruction_handle_t *iq_handle,
        void *user_data)
{
        connection_t *conn = user_data;
        cpu_bytes_t b = conn->iq_iface->get_instruction_bytes(cpu, iq_handle);
        attr_value_t data = SIM_make_attr_data(b.size, b.data);
        tuple_int_string_t da = conn->pi_iface->disassemble(
                cpu, 0, data, 0);
        SIM_attr_free(&data);

        if (da.integer <= 0)
                return;                      /* Illegal instruction */

        for (int i = 0; i < ALEN(branch_infos); i++) {
                strbuf_t sb = SB_INIT;
                /* compare with space in the end to distinguish between, e.g.,
                   "jg" and "jge". Do not do that for "ret" which lacks space */
                sb_fmt(&sb, "%s%s",
                       branch_infos[i].type,
                       strcmp(branch_infos[i].type, "ret") == 0 ? "" : " ");
                if (strncmp(da.string, sb_str(&sb), sb_len(&sb)) == 0) {
                        conn->ci_iface->register_instruction_after_cb(
                                cpu, ci_handle, branch_infos[i].cb,
                                &conn->taken_non_taken[i], NULL);
                }
                sb_free(&sb);
        }
        MM_FREE(da.string);
}

/*************************** Connection Object ********************************/

conf_class_t *connection_class;

// instrumentation_connection::alloc_object
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
        /* clean up connections to the provider */
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

/********************************** branch_prof_t *****************************/

/* The Simics object */
typedef struct {
        conf_object_t obj;
        int next_connection_number;
        VECT(connection_t *) connections;
} branch_prof_t;

FORCE_INLINE branch_prof_t *
branch_prof_of_obj(conf_object_t *obj) { return (branch_prof_t *)obj; }

static connection_t *
new_connection(branch_prof_t *bp, conf_object_t *cpu, attr_value_t args)
{
        strbuf_t sb = SB_INIT;
        
        sb_addfmt(&sb, "%s.con%d",
                  SIM_object_name(&bp->obj),
                  bp->next_connection_number);
        conf_object_t *conn_obj = SIM_create_object(
                connection_class, sb_str(&sb), args);
        sb_free(&sb);

        if (!conn_obj)
                return NULL;
        
        bp->next_connection_number++;                
        connection_t *conn = conn_of_obj(conn_obj);
        conn->cpu = cpu;
        
        conn->cpu_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_instrumentation_subscribe);
        
        conn->iq_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_instruction_query);

        conn->ci_iface = SIM_C_GET_INTERFACE(
                cpu, cpu_cached_instruction);

        conn->pi_iface = SIM_C_GET_INTERFACE(
                cpu, processor_info_v2);
        return conn;
}

static conf_object_t *
alloc_object(void *arg)
{
        branch_prof_t *pb = MM_ZALLOC(1, branch_prof_t);
        return &pb->obj;
}

static int
it_delete_object(conf_object_t *obj)
{
        branch_prof_t *pb = branch_prof_of_obj(obj);
        ASSERT_FMT(VEMPTY(pb->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(pb);
        return 0;
}

// instrumentation_tool::connect()
static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *cpu, attr_value_t args)
{
        branch_prof_t *bp = branch_prof_of_obj(obj);
        
        /* Create the new connection */
        connection_t *conn = new_connection(bp, cpu, args);
        if (!conn)
                return NULL;

        VADD(bp->connections, conn);
        
        /* Register a callback that will be called for each instruction */
        conn->cpu_iface->register_cached_instruction_cb(
                cpu, &conn->obj, cached_instruction_cb, conn);
        
        return &conn->obj;
}

// instrumentation_tool::disconnect()
static void
it_disconnect(conf_object_t *obj, conf_object_t *conn_obj)
{
        branch_prof_t *bp = branch_prof_of_obj(obj);
        connection_t *conn = conn_of_obj(conn_obj);
        conn->cpu_iface->remove_connection_callbacks(conn->cpu, conn_obj);
        VREMOVE_FIRST_MATCH(bp->connections, conn_of_obj(conn_obj));
        SIM_delete_object(conn_obj);
}

static attr_value_t
get_stats(void *arg, conf_object_t *obj, attr_value_t *idx)
{
        branch_prof_t *bp = branch_prof_of_obj(obj);
        attr_value_t ret = SIM_alloc_attr_list(ALEN(branch_infos));
        
        for (int i = 0; i < ALEN(branch_infos); i++) {
                taken_non_taken_t tnt = { 0, 0 };
                VFORI(bp->connections, c) {
                        connection_t *conn = VGET(bp->connections, c);
                        tnt.taken += conn->taken_non_taken[i].taken;
                        tnt.non_taken += conn->taken_non_taken[i].non_taken;
                }
                
                attr_value_t item = SIM_make_attr_list(
                        3,
                        SIM_make_attr_string(branch_infos[i].type),
                        SIM_make_attr_uint64(tnt.taken),
                        SIM_make_attr_uint64(tnt.non_taken));
                SIM_attr_list_set_item(&ret, i, item);
        }
        return ret;
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
        connection_class = SIM_register_class("x86_branch_prof_connection",
                                              &funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);
}

static void
init_branch_prof_class()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = it_delete_object,
                .class_desc = "x86 branch statistics collector",
                .description = "The x86 branch profiler collects branch"
                " statistics for every branch executed on the processor."
                " For every conditional branch all taken and non-taken counts"
                " are displayed. For non-conditional branches and calls only"
                " the counts are displayed. Control flow through exceptions"
                " and interrupts are not collected.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("x86_branch_profiler",
                                              &funcs);

       static const instrumentation_tool_interface_t it_iface = {
                .connect = it_connect,
                .disconnect = it_disconnect,
       };
       SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it_iface);

       SIM_register_typed_attribute(
               cl, "stats",
               get_stats, NULL,
               NULL, NULL,
               Sim_Attr_Pseudo | Sim_Attr_Read_Only,
               "[[sii]*]", NULL,
                "Taken/non taken statistics");       
}

void
init_local()
{
        init_branch_prof_class();
        init_connection_class();
}
