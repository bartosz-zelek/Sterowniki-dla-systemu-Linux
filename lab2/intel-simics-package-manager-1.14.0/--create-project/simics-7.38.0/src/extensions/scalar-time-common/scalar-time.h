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

#ifndef SCALAR_TIME_H
#define SCALAR_TIME_H

#include <simics/simulator-api.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define MAX_CONSUMERS 32

typedef uint32 st_consumer_set_t;

typedef struct {
        double time;
        int64 count;
} st_sample_t;

typedef struct {
        QUEUE(st_sample_t) samples;
        size_t consumed[MAX_CONSUMERS];
} st_sample_vect_t;

typedef struct st_funcs st_funcs_t;

typedef struct {
        st_consumer_set_t consumers;
        int mutable_counters;
        struct {
                double period; /* seconds */
                int active;
        } pev;
        const st_funcs_t *f;
} st_stats_port_t;

struct st_funcs {
        void (*periodic_callback)(st_stats_port_t *sp, conf_object_t *obj);
        int (*add_consumer)(st_stats_port_t *sp);
        void (*remove_consumer)(st_stats_port_t *sp, int consumer);
        attr_value_t (*poll)(st_stats_port_t *sp, int consumer, double now);
        void (*discard_data)(st_stats_port_t *sp, conf_object_t *obj);
};

typedef struct {
        st_stats_port_t sp;
        st_sample_vect_t counts;
} st_single_stats_port_t;

typedef struct {
        st_stats_port_t sp;
        ht_str_table_t counts; /* name -> sample_vect_t */
} st_multi_stats_port_t;

FORCE_INLINE void
st_sv_increment_counter(st_sample_vect_t *sv, int64 count)
{
        QLAST(sv->samples).count += count;
}

/* Counter increment functions. May only be called for stats ports with mutable
   counters. */
FORCE_INLINE void
st_ssp_increment_counter(st_single_stats_port_t *ssp, int64 count)
{
        st_sv_increment_counter(&ssp->counts, count);
}
void st_msp_increment_counter(st_multi_stats_port_t *msp,
                              const char *stream, int64 count,
                              conf_object_t *obj);

/* Create a new counter. May be called regardless of whether counters are
   mutable or not. */
void st_ssp_new_counter(st_single_stats_port_t *ssp, double now, int64 count);
void st_msp_new_counter(st_multi_stats_port_t *msp, const char *stream,
                        double now, int64 count);

/* Call once in your module init function. */
void st_init(conf_class_t *cc);

/* Initialize stats ports. If mutable_counters is true, you may call the
   counter increment functions; also, new counters will be provided every
   period simulated seconds, as well as whenever the old counter can't be used
   for some reason. */
void st_ssp_init(st_single_stats_port_t *ssp, int mutable_counters,
                 double period);
void st_msp_init(st_multi_stats_port_t *msp, int mutable_counters,
                 double period);

/* Functions to help you implement the scalar_time interface. */
int st_add_consumer(st_stats_port_t *sp, conf_object_t *obj);
void st_remove_consumer(st_stats_port_t *sp, int consumer, conf_object_t *obj);
attr_value_t st_poll(st_stats_port_t *sp, int consumer, double now);
void st_discard_data(st_stats_port_t *sp, conf_object_t *obj);

int st_has_consumers(st_stats_port_t *sp);

#if defined(__cplusplus)
}
#endif

#endif  /* ! SCALAR_TIME_H */
