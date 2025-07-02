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

#ifndef SIMICS_BASE_BIGTIME_H
#define SIMICS_BASE_BIGTIME_H

#include <simics/base-types.h>
#include <simics/util/int128.h>
#include <simics/util/help-macros.h>

#include <simics/base/duration.h>
#include <simics/base/attr-value.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* <add id="bigtime_t DOC">
   <ndx>bigtime_t</ndx>
   <doc>
   <doc-item name="NAME">bigtime_t</doc-item>
   <doc-item name="SYNOPSIS">
     <insert id="bigtime_t def"/>
   </doc-item>
   <doc-item name="DESCRIPTION">
   	 A bigtime_t represents time in picoseconds, or BIGTIME_ILLEGAL.

   </doc-item>
   </doc></add>
*/

/* <add-type id="bigtime_t def"></add-type> */
typedef struct {
        int128 val;
} bigtime_t;

#ifndef PYWRAP

#define BIGTIME_ZERO { INT128_ZERO }
#define BIGTIME_ILLEGAL { INT128_MIN }

FORCE_INLINE bool
bigtime_is_illegal(bigtime_t t) 
{
        int128 illegal = INT128_MIN;
        return int128_eq(t.val, illegal);
}

/* A bigtime_t as a value in seconds. */
FORCE_INLINE double
bigtime_as_sec(bigtime_t t)
{
        ASSERT(!bigtime_is_illegal(t));
        return int128_to_double(t.val) / 1e12;
}

/* The bigtime_t equivalent to a value in seconds, or BIGTIME_ILLEGAL
   if not representable. */
FORCE_INLINE bigtime_t
bigtime_from_sec(double t)
{
        double ps = t * 1e12;
        bigtime_t bt;
        /* add 0.5 to change from rounding to zero to rounding to nearest
           integer */
        if (int128_from_double(&bt.val, ps + ((ps >= 0) ? 0.5 : -0.5)))
                return bt;
        else {
                /* conversion failed */
                bigtime_t ret = BIGTIME_ILLEGAL;
                return ret;
        }
}

/* The next six functions purposedly do not check if the bigtime_t created or
   passed as argument is illegal, so even illegal bigtimes can be
   manipulated. */

/* Make a bigtime_t from a signed 128-bit value. */
FORCE_INLINE bigtime_t
bigtime_from_ps_int128(int128 ps) { bigtime_t t; t.val = ps; return t; }

/* Make a bigtime_t from a signed 128-bit value. */
FORCE_INLINE bigtime_t
bigtime_from_ps(int64 ps_hi, uint64 ps_lo)
{ return bigtime_from_ps_int128(int128_from_hi_lo(ps_hi, ps_lo)); }

/* Make a bigtime_t from a signed 64-bit value.
   (The name of this function is somewhat misleading.) */
FORCE_INLINE bigtime_t
bigtime_from_ps_lo(int64 ps_lo)
{ return bigtime_from_ps_int128(int128_from_int64(ps_lo)); }

/* The high 64 bits of a bigtime_t (t / 2**64, rounded towards zero). */
FORCE_INLINE int64
bigtime_as_ps_hi(bigtime_t t) { return int128_hi(t.val); }

/* The low 64 bits of a bigtime_t (t mod 2**64). */
FORCE_INLINE uint64
bigtime_as_ps_lo(bigtime_t t) { return int128_lo(t.val); }

/* A bigtime_t as a signed 128-bit value. */
FORCE_INLINE int128
bigtime_as_ps_int128(bigtime_t t) { return t.val; }


/* a = b */
FORCE_INLINE bool
bigtime_eq(bigtime_t a, bigtime_t b) 
{ 
        return     !bigtime_is_illegal(a) 
                && !bigtime_is_illegal(b)
                && int128_eq(a.val, b.val);
}

/* a < b */
FORCE_INLINE bool
bigtime_lt(bigtime_t a, bigtime_t b) 
{
        return     !bigtime_is_illegal(a) 
                && !bigtime_is_illegal(b)
                && int128_lt(a.val, b.val);
}

/* a > b */
FORCE_INLINE bool
bigtime_gt(bigtime_t a, bigtime_t b) 
{ 
        return     !bigtime_is_illegal(a) 
                && !bigtime_is_illegal(b)
                && int128_gt(a.val, b.val);
}

/* a ≤ b */
FORCE_INLINE bool
bigtime_le(bigtime_t a, bigtime_t b) 
{
        return     !bigtime_is_illegal(a) 
                && !bigtime_is_illegal(b)
                && int128_le(a.val, b.val);
}

/* a ≥ b */
FORCE_INLINE bool
bigtime_ge(bigtime_t a, bigtime_t b) 
{
        return     !bigtime_is_illegal(a) 
                && !bigtime_is_illegal(b)
                && int128_ge(a.val, b.val);
}


/* a + b if representable, or BIGTIME_ILLEGAL otherwise. */
FORCE_INLINE bigtime_t
bigtime_add_bigtime(bigtime_t a, bigtime_t b) 
{
        if (bigtime_is_illegal(a) || bigtime_is_illegal(b)) {
                bigtime_t r = BIGTIME_ILLEGAL; 
                return r;
        } else {
                return bigtime_from_ps_int128(int128_add(a.val, b.val));
        }
}

/* a - b if representable, or BIGTIME_ILLEGAL otherwise. */
FORCE_INLINE bigtime_t
bigtime_sub_bigtime(bigtime_t a, bigtime_t b)
{
        if (bigtime_is_illegal(a) || bigtime_is_illegal(b)) {
                bigtime_t r = BIGTIME_ILLEGAL; 
                return r;
        } else {
                return bigtime_from_ps_int128(int128_sub(a.val, b.val));
        }
}

/* t + delta if representable, or BIGTIME_ILLEGAL otherwise. */
FORCE_INLINE bigtime_t
bigtime_add(bigtime_t t, duration_t delta)
{
        if (bigtime_is_illegal(t) || duration_is_illegal(delta)) {
                bigtime_t r = BIGTIME_ILLEGAL; 
                return r;
        } else {
                return bigtime_from_ps_int128(
                        int128_add_int64(t.val, duration_as_ps(delta)));
        }
}

/* t - delta if representable, or BIGTIME_ILLEGAL otherwise. */
FORCE_INLINE bigtime_t
bigtime_sub(bigtime_t t, duration_t delta)
{
        if (bigtime_is_illegal(t) || duration_is_illegal(delta)) {
                bigtime_t r = BIGTIME_ILLEGAL; 
                return r;
        } else {
                return bigtime_from_ps_int128(
                        int128_sub_int64(t.val, duration_as_ps(delta)));
        }
}

/* t1 - t2; must be representable as a duration_t. */
FORCE_INLINE duration_t
bigtime_diff(bigtime_t t1, bigtime_t t2)
{
        bigtime_t t = bigtime_sub_bigtime(t1, t2);
        duration_t d;
        int64 r = 0;
        ASSERT(int128_to_int64(&r, t.val));
        d = duration_from_ps(r);
        ASSERT(!duration_is_illegal(d));
        return d;
}

EXPORTED bigtime_t bigtime_mul(bigtime_t t, int64 factor);

#ifndef PYWRAP
EXPORTED bigtime_t bigtime_div_raw(bigtime_t t, int64 divisor, bool *exact);
EXPORTED bigtime_t bigtime_div_floor(bigtime_t t, int64 divisor);
#endif

#define BIGTIME_ATTRTYPE "[ii]|n"

EXPORTED attr_value_t bigtime_to_attr(bigtime_t t);
EXPORTED bigtime_t bigtime_from_attr(attr_value_t a);

/* 0x0123456789abcdef0123456789abcdef (<floating-value> s)
   32 is 17 digits (floating-value) plus slack, and 5 is length of text*/
#define BIGTIME_STR_MAX_SIZE (INT128_STR_MAX_SIZE + 32 + 5)
EXPORTED char *bigtime_as_string(bigtime_t t, char *buf);

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
