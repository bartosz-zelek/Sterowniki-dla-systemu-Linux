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

#ifndef SIMICS_UTIL_INT128_H
#define SIMICS_UTIL_INT128_H

#include <simics/base-types.h>
#include <simics/util/arith.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
        uint64 lo;
        int64  hi;
} int128;

#ifndef PYWRAP

#define INT128_ZERO { 0 }
#define INT128_MAX { (uint64)-1, 0x7fffffffffffffffULL }
#define INT128_MIN { 0, (int64)0x8000000000000000ULL }

FORCE_INLINE int128
int128_from_hi_lo(int64 hi, uint64 lo)
{
        int128 r;
        r.lo = lo;
        r.hi = hi;
        return r;
}

FORCE_INLINE int64
int128_hi(int128 v) { return v.hi; }

FORCE_INLINE uint64
int128_lo(int128 v) { return v.lo; }

FORCE_INLINE int128
int128_neg(int128 t)
{
#if defined _MSC_VER
// C4146: unary minus operator applied to unsigned type, result still unsigned
#pragma warning(suppress: 4146)
        uint64 lo = -t.lo;
#else
        uint64 lo = -t.lo;
#endif
        return int128_from_hi_lo((int64)(~(uint64)t.hi + ((lo == 0) ? 1 : 0)),
                                 lo);
}

bool int128_from_double(int128 *dst, double src);
double int128_to_double(int128 src);

FORCE_INLINE int128
int128_from_int64(int64 v)
{
        return int128_from_hi_lo(v >> 63, (uint64)v);
}

FORCE_INLINE bool
int128_to_int64(int64 *dst, int128 src)
{
        uint64 lo_high_bit = src.lo >> 63;

        if ((uint64)src.hi + lo_high_bit == 0) {
                /* implies either that both are 0 (valid positive int64 case)
                   or that src.hi is -1 and log_high_bit 1 (valid negative
                   int64 case) */
                *dst = (int64)src.lo;
                return true;
        } else {
                return false;
        }
}

FORCE_INLINE bool
int128_eq(int128 a, int128 b)
{
        return a.hi == b.hi && a.lo == b.lo;
}

FORCE_INLINE bool
int128_lt(int128 a, int128 b)
{
        return a.hi < b.hi || (a.hi == b.hi && a.lo < b.lo);
}

FORCE_INLINE bool
int128_le(int128 a, int128 b)
{
        return !int128_lt(b, a);
}

FORCE_INLINE bool
int128_gt(int128 a, int128 b)
{
        return int128_lt(b, a);
}

FORCE_INLINE bool
int128_ge(int128 a, int128 b)
{
        return !int128_lt(a, b);
}

FORCE_INLINE int128
int128_add(int128 a, int128 b)
{
        uint64 lo = a.lo + b.lo;
        return int128_from_hi_lo((int64)((uint64)a.hi + (uint64)b.hi
                                         + ((lo < a.lo) ? 1 : 0)),
                                 lo);
}

FORCE_INLINE int128
int128_sub(int128 a, int128 b)
{
        uint64 lo = a.lo - b.lo;
        return int128_from_hi_lo((int64)((uint64)a.hi - (uint64)b.hi
                                         - ((b.lo > a.lo) ? 1 : 0)),
                                 lo);
}

FORCE_INLINE int128
int128_add_int64(int128 a, int64 b)
{
        return int128_add(a, int128_from_int64(b));
}

FORCE_INLINE int128
int128_sub_int64(int128 a, int64 b) 
{
        return int128_sub(a, int128_from_int64(b));
}

#if HAVE_NATIVE_INT128
FORCE_INLINE int128
int128_mul_int64(int128 a, int64 b)
{
        __int128_t aa = (__uint128_t)a.hi << 64 | a.lo;
        __int128_t prod = aa * b;
        return int128_from_hi_lo((int64)(prod >> 64), (uint64)prod);
}

#else
int128 int128_mul_int64(int128 a, int64 b);
#endif

bool int128_div_int64(int128 a, int64 b, int128 *q, int64 *r);

#define INT128_STR_MAX_SIZE (32 + 2 + 1)
char *int128_as_string(int128 v, char *buf);

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
