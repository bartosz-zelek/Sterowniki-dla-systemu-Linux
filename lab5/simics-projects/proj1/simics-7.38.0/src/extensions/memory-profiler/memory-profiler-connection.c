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

#include "memory-profiler-connection.h"
#include "memory-profiler.h"

conf_class_t *connection_class;

/* TODO: This code isn't MT safe, we should never access the
   memory_profile_t data-structures, instead we should have
   private connection specific data. */

static stat_page_t *
new_stat_page(memory_profiler_t *mp, uint64 start, uint64 end)
{
        stat_page_t *sp = MM_MALLOC(1, stat_page_t);
        sp->start = start;
        sp->end = end;
        sp->granularity = mp->granularity;
        sp->len = (end - start + 1) >> mp->granularity;
        sp->counters = MM_ZALLOC(mp->page_size >> mp->granularity, uint64);
        return sp;
}

FORCE_INLINE stat_page_t *
get_stat_page(connection_t *con, unsigned view, uint64 addr)
{
        stat_page_t *cached = con->cache_page[view];
        
        /* Last accessed? Then reuse it */
        if (likely(cached && cached->start <= addr && addr <= cached->end))
                return cached;

        /* Find it in interval set */
        interval_set_t *is = con->view[view];        
        stat_page_t *sp = get_interval_ptr(is, addr);

        if (likely(sp != NULL)) {
                con->cache_page[view] = sp;  /* Cache page */
                return sp;
        }

        /* Create a new entry in interval set */
        memory_profiler_t *mp = con->mp;
        uint64 start = addr & mp->page_addr_mask;
        uint64 end = (addr & mp->page_addr_mask) + mp->page_offs_mask;
        sp = new_stat_page(mp, start, end);
        insert_interval(is, start, end, sp);
        con->cache_page[view] = sp;           /* Cache page */
        return sp;         
}

FORCE_INLINE void
update_dstat(connection_t *con, unsigned view, uint64 addr, int size)
{
        memory_profiler_t *mp = con->mp;
        stat_page_t *sp = get_stat_page(con, view, addr);
        int slots = MAX(size >> mp->granularity, 1);
        int first = (addr & mp->page_offs_mask) >> mp->granularity;
        for (int i = 0; i < slots; i++) {
                sp->counters[first + i]++;
        }
}

FORCE_INLINE void
update_istat(connection_t *con, unsigned view, uint64 addr)
{
        memory_profiler_t *mp = con->mp;
        stat_page_t *sp = get_stat_page(con, view, addr);
        int slot = (addr & mp->page_offs_mask) >> mp->granularity;
        sp->counters[slot]++;
}

static void
l_instruction_forall_cb(
        conf_object_t *obj, conf_object_t *cpu,
        instruction_handle_t *ins_handle,
        lang_void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        uint64 la = con->iq_iface->logical_address(cpu, ins_handle);

        update_istat(con, View_Execute_Logical, la);
}

static void
p_instruction_cached_instruction_cb(
        conf_object_t *obj, conf_object_t *cpu,
        cached_instruction_handle_t *ci_handle,
        instruction_handle_t *iq_handle,
        lang_void *cb_user_data)
{
        memory_profiler_t *mp = cb_user_data;
        connection_t *con = obj_to_con(obj);
        uint64 iaddr = con->iq_iface->physical_address(cpu, iq_handle);        
        stat_page_t *sp = get_stat_page(con, View_Execute_Physical, iaddr);
        int slot = (iaddr & mp->page_offs_mask) >> mp->granularity;

        con->ci_iface->add_counter(cpu, ci_handle, &sp->counters[slot], false);
}

static void
p_read_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                 memory_handle_t *mem, lang_void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        uint64 pa = con->mq_iface->physical_address(cpu, mem);
        int size = con->mq_iface->get_bytes(con->cpu, mem).size;

        update_dstat(con, View_Read_Physical, pa, size);
}

static void
l_read_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                 memory_handle_t *mem, lang_void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        uint64 lina = con->mq_iface->logical_address(cpu, mem);
        int size = con->mq_iface->get_bytes(cpu, mem).size;
        
        update_dstat(con, View_Read_Logical, lina, size);
}

static void
p_write_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                 memory_handle_t *mem, lang_void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        uint64 pa = con->mq_iface->physical_address(cpu, mem);
        int size = con->mq_iface->get_bytes(cpu, mem).size;

        update_dstat(con, View_Write_Physical, pa, size);
}

static void
l_write_forall_cb(conf_object_t *obj, conf_object_t *cpu,
                  memory_handle_t *mem,
                  lang_void *cb_user_data)
{
        connection_t *con = obj_to_con(obj);
        uint64 lina = con->mq_iface->logical_address(cpu, mem);
        int size = con->mq_iface->get_bytes(cpu, mem).size;

        update_dstat(con, View_Write_Logical, lina, size);
}

void
configure_connection(connection_t *con)
{
        conf_object_t *obj = &con->obj;
        conf_object_t *cpu = con->cpu;
        
        const cpu_instrumentation_subscribe_interface_t *iface = con->is_iface;

        if (con->read_physical) {
                iface->register_read_before_cb(
                        cpu, obj, CPU_Access_Scope_Explicit, p_read_forall_cb,
                        con->mp);
                iface->register_read_before_cb(
                        cpu, obj, CPU_Access_Scope_Implicit, p_read_forall_cb,
                        con->mp);
        }
        if (con->read_logical) {
                iface->register_read_before_cb(
                        cpu, obj, CPU_Access_Scope_Explicit, l_read_forall_cb,
                        con->mp);
                iface->register_read_before_cb(
                        cpu, obj, CPU_Access_Scope_Implicit, l_read_forall_cb,
                        con->mp);
        }
        if (con->write_physical) {
                iface->register_write_before_cb(
                        cpu, obj, CPU_Access_Scope_Explicit, p_write_forall_cb,
                        con->mp);
                iface->register_write_before_cb(
                        cpu, obj, CPU_Access_Scope_Implicit, p_write_forall_cb,
                        con->mp);
        }
        if (con->write_logical) {
                iface->register_write_before_cb(
                        cpu, obj, CPU_Access_Scope_Explicit, l_write_forall_cb,
                        con->mp);
                iface->register_write_before_cb(
                        cpu, obj, CPU_Access_Scope_Implicit, l_write_forall_cb,
                        con->mp);
        }
        if (con->execute_physical)
                iface->register_cached_instruction_cb(
                        cpu, obj, p_instruction_cached_instruction_cb, con->mp);
        if (con->execute_logical)
                iface->register_instruction_before_cb(
                        cpu, obj, l_instruction_forall_cb, con->mp);
}

static conf_object_t *
ic_alloc(void *arg)
{
        connection_t *con = MM_ZALLOC(1, connection_t);

        for (int v = 0; v < View_Num; v++)
                con->view[v] = new_interval(0);

        return &con->obj;
}

// instrumentation_connection::pre_delete_instance
static void
ic_pre_delete_connection(conf_object_t *obj)
{
        connection_t *con = obj_to_con(obj);

        con->is_iface->remove_connection_callbacks(con->cpu, obj);
}

static void
for_page_delete(uint64 start, uint64 stop, void *p, void *data)
{
        stat_page_t *sp = p;
        MM_FREE(sp->counters);
        MM_FREE(sp);
}

static int
ic_delete_connection(conf_object_t *obj)
{
        connection_t *con = obj_to_con(obj);
        for (int i = 0; i < View_Num; i++)
                for_all_intervals(con->view[i], for_page_delete, NULL);

        
        MM_FREE(obj);
        return 0;
}

// instrumentation_connection::enable
static void
ic_enable(conf_object_t *obj)
{
        connection_t *c = obj_to_con(obj);
        c->is_iface->enable_connection_callbacks(c->cpu, obj);
}

// instrumentation_connection::enable
static void
ic_disable(conf_object_t *obj)
{
        connection_t *c = obj_to_con(obj);
        c->is_iface->disable_connection_callbacks(c->cpu, obj);
}


#define MAKE_ATTR_SET_GET_BOOL(option, desc)                            \
static set_error_t                                                      \
set_ ## option (void *arg, conf_object_t *obj, attr_value_t *val,       \
              attr_value_t *idx)                                        \
{                                                                       \
        connection_t *con = obj_to_con(obj);                            \
        con-> option = SIM_attr_boolean(*val);                          \
        return Sim_Set_Ok;                                              \
}                                                                       \
                                                                        \
static attr_value_t                                                     \
get_ ## option (void *arg, conf_object_t *obj, attr_value_t *idx)       \
{                                                                       \
        connection_t *mp = obj_to_con(obj);                             \
        return SIM_make_attr_boolean(mp-> option);                      \
}

FOR_OPTIONS(MAKE_ATTR_SET_GET_BOOL)

#define ATTR_REGISTER(option, desc)                     \
        SIM_register_typed_attribute(                   \
                connection_class,  #option,             \
                get_ ## option, NULL,                   \
                set_ ## option, NULL,                   \
                Sim_Attr_Optional | Sim_Attr_Read_Only, \
                "b", NULL,                              \
                "Enables " desc);

void
init_connection()
{
        const class_data_t ic_funcs = {
                .alloc_object = ic_alloc,
                .class_desc = "instrumentation connection",
                .description = "Instrumentation connection.",
                .pre_delete_instance = ic_pre_delete_connection,
                .delete_instance = ic_delete_connection,
                .kind = Sim_Class_Kind_Pseudo
        };
        connection_class = SIM_register_class("memory_profiler_connection",
                                              &ic_funcs);
        
        static const instrumentation_connection_interface_t ic_iface = {
                .enable = ic_enable,
                .disable = ic_disable,
        };
        SIM_REGISTER_INTERFACE(connection_class,
                               instrumentation_connection, &ic_iface);

        FOR_OPTIONS(ATTR_REGISTER);
}
