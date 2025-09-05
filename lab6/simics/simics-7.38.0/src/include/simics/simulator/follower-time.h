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

#ifndef SIMICS_SIMULATOR_FOLLOWER_TIME_H
#define SIMICS_SIMULATOR_FOLLOWER_TIME_H

#include <simics/base-types.h>
#include <simics/util/int128.h>
#include <simics/util/help-macros.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        int128 val;                          /* time in ps */
} follower_time_t;

#ifndef PYWRAP

#define FOLLOWER_TIME_ZERO { INT128_ZERO }

FORCE_INLINE double
follower_time_as_sec(follower_time_t t)
{ return int128_to_double(t.val) / 1e12; }

FORCE_INLINE follower_time_t
follower_time_from_sec(double t)
{
        double ps = t * 1e12;
        follower_time_t bt;
        /* add 0.5 to change from rounding to zero to rounding to nearest
           integer */
        ASSERT(int128_from_double(&bt.val, ps + ((ps >= 0) ? 0.5 : -0.5)));
        return bt;
}

FORCE_INLINE follower_time_t
follower_time_from_ps(int64 ps_hi, uint64 ps_lo)
{
        follower_time_t t;
        t.val = int128_from_hi_lo(ps_hi, ps_lo);
        return t;
}

FORCE_INLINE int64
follower_time_as_ps_hi(follower_time_t t) { return int128_hi(t.val); }

FORCE_INLINE uint64
follower_time_as_ps_lo(follower_time_t t) { return int128_lo(t.val); }

FORCE_INLINE follower_time_t
follower_time_from_ps_int128(int128 ps)
{ follower_time_t t; t.val = ps; return t; }

FORCE_INLINE int128
follower_time_as_ps_int128(follower_time_t t) { return t.val; }

FORCE_INLINE bool
follower_time_eq(follower_time_t a, follower_time_t b)
{ return int128_eq(a.val, b.val); }

FORCE_INLINE bool
follower_time_lt(follower_time_t a, follower_time_t b)
{ return int128_lt(a.val, b.val); }

FORCE_INLINE bool
follower_time_gt(follower_time_t a, follower_time_t b)
{ return int128_gt(a.val, b.val); }

FORCE_INLINE bool
follower_time_le(follower_time_t a, follower_time_t b)
{ return int128_le(a.val, b.val); }

FORCE_INLINE bool
follower_time_ge(follower_time_t a, follower_time_t b)
{ return int128_ge(a.val, b.val); }

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
