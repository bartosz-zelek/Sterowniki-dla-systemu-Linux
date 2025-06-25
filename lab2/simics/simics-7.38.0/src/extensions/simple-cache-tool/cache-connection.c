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

/* This file redirects accesses from the instrumentation API to the cache
   simulation. Some statistics are collected here as well. This file is
   specialized for x86 processor with regards to flushes and prefetching.
   With small modifications it can be adapted to other architectures.
*/

#include "cache-connection.h"
#include "cache-tool.h"

#include <simics/simulator-api.h>
#include <simics/model-iface/timing-model.h>

#include <simics/arch/x86.h>
#include <stdio.h>

conf_class_t *conn_class;

FORCE_INLINE bool
cachable(x86_memory_type_t mt)
{
        // X86_Strong_Uncacheable || mt == X86_Uncacheable
        if (mt > X86_Uncacheable)
                return true;
        else
                return false;
}

static cycles_t
handle_non_vanilla(connection_t *conn, conf_object_t *cpu, x86_access_type_t at,
                   physical_address_t pa, unsigned size)
{
        cycles_t penalty = 0;
        switch(at) {
        case X86_Clflush :
                if (conn->dl1)
                        penalty += conn->dl1_iface->invalidate(
                                conn->dl1, pa, Cache_Propagate_Next);
                if (conn->il1)
                        penalty += conn->il1_iface->invalidate(
                                conn->il1, pa, Cache_Propagate_None);
                conn->cache_line_flush_instructions++;
                break;
        case X86_Prefetch_3DNow :
        case X86_Prefetchw_3DNow :
        case X86_Prefetch_T0 :
        case X86_Prefetch_T1 :
        case X86_Prefetch_T2 :
        case X86_Prefetch_NTA :
                penalty += conn->dl1_iface->read(
                        conn->dl1, cpu, Sim_Initiator_CPU,
                        pa, size, Access_Type_Prefetch);
                conn->prefetch_instructions++;
                break;
        default:
                penalty += conn->dl1_iface->read(
                        conn->dl1, cpu, Sim_Initiator_CPU,
                        pa, size, Access_Type_Read);
        }
        return penalty;
}

static cycles_t
read_forall(conf_object_t *obj, conf_object_t *cpu,
            memory_handle_t *mem, void *connection_user_data)
{
        connection_t *conn = obj_to_conn(obj);
        const cpu_memory_query_interface_t *mq = conn->mq_iface;
        const x86_memory_query_interface_t *xq = conn->xq_iface;
        x86_memory_type_t mt = xq->memory_type(cpu, mem);
        x86_access_type_t at = xq->access_type(cpu, mem);

        if (!conn->dl1_iface) {
                SIM_LOG_ERROR(obj, 0, "Connection not configures with dcache");
                return 0;
        }
        
        if (cachable(mt)) {
                physical_address_t pa = mq->physical_address(cpu, mem);
                unsigned size = mq->get_bytes(cpu, mem).size;
                if (at == X86_Vanilla) {
                        return conn->dl1_iface->read(
                                conn->dl1, cpu, Sim_Initiator_CPU,
                                pa, size, Access_Type_Read);
                } else {
                        return handle_non_vanilla(conn, cpu, at, pa, size);
                }
        } else {
                conn->uncachable_reads++;
                return /* device penalty */ 0;
        }
}

static void
read_forall_cb(conf_object_t *obj, conf_object_t *cpu,
               memory_handle_t *mem, void *connection_user_data)
{
        read_forall(obj, cpu, mem, connection_user_data);
}


static void
read_forall_penalty_cb(conf_object_t *obj, conf_object_t *cpu,
                       memory_handle_t *mem, void *connection_user_data)
{
        connection_t *conn = obj_to_conn(obj);
        cycles_t penalty = read_forall(obj, cpu, mem, connection_user_data);
        conn->tool->cycle_staller.iface->add(
                conn->tool->cycle_staller.obj, cpu, penalty);
}

static cycles_t
write_forall(conf_object_t *obj, conf_object_t *cpu,
             memory_handle_t *mem, void *connection_user_data)
{
        connection_t *conn = obj_to_conn(obj);
        const cpu_memory_query_interface_t *mq = conn->mq_iface;
        const x86_memory_query_interface_t *xq = conn->xq_iface;
        x86_memory_type_t mt = xq->memory_type(cpu, mem);

        if (!conn->dl1_iface) {
                SIM_LOG_ERROR(obj, 0, "Connection not configures with dcache");
                return 0;
        }

        if (cachable(mt)) {
                physical_address_t pa = mq->physical_address(cpu, mem);
                unsigned size = mq->get_bytes(cpu, mem).size;
                return conn->dl1_iface->write(conn->dl1, cpu, Sim_Initiator_CPU,
                                              pa, size, Access_Type_Write);
        } else {
                conn->uncachable_writes++;
                return /* device penalty */ 0;
        }
}

static void
write_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                memory_handle_t *mem, void *connection_user_data)
{
        write_forall(obj, cpu, mem, connection_user_data);
}

static void
write_forall_penalty_cb(conf_object_t *obj, conf_object_t *cpu,
                        memory_handle_t *mem, void *connection_user_data)
{
        connection_t *conn = obj_to_conn(obj);
        cycles_t penalty = write_forall(obj, cpu, mem, connection_user_data);
        conn->tool->cycle_staller.iface->add(
                conn->tool->cycle_staller.obj, cpu, penalty);
}

static cycles_t
instruction_forall(conf_object_t *obj, conf_object_t *cpu,
                   instruction_handle_t *instr,
                   void *connection)
{
        connection_t *conn = obj_to_conn(obj);
        const cpu_instruction_query_interface_t *iq = conn->iq_iface;

        if (!conn->il1_iface) {
                SIM_LOG_ERROR(obj, 0, "Connection not configures with icache");
                return 0;
        }

        uint64 pa = iq->physical_address(cpu, instr);
        pa = pa & ~(conn->instruction_block_size - 1);

        /* only call instruction cache if we move to another block */
        if (pa != conn->last_instruction_block) {
                conn->last_instruction_block = pa;
                return conn->il1_iface->read(conn->il1, cpu, Sim_Initiator_CPU,
                                             pa, 64, Access_Type_Execute);
        }
        return 0;
}

static void
instruction_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                      instruction_handle_t *instr,
                      void *connection)
{
        instruction_forall(obj, cpu, instr, connection);
}

static void
instruction_forall_penalty_cb(conf_object_t *obj, conf_object_t *cpu,
                      instruction_handle_t *instr,
                      void *connection)
{
        connection_t *conn = obj_to_conn(obj);
        cycles_t penalty = instruction_forall(obj, cpu, instr, connection);
        conn->tool->cycle_staller.iface->add(
                conn->tool->cycle_staller.obj, cpu, penalty);
}

/* Both for invd and wbinvd. We cannot handle wbinvd correctly since typically
   Simics can not have inconsistent memory. */
static void
inv_cb(conf_object_t *obj, conf_object_t *cpu, instruction_handle_t *handle,
       lang_void *user_data)
{
        connection_t *conn = obj_to_conn(obj);

        if (conn->dl1)
                conn->dl1_iface->invalidate_all(
                        conn->dl1, Cache_Propagate_Next);
        if (conn->il1)
                conn->il1_iface->invalidate_all(
                        conn->il1, Cache_Propagate_None);
        conn->cache_all_flush_instructions++;
}

static void
cached_instruction_cb(
        conf_object_t *obj, conf_object_t *cpu,
        cached_instruction_handle_t *handle,
        instruction_handle_t *iq_handle,
        void *connection_user_data)
{
        connection_t *conn = obj_to_conn(obj);
        cpu_bytes_t oc = conn->iq_iface->get_instruction_bytes(
                cpu, iq_handle);

        if (oc.data[0] == 0x0F) {
                if (oc.data[1] == 0x08) // invd
                        conn->ci_iface->register_instruction_after_cb(
                                cpu, handle, inv_cb, NULL, NULL);
                else if (oc.data[1] == 0x09) // wbinvd
                        conn->ci_iface->register_instruction_after_cb(
                                cpu, handle, inv_cb, NULL, NULL);
        }
}

void
configure_conn(connection_t *conn)
{
	/* Callback for memory read and write operations */
        cpu_memory_cb_t read;
        cpu_memory_cb_t write;

        if (conn->tool->cycle_staller.obj) {
                read = read_forall_penalty_cb;
                write = write_forall_penalty_cb;
        } else {
                read = read_forall_cb;
                write = write_forall_cb;
        }
                
        conn->cis_iface->register_read_before_cb(
                conn->provider, &conn->obj, CPU_Access_Scope_Explicit,
                read, NULL);

        conn->cis_iface->register_write_before_cb(
                conn->provider, &conn->obj, CPU_Access_Scope_Explicit,
                write, NULL);

        conn->cis_iface->register_read_before_cb(
                conn->provider, &conn->obj, CPU_Access_Scope_Implicit,
                read, NULL);

        conn->cis_iface->register_write_before_cb(
                conn->provider, &conn->obj, CPU_Access_Scope_Implicit,
                write, NULL);

        conn->cis_iface->register_cached_instruction_cb(
                conn->provider, &conn->obj, cached_instruction_cb,
                NULL);
}

/* Allocate the object, return a pointer to the object */
static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *conn = MM_ZALLOC(1, connection_t);

        conn->dl1 = NULL;
        conn->il1 = NULL;
        return &conn->obj;
}

/* pre_delete_instance - less expensive than delete */
static void
ic_pre_delete_conn(conf_object_t *obj)
{
	connection_t *conn = obj_to_conn(obj);
	conn->cis_iface->remove_connection_callbacks(conn->provider,
                                                     &conn->obj);
        if (conn->dl1 && object_exists(conn->dl1_name))
                SIM_delete_object(conn->dl1);
        if (conn->il1 && object_exists(conn->il1_name))
                SIM_delete_object(conn->il1);
}

/* Delete connection-cache */
static int
ic_delete_conn(conf_object_t *obj)
{
	MM_FREE(obj);
	return 0;
}

/* Enable/disable cache-connection */
static void
ic_enable(conf_object_t *obj)
{
	connection_t *c = obj_to_conn(obj);
	c->cis_iface->enable_connection_callbacks(c->provider, obj);
}

static void
ic_disable(conf_object_t *obj)
{
	connection_t *c = obj_to_conn(obj);
	c->cis_iface->disable_connection_callbacks(c->provider, obj);
}

typedef enum {
        Data_Cache,
        Instr_Cache,
} di_t;


static attr_value_t
get_next(void *data, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = obj_to_conn(obj);
        di_t kind = (di_t)data;
        attr_value_t ret;

        switch (kind) {

        case Data_Cache:
                ret = SIM_make_attr_object(conn->dl1);
                break;
        case Instr_Cache:
                ret = SIM_make_attr_object(conn->il1);
                break;
        }
        return ret;
}

static set_error_t
set_next(void *data, conf_object_t *obj,
         attr_value_t *val, attr_value_t *idx)
{
        connection_t *conn = obj_to_conn(obj);
        conf_object_t *next_level = SIM_attr_object(*val);
        di_t kind = (di_t)data;

        switch (kind) {

        case Data_Cache:
                conn->dl1 = next_level;
                conn->dl1_iface = SIM_get_interface(conn->dl1, "simple_cache");
                if (conn->dl1_name)
                        MM_FREE(conn->dl1_name);
                conn->dl1_name = MM_STRDUP(SIM_object_name(conn->dl1));
                if (!conn->dl1_iface)
                        return Sim_Set_Interface_Not_Found;
                break;
        case Instr_Cache:
                conn->il1 = next_level;
                conn->il1_iface = SIM_get_interface(conn->il1, "simple_cache");
                if (conn->il1_name)
                        MM_FREE(conn->il1_name);
                conn->il1_name = MM_STRDUP(SIM_object_name(conn->il1));
                if (!conn->il1_iface)
                        return Sim_Set_Interface_Not_Found;
                attr_value_t sa = SIM_get_attribute(
                        conn->il1, "cache_block_size");
                conn->instruction_block_size = SIM_attr_integer(sa);
                break;
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_cpu(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_object(conn->provider);
}

static attr_value_t
get_tool(void *_id, conf_object_t *obj, attr_value_t *idx)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_object(&conn->tool->obj);
}

static attr_value_t
get_issue_instructions(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_boolean(conn->issue_instructions);
}

static set_error_t
set_issue_instructions(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->issue_instructions = SIM_attr_boolean(*val);

        if (conn->instruction_handle) {
                conn->cis_iface->remove_callback(
                        conn->provider, conn->instruction_handle);
                conn->instruction_handle = NULL;
        }

        /* Register callback for instructions if requested */
        if (conn->issue_instructions) {
                cpu_instruction_cb_t instruction;
                if (conn->tool->cycle_staller.obj)
                        instruction = instruction_forall_penalty_cb;
                else
                        instruction = instruction_forall_cb;
                conn->cis_iface->register_instruction_before_cb(
                        conn->provider, &conn->obj, instruction, NULL);
        }
        return Sim_Set_Ok;
}

static attr_value_t
get_uncachable_reads(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_uint64(conn->uncachable_reads);
}

static set_error_t
set_uncachable_reads(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->uncachable_reads = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_uncachable_writes(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_uint64(conn->uncachable_writes);
}

static set_error_t
set_uncachable_writes(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->uncachable_writes = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_prefetch_instructions(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_uint64(conn->prefetch_instructions);
}

static set_error_t
set_prefetch_instructions(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->prefetch_instructions = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_cache_all_flush_instructions(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_uint64(conn->cache_all_flush_instructions);
}

static set_error_t
set_cache_all_flush_instructions(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->cache_all_flush_instructions = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

static attr_value_t
get_cache_line_flush_instructions(conf_object_t *obj)
{
        connection_t *conn = obj_to_conn(obj);
        return SIM_make_attr_uint64(conn->cache_line_flush_instructions);
}

static set_error_t
set_cache_line_flush_instructions(conf_object_t *obj, attr_value_t *val)
{
        connection_t *conn = obj_to_conn(obj);
        conn->cache_line_flush_instructions = SIM_attr_integer(*val);
        return Sim_Set_Ok;
}

void
init_conn()
{
        const class_data_t ic_funcs = {
                .alloc_object = ic_alloc,
                .description = "Instrumentation connection",
                .class_desc = "instrumentation connection",
                .pre_delete_instance = ic_pre_delete_conn,
                .delete_instance = ic_delete_conn,
                .kind = Sim_Class_Kind_Pseudo
        };
        conn_class = SIM_register_class("simple_cache_connection", &ic_funcs);

        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };

        SIM_REGISTER_INTERFACE(conn_class,
                               instrumentation_connection, &ic_iface);

        /* Register attributes */
        SIM_register_typed_attribute(
                conn_class, "dcache",
                get_next, (void *)Data_Cache,
                set_next, (void *)Data_Cache,
                Sim_Attr_Pseudo,
                "o|n", NULL,
                "next hierarchy object");

        SIM_register_typed_attribute(
                conn_class, "icache",
                get_next, (void *)Instr_Cache,
                set_next, (void *)Instr_Cache,
                Sim_Attr_Pseudo,
                "o|n", NULL,
                "next hierarchy object");

        SIM_register_typed_attribute(
                conn_class, "cpu",
                get_cpu, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "n|o", NULL,
                "to read out the provider");

        SIM_register_typed_attribute(
                conn_class, "tool",
                get_tool, NULL,
                NULL, NULL,
                Sim_Attr_Pseudo,
                "n|o", NULL,
                "The cache tool of the connection");

        SIM_register_attribute(
                conn_class, "issue_instructions",
                get_issue_instructions,
                set_issue_instructions,
                Sim_Attr_Pseudo,
                "b",
                "If set the connection will issue instruction accesses to the"
                " added instruction cache. If the cache block is the same as"
                " last access, no issue will be done.");

        SIM_register_attribute(
                conn_class, "uncachable_reads",
                get_uncachable_reads,
                set_uncachable_reads,
                Sim_Attr_Pseudo,
                "i",
                "Number of uncachable read accesses done.");

        SIM_register_attribute(
                conn_class, "uncachable_writes",
                get_uncachable_writes,
                set_uncachable_writes,
                Sim_Attr_Pseudo,
                "i",
                "Number of uncachable write accesses done.");

        SIM_register_attribute(
                conn_class, "prefetch_instructions",
                get_prefetch_instructions,
                set_prefetch_instructions,
                Sim_Attr_Pseudo,
                "i",
                "Number of prefetch instructions.");

        SIM_register_attribute(
                conn_class, "cache_all_flush_instructions",
                get_cache_all_flush_instructions,
                set_cache_all_flush_instructions,
                Sim_Attr_Pseudo,
                "i",
                "Number of flush entire cache instructions.");

        SIM_register_attribute(
                conn_class, "cache_line_flush_instructions",
                get_cache_line_flush_instructions,
                set_cache_line_flush_instructions,
                Sim_Attr_Pseudo,
                "i",
                "Number of cache line flush instructions.");


}


