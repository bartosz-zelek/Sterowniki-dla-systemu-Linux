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

#ifndef SIMICS_BASE_LOCAL_TIME_H
#define SIMICS_BASE_LOCAL_TIME_H

#include <simics/base/types.h>

#include <simics/base/duration.h>
#include <simics/base/bigtime.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        /* The clock is an object which implements the cycle interface. The
           time stamp given in this struct is only valid in the clocks' "time
           space". */
        conf_object_t *clock;

        bigtime_t t;
} local_time_t;

#ifndef PYWRAP

#define ILLEGAL_LOCAL_TIME(clock) ((local_time_t){ (clock), BIGTIME_ILLEGAL })
#define ZERO_LOCAL_TIME(clock) ((local_time_t){ (clock), BIGTIME_ZERO })

FORCE_INLINE conf_object_t *
local_time_clock(local_time_t t) { return t.clock; }

FORCE_INLINE local_time_t
local_time_from_bigtime(conf_object_t *clock, bigtime_t t)
{
        local_time_t gt;
        gt.clock = clock;
        gt.t = t;
        return gt;
}

FORCE_INLINE local_time_t
local_time_from_sec(conf_object_t *clock, double t)
{ return local_time_from_bigtime(clock, bigtime_from_sec(t)); }

FORCE_INLINE double
local_time_as_sec(local_time_t t) { return bigtime_as_sec(t.t); }

FORCE_INLINE bool
local_time_is_illegal(local_time_t t) { return bigtime_is_illegal(t.t); }

FORCE_INLINE bool
local_time_eq(local_time_t a, local_time_t b)
{
        ASSERT(a.clock == b.clock);
        return bigtime_eq(a.t, b.t);
}

FORCE_INLINE bool
local_time_lt(local_time_t a, local_time_t b)
{
        ASSERT(a.clock == b.clock);
        return bigtime_lt(a.t, b.t);
}

FORCE_INLINE bool
local_time_gt(local_time_t a, local_time_t b)
{
        ASSERT(a.clock == b.clock);
        return bigtime_gt(a.t, b.t);
}

FORCE_INLINE bool
local_time_le(local_time_t a, local_time_t b)
{
        ASSERT(a.clock == b.clock);
        return bigtime_le(a.t, b.t);
}

FORCE_INLINE bool
local_time_ge(local_time_t a, local_time_t b)
{
        ASSERT(a.clock == b.clock);
        return bigtime_ge(a.t, b.t);
}

FORCE_INLINE bigtime_t
local_time_as_bigtime(local_time_t t) { return t.t; }

FORCE_INLINE local_time_t
local_time_add(local_time_t t, duration_t delta)
{
        return local_time_from_bigtime(t.clock, 
                                       bigtime_add(t.t, delta));
}

FORCE_INLINE local_time_t
local_time_sub(local_time_t t, duration_t delta)
{
        return local_time_from_bigtime(t.clock,
                                       bigtime_sub(t.t, delta));
}

FORCE_INLINE duration_t
local_time_diff(local_time_t t1, local_time_t t2)
{
        ASSERT(t1.clock == t2.clock);
        return bigtime_diff(t1.t, t2.t);
}

#define LOCAL_TIME_ATTRTYPE "[o" BIGTIME_ATTRTYPE "]"

EXPORTED attr_value_t local_time_to_attr(local_time_t t);
EXPORTED local_time_t local_time_from_attr(attr_value_t val);

#define LOCAL_TIME_STR_MAX_SIZE BIGTIME_STR_MAX_SIZE
EXPORTED void local_time_as_string(local_time_t v, char *str);

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
