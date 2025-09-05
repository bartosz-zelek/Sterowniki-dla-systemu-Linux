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

#ifndef SIMICS_UTIL_ARITH_H
#define SIMICS_UTIL_ARITH_H

#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

#if defined __GNUC__ && defined __x86_64__
FORCE_INLINE void
unsigned_divide_128_by_64_nocheck(uint64 dividend_high, uint64 dividend_low,
                                  uint64 divisor,
                                  uint64 *quotient, uint64 *remainder)
{
        uint64 c, r;
        __asm__("divq %4"
                : "=a"(c), "=d"(r)
                : "a"(dividend_low), "d"(dividend_high), "g"(divisor));
        *quotient = c;
        *remainder = r;
}
#endif

#if defined __GNUC__ && defined __x86_64__ && !defined __INTEL_COMPILER
 #define HAVE_NATIVE_INT128 1
#else
 #define HAVE_NATIVE_INT128 0
#endif

#if HAVE_NATIVE_INT128

FORCE_INLINE void
unsigned_multiply_64_by_64(uint64 a, uint64 b, uint64 *prod_h, uint64 *prod_l)
{
        __uint128_t prod = (__uint128_t)a * (__uint128_t)b;
        *prod_h = (uint64)(prod >> 64);
        *prod_l = (uint64)prod;
}

FORCE_INLINE void
signed_multiply_64_by_64(uint64 a, uint64 b,
                         uint64 *prod_h, uint64 *prod_l)
{
        __int128_t prod = (__int128_t)(int64)a * (__int128_t)(int64)b;
        *prod_h = (uint64)(prod >> 64);
        *prod_l = (uint64)prod;
}

FORCE_INLINE uint64
unsigned_multiply_64_by_64_high(uint64 a, uint64 b)
{ return (uint64)(((__uint128_t)a * (__uint128_t)b) >> 64); }

FORCE_INLINE uint64
signed_multiply_64_by_64_high(uint64 a, uint64 b)
{ return (uint64)(((__int128_t)(int64)a * (__int128_t)(int64)b) >> 64); }

#else  /* not HAVE_NATIVE_INT128 */

void unsigned_divide_128_by_64_nocheck(
        uint64 dividend_high, uint64 dividend_low, uint64 divisor,
        uint64 *quotient, uint64 *remainder);
void unsigned_multiply_64_by_64(uint64 a, uint64 b,
                                uint64 *prod_h, uint64 *prod_l);
void signed_multiply_64_by_64(uint64 a, uint64 b,
                              uint64 *prod_h, uint64 *prod_l);
uint64 unsigned_multiply_64_by_64_high(uint64 a, uint64 b);
uint64 signed_multiply_64_by_64_high(uint64 a, uint64 b);

#endif /* not HAVE_NATIVE_INT128 */

int signed_divide_128_by_64(int64 dividend_high, int64 dividend_low,
                            int64 divisor, int64 *quotient, int64 *remainder);
uint64 unsigned_gcd(uint64 n, uint64 d);
int unsigned_multiply_rational(uint64 num1, uint64 den1,
                               uint64 num2, uint64 den2,
                               uint64 *result_num, uint64 *result_den);

FORCE_INLINE int
unsigned_divide_128_by_64(uint64 dividend_high, uint64 dividend_low,
                          uint64 divisor,
                          uint64 *quotient, uint64 *remainder)
{
        if (divisor == 0 || dividend_high >= divisor)
                return 0;                    /* divide by 0 or overflow */
        unsigned_divide_128_by_64_nocheck(dividend_high, dividend_low,
                                          divisor, quotient, remainder);
        return 1;
}

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
