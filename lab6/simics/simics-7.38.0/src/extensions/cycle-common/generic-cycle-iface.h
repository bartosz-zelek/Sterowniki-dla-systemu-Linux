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

#ifndef CYCLE_COMMON_GENERIC_CYCLE_IFACE_H
#define CYCLE_COMMON_GENERIC_CYCLE_IFACE_H

#include <simics/base/bigtime.h>
#include <simics/base/local-time.h>
#include <simics/base/event.h>
#include <simics/base/time.h>
#include <simics/base/conf-object.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define TIME_OFFSET_ATTRTYPE BIGTIME_ATTRTYPE "|f"

EXPORTED bigtime_t adjusted_time_offset(
        bigtime_t current_offset, cycles_t cycle_count,
        uint64 old_freq, uint64 new_freq);
EXPORTED bigtime_t time_offset_from_attr(attr_value_t val);
EXPORTED attr_value_t time_offset_as_attr(bigtime_t time_offset);

EXPORTED local_time_t generic_get_time_in_ps(conf_object_t *clock, 
                                             bigtime_t time_offset,
                                             cycles_t cycle_count, uint64 freq);
EXPORTED bool generic_cycles_from_ps(local_time_t when,
                                     bigtime_t time_offset,
                                     uint64 freq, cycles_t *cycles);
EXPORTED bool generic_delta_from_ps(local_time_t when, bigtime_t time_offset,
                                    cycles_t cycle_count, uint64 freq,
                                    cycles_t *delta);

EXPORTED bigtime_t cycle_count_as_ps(cycles_t cycles, uint64 frequency);
EXPORTED bool ps_as_cycle_count_floor(bigtime_t time, uint64 frequency, 
                                      cycles_t *cycles);
EXPORTED bool ps_as_cycle_count_nearest(bigtime_t time, uint64 frequency,
                                        cycles_t *cycles);

EXPORTED bool posting_cycle_delta(bigtime_t ps, uint64 freq, cycles_t *delta);
EXPORTED const char *check_post_cycle_params(cycles_t cycles,
                                             event_class_t *evclass);

typedef void (*post_cycle_t)(conf_object_t *NOTNULL queue_obj,
                             event_class_t *NOTNULL evclass,
                             conf_object_t *NOTNULL poster_obj,
                             cycles_t cycles,
                             lang_void *user_data);

EXPORTED const char *generic_post_time(conf_object_t *NOTNULL queue_obj,
                                       event_class_t *NOTNULL evclass,
                                       conf_object_t *NOTNULL poster_obj,
                                       double seconds,
                                       lang_void *user_data,
                                       post_cycle_t f,
                                       uint64 freq);

EXPORTED const char *generic_post_time_in_ps(conf_object_t *NOTNULL queue_obj,
                                             event_class_t *NOTNULL evclass,
                                             conf_object_t *NOTNULL poster_obj,
                                             duration_t picoseconds,
                                             lang_void *user_data,
                                             post_cycle_t f,
                                             uint64 freq);

typedef cycles_t (*find_next_cycle_t)(conf_object_t *NOTNULL queue_obj,
                                      event_class_t *NOTNULL evclass,
                                      conf_object_t *NOTNULL poster_obj,
                                      int (*pred)(lang_void *data, 
                                                  lang_void *match_data),
                                      lang_void *match_data);

EXPORTED double generic_find_next_time(conf_object_t *NOTNULL queue,
                                       event_class_t *NOTNULL evclass,
                                       conf_object_t *NOTNULL obj,
                                       int (*pred)(lang_void *data, 
                                                   lang_void *match_data),
                                       lang_void *match_data,
                                       find_next_cycle_t f,
                                       uint64 freq);
EXPORTED duration_t generic_find_next_time_in_ps(
        conf_object_t *NOTNULL clock,
        event_class_t *NOTNULL evclass,
        conf_object_t *NOTNULL obj,
        int (*pred)(lang_void *data, 
                    lang_void *match_data),
        lang_void *match_data,
        find_next_cycle_t f,
        uint64 freq);

#if defined(__cplusplus)
}
#endif

#endif
