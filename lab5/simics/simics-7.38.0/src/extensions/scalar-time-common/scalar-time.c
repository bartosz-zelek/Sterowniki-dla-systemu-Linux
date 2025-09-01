/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "scalar-time.h"

static int
cs_add_consumer(st_consumer_set_t *consumers)
{
        for (int i = 0; i < MAX_CONSUMERS; i++) {
                if (((1<<i) & *consumers) == 0) {
                        *consumers |= 1<<i;
                        return i;
                }
        }
        return -1; /* we don't have room for another consumer */
}

static void
cs_remove_consumer(st_consumer_set_t *consumers, int consumer)
{
        *consumers &= ~(st_consumer_set_t)(1<<consumer);
}

static int
cs_good_consumer(st_consumer_set_t consumers, int consumer)
{
        if (consumer < 0 || consumer >= MAX_CONSUMERS)
                return 0;
        return (consumers & (1 << consumer)) != 0;
}

static void
sv_new_counter(st_sample_vect_t *sv, double now, int64 count)
{
        st_sample_t s = { .time = now, .count = count };
        QADD(sv->samples, s);
}

static void
sv_init(st_sample_vect_t *sv)
{
        QINIT(sv->samples);
        memset(sv->consumed, 0, sizeof(sv->consumed));
}

static void
sv_clean(st_sample_vect_t *sv)
{
        QFREE(sv->samples);
        memset(sv->consumed, 0, sizeof(sv->consumed));
}

/* Get rid of samples that have been seen by all consumers. */
static void
sv_gc_samples(st_sample_vect_t *v, st_consumer_set_t consumers)
{
        int i;

        size_t min_consumed = (size_t)-1;
        for (i = 0; i < MAX_CONSUMERS; i++)
                if (consumers & (1 << i))
                        min_consumed = MIN(v->consumed[i], min_consumed);

        /* If we have at least one consumer, this will drop the samples that
           everyone has seen already. If we don't, min_consumed == -1 and we'll
           drop all samples. */
        QDROP(v->samples, min_consumed);

        for (i = 0; i < MAX_CONSUMERS; i++)
                if (consumers & (1 << i))
                        v->consumed[i] -= min_consumed;
}

static attr_value_t
sv_poll(st_sample_vect_t *sv, const char *name,
        st_consumer_set_t consumers, int consumer)
{
        int slen = QLEN(sv->samples) - sv->consumed[consumer];
        attr_value_t samples = SIM_alloc_attr_list(slen);
        for (unsigned i = 0; i < slen; i++) {
                st_sample_t *s = &QGET(sv->samples, sv->consumed[consumer] + i);
                SIM_attr_list_set_item(
                        &samples, i, SIM_make_attr_list(
                                2, SIM_make_attr_floating(s->time),
                                SIM_make_attr_uint64(s->count)));
        }
        sv->consumed[consumer] = QLEN(sv->samples);
        sv_gc_samples(sv, consumers);
        return SIM_make_attr_list(2, SIM_make_attr_string(name), samples);
}

void
st_ssp_new_counter(st_single_stats_port_t *ssp, double now, int64 count)
{
        if (ssp->sp.consumers == 0)
                return;
        sv_new_counter(&ssp->counts, now, count);
}

void
st_msp_new_counter(st_multi_stats_port_t *msp, const char *stream,
                   double now, int64 count)
{
        if (msp->sp.consumers == 0)
                return;
        st_sample_vect_t *sv = ht_lookup_str(&msp->counts, stream);
        if (!sv) {
                sv = MM_MALLOC(1, st_sample_vect_t);
                sv_init(sv);
                ht_insert_str(&msp->counts, stream, sv);
        }
        sv_new_counter(sv, now, count);
}

static void
msp_new_counters(st_multi_stats_port_t *msp, double now)
{
        if (msp->sp.consumers == 0)
                return;
        HT_FOREACH_STR(&msp->counts, it) {
                st_sample_vect_t *sv = ht_iter_str_value(it);
                sv_new_counter(sv, now, 0);
        }
}

static void
single_periodic_callback(st_stats_port_t *sp, conf_object_t *obj)
{
        st_single_stats_port_t *ssp = (st_single_stats_port_t *)sp;
        st_ssp_new_counter(ssp, SIM_time(obj), 0);
}

static void
multi_periodic_callback(st_stats_port_t *sp, conf_object_t *obj)
{
        st_multi_stats_port_t *msp = (st_multi_stats_port_t *)sp;
        msp_new_counters(msp, SIM_time(obj));
}

static int
single_add_consumer(st_stats_port_t* sp)
{
        st_single_stats_port_t *ssp = (st_single_stats_port_t *)sp;
        int consumer = cs_add_consumer(&sp->consumers);
        if (consumer >= 0)
                ssp->counts.consumed[consumer] = 0;
        return consumer;
}

static int
multi_add_consumer(st_stats_port_t *sp)
{
        st_multi_stats_port_t *msp = (st_multi_stats_port_t *)sp;
        int consumer = cs_add_consumer(&sp->consumers);
        if (consumer >= 0) {
                HT_FOREACH_STR(&msp->counts, it) {
                        st_sample_vect_t *v = ht_iter_str_value(it);
                        v->consumed[consumer] = 0;
                }
        }
        return consumer;
}

static void
single_remove_consumer(st_stats_port_t *sp, int consumer)
{
        st_single_stats_port_t *ssp = (st_single_stats_port_t *)sp;
        cs_remove_consumer(&sp->consumers, consumer);
        sv_gc_samples(&ssp->counts, sp->consumers);
}

static void
multi_remove_consumer(st_stats_port_t *sp, int consumer)
{
        st_multi_stats_port_t *msp = (st_multi_stats_port_t *)sp;
        cs_remove_consumer(&sp->consumers, consumer);
        HT_FOREACH_STR(&msp->counts, it) {
                st_sample_vect_t *sv = ht_iter_str_value(it);
                sv_gc_samples(sv, sp->consumers);
        }
}

void
st_msp_increment_counter(st_multi_stats_port_t *msp,
                         const char *stream, int64 count, conf_object_t *obj)
{
        if (msp->sp.consumers == 0)
                return;
        ASSERT(msp->sp.mutable_counters);
        st_sample_vect_t *sv = ht_lookup_str(&msp->counts, stream);
        if (sv) {
                st_sv_increment_counter(sv, count);
        } else {
                sv = MM_MALLOC(1, st_sample_vect_t);
                sv_init(sv);
                sv_new_counter(sv, SIM_time(obj), count);
                ht_insert_str(&msp->counts, stream, sv);
        }
}

static attr_value_t
single_poll(st_stats_port_t *sp, int consumer, double now)
{
        st_single_stats_port_t *ssp = (st_single_stats_port_t *)sp;
        attr_value_t ret = SIM_make_attr_list(
                1, sv_poll(&ssp->counts, "", sp->consumers, consumer));
        if (ssp->sp.mutable_counters)
                sv_new_counter(&ssp->counts, now, 0);
        return ret;
}

static attr_value_t
multi_poll(st_stats_port_t *sp, int consumer, double now)
{
        st_multi_stats_port_t *msp = (st_multi_stats_port_t *)sp;
        attr_value_t val = SIM_alloc_attr_list(ht_num_entries_str(&msp->counts));
        unsigned i = 0;
        HT_FOREACH_STR(&msp->counts, it) {
                const char *stream = ht_iter_str_key(it);
                st_sample_vect_t *sv = ht_iter_str_value(it);
                SIM_attr_list_set_item(
                        &val, i, sv_poll(sv, stream, sp->consumers, consumer));
                if (msp->sp.mutable_counters)
                        sv_new_counter(sv, now, 0);
                i++;
        }
        return val;
}

static void
single_discard_data(st_stats_port_t *sp, conf_object_t *obj)
{
        st_single_stats_port_t *ssp = (st_single_stats_port_t *)sp;
        sv_clean(&ssp->counts);
        if (ssp->sp.mutable_counters)
                sv_new_counter(&ssp->counts, SIM_time(obj), 0);
}

static void
multi_discard_data(st_stats_port_t *sp, conf_object_t *obj)
{
        st_multi_stats_port_t *msp = (st_multi_stats_port_t *)sp;
        HT_FOREACH_STR(&msp->counts, it) {
                st_sample_vect_t *v = ht_iter_str_value(it);
                QFREE(v->samples);
        }
        ht_clear_str_table(&msp->counts, 1);
}

static const st_funcs_t single_funcs = {
        .periodic_callback = single_periodic_callback,
        .add_consumer = single_add_consumer,
        .remove_consumer = single_remove_consumer,
        .poll = single_poll,
        .discard_data = single_discard_data };
static const st_funcs_t multi_funcs = {
        .periodic_callback = multi_periodic_callback,
        .add_consumer = multi_add_consumer,
        .remove_consumer = multi_remove_consumer,
        .poll = multi_poll,
        .discard_data = multi_discard_data };

static event_class_t *periodic_event;

static void
periodic_callback(conf_object_t *obj, lang_void *data)
{
        st_stats_port_t *sp = (st_stats_port_t *)data;
        sp->f->periodic_callback(sp, obj);
        SIM_event_post_time(SIM_object_clock(obj), periodic_event, obj,
                            sp->pev.period, data);
}

static void
start_periodic_callback(conf_object_t *obj, st_stats_port_t *sp)
{
        if (sp->mutable_counters) {
                sp->pev.active = 1;
                periodic_callback(obj, sp);
        }
}

static void
cancel_periodic_callback(conf_object_t *obj, st_stats_port_t *sp)
{
        if (sp->mutable_counters) {
                sp->pev.active = 0;
                SIM_event_cancel_time(SIM_object_clock(obj), periodic_event, 
                                      obj, 0, sp);
        }
}

static void
sp_init(st_stats_port_t *sp, int mutable_counters, double period,
        const st_funcs_t *funcs)
{
        sp->consumers = 0;
        sp->mutable_counters = !!mutable_counters;
        sp->pev.period = period;
        sp->pev.active = 0;
        sp->f = funcs;
}

void
st_init(conf_class_t *cc)
{
        periodic_event = SIM_register_event(
                "Scalar-time data source periodic event", cc,
                Sim_EC_Notsaved, periodic_callback, 0, 0, 0, 0);
}

void
st_ssp_init(st_single_stats_port_t *ssp, int mutable_counters, double period)
{
        sp_init(&ssp->sp, mutable_counters, period, &single_funcs);
        sv_init(&ssp->counts);
        if (ssp->sp.mutable_counters)
                sv_new_counter(&ssp->counts, 0.0, 0);
}

void
st_msp_init(st_multi_stats_port_t *msp, int mutable_counters, double period)
{
        sp_init(&msp->sp, mutable_counters, period, &multi_funcs);
        ht_init_str_table(&msp->counts, 1);
}

int
st_add_consumer(st_stats_port_t *sp, conf_object_t *obj)
{
        /* When we don't have any consumers, we collect counts in dummy
           counters. Get rid of them so that consumers don't see them. */
        if (sp->consumers == 0)
                st_discard_data(sp, obj);

        int consumer = sp->f->add_consumer(sp);
        if (sp->consumers != 0 && !sp->pev.active)
                start_periodic_callback(obj, sp);
        return consumer;
}

void
st_remove_consumer(st_stats_port_t *sp, int consumer, conf_object_t *obj)
{
        sp->f->remove_consumer(sp, consumer);
        if (sp->consumers == 0 && sp->pev.active)
                cancel_periodic_callback(obj, sp);
}

attr_value_t
st_poll(st_stats_port_t *sp, int consumer, double now)
{
        if (!cs_good_consumer(sp->consumers, consumer))
                return SIM_make_attr_invalid();
        return sp->f->poll(sp, consumer, now);
}

void
st_discard_data(st_stats_port_t *sp, conf_object_t *obj)
{
        sp->f->discard_data(sp, obj);
}

int
st_has_consumers(st_stats_port_t *sp)
{
        return sp->consumers != 0;
}
