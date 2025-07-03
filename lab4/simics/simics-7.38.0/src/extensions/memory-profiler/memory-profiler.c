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

#include "memory-profiler.h"
#include "memory-profiler-connection.h"

#include <stdio.h>

#include <simics/simulator/conf-object.h>
#include <simics/base/sim-exception.h>

typedef VECT(attr_value_t) attr_value_vect_t;

FORCE_INLINE memory_profiler_t *
obj_to_mp(conf_object_t *obj)
{
        return (memory_profiler_t *)obj;
}


static conf_object_t *
alloc_object(void *arg)
{
        memory_profiler_t *mp = MM_ZALLOC(1, memory_profiler_t);
        
        mp->granularity = 0;
        mp->page_size = 4096; // must be multiple of 2
        mp->page_offs_mask = mp->page_size - 1;
        mp->page_addr_mask = ~mp->page_offs_mask;
        
        return &mp->obj;
}

static void
finalize_instance(conf_object_t *obj)
{
}

static int
delete_instance(conf_object_t *obj)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        ASSERT_FMT(VEMPTY(mp->connections),
                   "%s deleted with active connections", SIM_object_name(obj));
        MM_FREE(obj);
        return 0;
}

static connection_t *
new_connection(memory_profiler_t *mp, conf_object_t *cpu, attr_value_t attr)
{
        strbuf_t sb = SB_INIT;
        sb_addfmt(&sb, "%s.con%d",
                  SIM_object_name(&mp->obj), mp->next_connection_number);
        conf_object_t *c = SIM_create_object(connection_class, sb_str(&sb),
                                             attr);
        sb_free(&sb);
        
        if (!c)
                return NULL;

        mp->next_connection_number++;
        connection_t *con = obj_to_con(c);
        con->cpu = cpu;
        con->mp = mp;

        /* Create cached interfaces for this provider */
        con->is_iface = SIM_C_GET_INTERFACE(cpu, cpu_instrumentation_subscribe);
        con->iq_iface = SIM_C_GET_INTERFACE(cpu, cpu_instruction_query);
        con->mq_iface = SIM_C_GET_INTERFACE(cpu, cpu_memory_query);
        con->ci_iface = SIM_C_GET_INTERFACE(cpu, cpu_cached_instruction);
        return con;
}

static conf_object_t *
it_connect(conf_object_t *obj, conf_object_t *provider, attr_value_t attr)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        connection_t *con = new_connection(mp, provider, attr);

        if (!con)
                return NULL;
        
        VADD(mp->connections, con);
        configure_connection(con);
        return &con->obj;
}

static void
it_disconnect(conf_object_t *obj, conf_object_t *con_obj)
{
        VREMOVE_FIRST_MATCH(obj_to_mp(obj)->connections, obj_to_con(con_obj));
        SIM_delete_object(con_obj);
}

static void
for_page(uint64 start, uint64 stop, void *p, void *data)
{
        attr_value_vect_t *v = data;
        stat_page_t *sp = p;
        unsigned granularity = sp->granularity;
        attr_value_t counters = SIM_alloc_attr_list(sp->len);
        for (int i = 0; i < sp->len; i++)
                SIM_attr_list_set_item(&counters, i,
                                       SIM_make_attr_uint64(sp->counters[i]));
        
        attr_value_t e = SIM_make_attr_list(4,
                                            SIM_make_attr_uint64(start),
                                            SIM_make_attr_uint64(stop),
                                            SIM_make_attr_uint64(granularity),
                                            counters);
        VADD(*v, e);
}

static void
for_page_add_to_iter(uint64 start, uint64 stop, void *p, void *data)
{
        memory_profiler_iter_t *mpi = data;        
        stat_page_t *sp = p;
        VADD(mpi->pages, sp);
}

typedef struct {
        uint64 start;
        uint64 stop;
        uint64 integer;
} int_data_t;

static void
for_page_sum(uint64 start, uint64 stop, void *p, void *data)
{
        int_data_t *id = data;
        stat_page_t *sp = p;

        if (id->start <= start && stop <= id->stop) {
                // whole page within interval, add all
                for (unsigned i = 0; i < sp->len; i++) {
                        if (sp->counters[i])
                                id->integer += sp->counters[i];
                }
        } else {
                for (unsigned i = 0; i < sp->len; i++) {
                        uint64 a = sp->start + (i << sp->granularity);
                        if (id->start <= a && a <= id->stop)
                                id->integer += sp->counters[i];
                }
        }
}

static void
for_page_max(uint64 start, uint64 stop, void *p, void *data)
{
        int_data_t *id = data;
        stat_page_t *sp = p;

        if (id->start <= start && stop <= id->stop) {
                // whole page within interval, check all
                for (unsigned i = 0; i < sp->len; i++)
                        if (sp->counters[i] > id->integer)
                                id->integer = sp->counters[i];
        } else {
                for (unsigned i = 0; i < sp->len; i++) {
                        uint64 a = sp->start + (i << sp->granularity);
                        if (id->start <= a && a <= id->stop)
                                if (sp->counters[i] > id->integer)
                                        id->integer = sp->counters[i];
                }
        }
}

static void
for_page_free(uint64 start, uint64 stop, void *p, void *data)
{
        stat_page_t *sp = p;
        MM_FREE(sp->counters);
        MM_FREE(sp);
}

static attr_value_t
get_stat(void *param, conf_object_t *obj, attr_value_t *idx)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        attr_value_vect_t list = VNULL;
        if (!SIM_attr_is_integer(*idx)) {
                VT_frontend_exception(SimExc_Index,
                                      "Illegal view index");
                return SIM_make_attr_nil();
        }
        
        
        int view = SIM_attr_integer(*idx);

        if (view < 0 || View_Num <= view) {
                return SIM_make_attr_nil();
        }

        VFORP(mp->connections, connection_t, con)
                for_all_intervals(con->view[view], for_page, &list);

        int len = VLEN(list);
        attr_value_t ret = SIM_alloc_attr_list(len);
        for (int i = 0; i < len; i++)
                SIM_attr_list_set_item(&ret, i, VGET(list, i));
        return ret;
} 

static void
clear_view_stat(memory_profiler_t *mp, int view)
{
        VFORP(mp->connections, connection_t, con) {
                for_all_intervals(con->view[view], for_page_free, NULL);
                clear_interval(con->view[view]);
        }
}

static void
clear_all_stat(memory_profiler_t *mp)
{
        for(int view = 0; view < 6; view++)
                clear_view_stat(mp, view);
}

static set_error_t
set_stat(void *param, conf_object_t *obj, attr_value_t *val,
           attr_value_t *idx)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        int view = SIM_attr_integer(*idx);
        clear_view_stat(mp, view);
        return Sim_Set_Ok;
}

static uint64
next(addr_prof_iter_t *iter)
{
        memory_profiler_iter_t *mpi = (memory_profiler_iter_t *)iter;
        unsigned gran = mpi->mp->granularity;

        for (unsigned c = mpi->current_page; c < VLEN(mpi->pages); c++) {
                stat_page_t *sp = VGET(mpi->pages, c);
                unsigned len = sp->len;
                for (unsigned i = mpi->current_page_offs_idx; i < len; i++) {
                        uint64 addr = sp->start + (i << gran);
                        if (!(mpi->start <= addr && addr <= mpi->stop))
                                continue;
                        if (sp->counters[i]) {
                                mpi->super.addr = addr;
                                if (i == len - 1) {
                                        mpi->current_page = c + 1;
                                        mpi->current_page_offs_idx = 0;
                                } else {
                                        mpi->current_page = c;
                                        mpi->current_page_offs_idx = i + 1;
                                }
                                return sp->counters[i];
                        }
                }
                mpi->current_page_offs_idx = 0;
        }
        return 0;
}

static void
destroy(addr_prof_iter_t *iter)
{
        memory_profiler_iter_t *mpi = (memory_profiler_iter_t *)iter;
        VFREE(mpi->pages);
        MM_FREE(iter);
}

static int
cmp(const void *_a, const void *_b)
{
        const stat_page_t *a = _a;
        const stat_page_t *b = _b;
        ASSERT(a->start != b->start);
        if (a->start < b->start)
                return -1;
        else
                return 1;
}

static addr_prof_iter_t *
iter(conf_object_t *obj, unsigned view, generic_address_t start,
     generic_address_t stop)
{
        ASSERT(view < View_Num);
        memory_profiler_t *mp = obj_to_mp(obj);        
        memory_profiler_iter_t *mpi = MM_ZALLOC(1, memory_profiler_iter_t);
        
        mpi->super.next = next;
        mpi->super.destroy = destroy;
        
        VINIT(mpi->pages);
        mpi->current_page = 0;
        mpi->current_page_offs_idx = 0;
        mpi->start = start;
        mpi->stop = stop;
        mpi->mp = mp;

        VFORP(mp->connections, connection_t, con)
                for_some_intervals(con->view[view], start, stop,
                                   for_page_add_to_iter, mpi);

        /* check that the collected pages are sorted */
        VSORT(mpi->pages, cmp);

        /* merge pages that cover the same address range (comming from
           different cpus) */
        for (int i = 0; i < VLEN(mpi->pages) - 1;) {
                stat_page_t *this = VGET(mpi->pages, i);
                stat_page_t *next = VGET(mpi->pages, i + 1);
                if (this->start == next->start) {
                        /* same address range, merge */
                        ASSERT(this->end == next->end);
                        ASSERT(this->len == next->len);
                        for (int c = 0; c < this->len; c++)
                                this->counters[c] += next->counters[c];
                        /* remove next */
                        VDELETE_ORDER(mpi->pages, i + 1);
                } else {
                        i++;
                }
        }
                
        mpi->super.addr = 0;
        mpi->first = true;
        return &mpi->super;
}

static uint64
sum(conf_object_t *obj, unsigned view, generic_address_t start,
    generic_address_t stop)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        int_data_t id = { .start = start, .stop = stop, .integer = 0 };
        VFORP(mp->connections, connection_t, con)
                for_some_intervals(
                        con->view[view], start, stop, for_page_sum, &id);
        return id.integer;
}

static uint64
max(conf_object_t *obj, unsigned view, generic_address_t start,
    generic_address_t stop)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        int_data_t id = { .start = start, .stop = stop, .integer = 0 };
        VFORP(mp->connections, connection_t, con)
                for_some_intervals(
                        con->view[view], start, stop, for_page_max, &max);
        return id.integer;
}

static unsigned
granularity_log2(conf_object_t *obj, unsigned view)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        return mp->granularity;
}

static int
address_bits(conf_object_t *obj, unsigned view)
{
        return 64;
}

static int
physical_addresses(conf_object_t *obj, unsigned view)
{
        return (view & 1) == 0 ? 1 : 0;
}

static const char *
description(conf_object_t *obj, unsigned view)
{
        switch (view) {
        case View_Read_Physical:    return "Read physical address";
        case View_Read_Logical:     return "Read logical address";
        case View_Write_Physical:   return "Write physical address";
        case View_Write_Logical:    return "Write logical address";
        case View_Execute_Physical: return "Execute physical address";
        case View_Execute_Logical:  return "Execute logical address";
        }
        return "Unknown view";
}

static unsigned
num_views(conf_object_t *obj)
{
        return View_Num;
}

static attr_value_t
get_granularity_log2(conf_object_t *obj)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        return SIM_make_attr_int64(mp->granularity);
}

static set_error_t
set_granularity_log2(conf_object_t *obj, attr_value_t *val)
{
        memory_profiler_t *mp = obj_to_mp(obj);
        int64 g = SIM_attr_integer(*val);
        if (0 <= g || g <= 63) {
                clear_all_stat(mp);
                mp->granularity = g;
                // We store 4096 (1 << 12) entries per page 
                mp->page_size = 1ULL << MIN(g + 12, 63);
                mp->page_offs_mask = mp->page_size - 1;
                mp->page_addr_mask = ~mp->page_offs_mask;
                return Sim_Set_Ok;
        }
        return Sim_Set_Illegal_Value;
}

void
init_local()
{
        const class_data_t funcs = {
                .alloc_object = alloc_object,
                .delete_instance = delete_instance,
                .finalize_instance = finalize_instance,
                .class_desc = "memory accesses profiler",
                .description = "The memory-profiler tool counts different"
                " memory accesses together with the associated address and"
                " presents the data in a table. Read, written, and executed"
                " addresses can be collected separately at a given address"
                " type - virtual or physical. Use the new-memory-profiler"
                " command to select the data be be collected. The profile"
                " can be displayed in a table at a given address granularity"
                " with the tool's profile command. The data can"
                " also be displayed when disassembling instructions by adding"
                " a view to each line with a processor's aprof-view command.",
                .kind = Sim_Class_Kind_Pseudo
        };

        conf_class_t *cl = SIM_register_class("memory_profiler",
                                              &funcs);

        static const instrumentation_tool_interface_t it = {
                .connect = it_connect,
                .disconnect = it_disconnect
        };
        SIM_REGISTER_INTERFACE(cl, instrumentation_tool, &it);

        static const address_profiler_interface_t iface = {
                .iter = iter,
                .sum = sum,
                .max = max,
                .granularity_log2 = granularity_log2,
                .address_bits = address_bits,
                .physical_addresses = physical_addresses,
                .description = description,
                .num_views = num_views
        };
        SIM_register_interface(cl, ADDRESS_PROFILER_INTERFACE, &iface);
        
        SIM_register_typed_attribute(
                cl, "stat",
                get_stat, NULL,
                set_stat, NULL,
                Sim_Attr_Integer_Indexed | Sim_Attr_Pseudo,
                "[[iii[i*]]*]", NULL,
                "Statistics indexed by view 0-5\n");
        
        SIM_register_attribute(cl, "granularity_log2", 
                               get_granularity_log2,
                               set_granularity_log2,
                               Sim_Attr_Pseudo,
                               "i",
                               "Granularity in log2 for data be collected."
                               " Valid range is 0 - 63");

        init_connection();
}
