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

#ifndef SIMICS_BASE_DURATION_H
#define SIMICS_BASE_DURATION_H

#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct { int64 t; } duration_t;

#ifndef PYWRAP

#define DURATION_SCALE 1000000000000ull

#define ILLEGAL_DURATION__VAL ((int64)0x8000000000000000ull)
#define ILLEGAL_DURATION ((duration_t){ ILLEGAL_DURATION__VAL })
#define MAX_POSITIVE_DURATION_VAL_SEC \
                ((double)((1ull << 63) - 1) / DURATION_SCALE)
#define MIN_POSITIVE_DURATION_VAL_SEC ((double)1 / DURATION_SCALE)

#define ZERO_DURATION ((duration_t){ 0 })
#define EPSILON_DURATION ((duration_t){ 1 })

FORCE_INLINE duration_t
duration_from_ps(int64 ps) { duration_t d; d.t = ps; return d; }

FORCE_INLINE duration_t
duration_from_sec(double dt)
{ return duration_from_ps((int64)(dt * DURATION_SCALE)); }

FORCE_INLINE double
duration_as_sec(duration_t dt)
{ return (double)dt.t / DURATION_SCALE; }

FORCE_INLINE int64
duration_as_ps(duration_t dt) { return dt.t; }

FORCE_INLINE bool
duration_is_illegal(duration_t dt)
{ return dt.t == ILLEGAL_DURATION__VAL; }

FORCE_INLINE bool
duration_eq(duration_t a, duration_t b)
{ return a.t == b.t; }

FORCE_INLINE bool
duration_lt(duration_t a, duration_t b)
{ return a.t < b.t; }

FORCE_INLINE bool
duration_gt(duration_t a, duration_t b)
{ return a.t > b.t; }

FORCE_INLINE duration_t
duration_half(duration_t dt)
{ return duration_from_ps(duration_as_ps(dt) / 2); }

FORCE_INLINE duration_t
duration_double(duration_t dt)
{ return duration_from_ps(duration_as_ps(dt) * 2); }

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
