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

#include "generic-cycle-iface.h"

#define ONE_SECOND_AS_PS 1000000000000

/* How many picoseconds have been spend to run 'cycles' at 'frequency' */
bigtime_t
cycle_count_as_ps(cycles_t cycles, uint64 frequency)
{
        /* There is no risk of overflow here because ONE_SECOND_AS_PS is about
           2^40ps, and cycles is fitting in an uint64 */
        return bigtime_div_floor(
                bigtime_mul(bigtime_from_ps_lo(cycles),
                            ONE_SECOND_AS_PS),
                (int64)frequency);
}

/* How many 'cycles' can we run before passing 'time' at 'frequency'. The
   result is always rounded down so the cycle count stops just before
   'time'. Return true if the time could be represented as cycles, false
   otherwise */
bool
ps_as_cycle_count_floor(bigtime_t time, uint64 frequency, cycles_t *cycles)
{
        bigtime_t result = bigtime_div_floor(
                bigtime_mul(time, (int64)frequency),
                ONE_SECOND_AS_PS);
        *cycles = (cycles_t)bigtime_as_ps_lo(result);
        /* make sure the result fits in cycles_t */
        return bigtime_as_ps_hi(result) == 0 || bigtime_as_ps_hi(result) == -1;
}

/* How many 'cycles' should we run to approximate 'time'. The result is rounded
   to the nearest cycle. Return true if the time could be represented as
   cycles, false otherwise */
bool
ps_as_cycle_count_nearest(bigtime_t time, uint64 frequency, cycles_t *cycles)
{
        /* Round to nearest cycle, rounding up if exactly half-way rather than
           rounding to an even cycle - event posting is relative and rounding
           should not depend on the parity of the current cycle.
           See bug 8619. */
        bigtime_t result = bigtime_div_floor(
                bigtime_add(bigtime_mul(time, (int64)frequency),
                            duration_from_ps(ONE_SECOND_AS_PS / 2)),
                ONE_SECOND_AS_PS);
        uint64 lo = bigtime_as_ps_lo(result);
        *cycles = (cycles_t)lo;
        /* make sure the result fits in cycles_t */
        return (bigtime_as_ps_hi(result) ==  0 && (lo >> 63) == 0)
            || (bigtime_as_ps_hi(result) == -1 && (lo >> 63) == 1);
}

bigtime_t
time_offset_from_attr(attr_value_t val)
{
        if (SIM_attr_is_floating(val))
                return bigtime_from_sec(SIM_attr_floating(val));
        else
                return bigtime_from_attr(val);
}

attr_value_t
time_offset_as_attr(bigtime_t time_offset)
{
        return bigtime_to_attr(time_offset);
}

static bigtime_t
get_time_in_big_ps(bigtime_t time_offset,
                   cycles_t cycle_count, uint64 freq)
{
        if (cycle_count && !freq)
                return (bigtime_t) BIGTIME_ILLEGAL;
        return bigtime_add_bigtime(
                time_offset,
                (freq) ? cycle_count_as_ps(cycle_count, freq) 
                       : (bigtime_t) BIGTIME_ZERO);
}

local_time_t
generic_get_time_in_ps(conf_object_t *clock, bigtime_t time_offset,
                       cycles_t cycle_count, uint64 freq)
{
        return local_time_from_bigtime(
                clock, get_time_in_big_ps(time_offset, cycle_count, freq));
}

bool
generic_cycles_from_ps(local_time_t when, bigtime_t time_offset, uint64 freq,
                       cycles_t *cycles)
{
        return ps_as_cycle_count_floor(bigtime_sub_bigtime(
                                               local_time_as_bigtime(when), 
                                               time_offset),
                                       freq, cycles);
}

bool
generic_delta_from_ps(local_time_t when, bigtime_t time_offset,
                      cycles_t cycle_count, uint64 freq, cycles_t *delta)
{
        cycles_t cycles;
        bool ret = generic_cycles_from_ps(when, time_offset, freq, &cycles);
        *delta = cycles - cycle_count;
        return ret;
}

bigtime_t
adjusted_time_offset(bigtime_t current_offset, cycles_t cycle_count,
                     uint64 old_freq, uint64 new_freq)
{
        bigtime_t current_time = get_time_in_big_ps(
                current_offset, cycle_count, old_freq);
        bigtime_t raw_cc_time = get_time_in_big_ps(
                (bigtime_t) BIGTIME_ZERO, cycle_count, new_freq);
        return bigtime_sub_bigtime(current_time, raw_cc_time);
}

bool
posting_cycle_delta(bigtime_t ps, uint64 freq, cycles_t *delta)
{
        cycles_t result;
        if (!ps_as_cycle_count_nearest(ps, freq, &result))
                return false;
        *delta = (result <= 0) ? 1 : result;
        return true;
}

const char *
check_post_cycle_params(cycles_t cycles, event_class_t *evclass)
{
        if (cycles < 0)
                return "posting on negative time";
        else if (evclass->flags & Sim_EC_Machine_Sync)
                return  "machine sync event can not"
                        " be posted directly on a cpu; use"
                        " SIM_event_post_time() or SIM_event_post_cycle()"
                        " instead.";
        else
                return NULL;
}

static const char *
post_time_in_big_ps(conf_object_t *NOTNULL queue_obj,
                    event_class_t *NOTNULL evclass,
                    conf_object_t *NOTNULL poster_obj,
                    bigtime_t picoseconds,
                    lang_void *user_data,
                    post_cycle_t f,
                    uint64 freq)
{
        if (bigtime_lt(picoseconds, (bigtime_t) BIGTIME_ZERO))
                return "posting on negative time";
        cycles_t delta;
        if (!posting_cycle_delta(picoseconds, freq, &delta))
                return "posting too far ahead";
        (*f)(queue_obj, evclass, poster_obj, delta, user_data);
        return NULL;
}

const char *
generic_post_time(conf_object_t *NOTNULL queue_obj,
                  event_class_t *NOTNULL evclass,
                  conf_object_t *NOTNULL poster_obj,
                  double seconds,
                  lang_void *user_data,
                  post_cycle_t f,
                  uint64 freq)
{
        bigtime_t ps = bigtime_from_sec(seconds);
        if (bigtime_is_illegal(ps))
                return  "post_time(): posting time can not be represented "
                        "in simulation time";
        return post_time_in_big_ps(queue_obj, evclass, poster_obj,
                                   ps, user_data,
                                   f, freq);
}

const char *
generic_post_time_in_ps(conf_object_t *NOTNULL queue_obj,
                        event_class_t *NOTNULL evclass,
                        conf_object_t *NOTNULL poster_obj,
                        duration_t picoseconds,
                        lang_void *user_data,
                        post_cycle_t f,
                        uint64 freq)
{
        return post_time_in_big_ps(
                queue_obj, evclass, poster_obj, 
                bigtime_from_ps_lo(duration_as_ps(picoseconds)),
                user_data, f, freq);
}

static bigtime_t
find_next_time_in_big_ps(conf_object_t *NOTNULL queue,
                         event_class_t *NOTNULL evclass,
                         conf_object_t *NOTNULL obj,
                         int (*pred)(lang_void *data, lang_void *match_data),
                         lang_void *match_data,
                         find_next_cycle_t f,
                         uint64 freq)
{
        cycles_t cycles = (*f)(queue, evclass, obj, pred, match_data);
        return cycle_count_as_ps(cycles, freq);
}

double
generic_find_next_time(conf_object_t *NOTNULL queue,
                       event_class_t *NOTNULL evclass,
                       conf_object_t *NOTNULL obj,
                       int (*pred)(lang_void *data, lang_void *match_data),
                       lang_void *match_data,
                       find_next_cycle_t f,
                       uint64 freq)
{
        bigtime_t ps = find_next_time_in_big_ps(queue, evclass, obj,
                                                pred, match_data, f, freq);
        if (bigtime_is_illegal(ps)
            || bigtime_lt(ps, (bigtime_t) BIGTIME_ZERO))
                return -1.0;
        else
                return bigtime_as_sec(ps);
}

duration_t
generic_find_next_time_in_ps(conf_object_t *NOTNULL clock,
                             event_class_t *NOTNULL evclass,
                             conf_object_t *NOTNULL obj,
                             int (*pred)(lang_void *data, 
                                         lang_void *match_data),
                             lang_void *match_data,
                             find_next_cycle_t f,
                             uint64 freq)
{
        bigtime_t ps = find_next_time_in_big_ps(clock, evclass, obj, pred,
                                                match_data, f, freq);
        if (bigtime_is_illegal(ps)
            || bigtime_lt(ps, (bigtime_t) BIGTIME_ZERO)
            || bigtime_as_ps_hi(ps) != 0 
            || bigtime_as_ps_lo(ps) > 0x7fffffffffffffff)
                return ILLEGAL_DURATION;
        else
                return duration_from_ps((int64)bigtime_as_ps_lo(ps));
}
