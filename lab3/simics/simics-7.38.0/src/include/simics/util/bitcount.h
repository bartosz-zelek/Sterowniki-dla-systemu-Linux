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

#ifndef SIMICS_UTIL_BITCOUNT_H
#define SIMICS_UTIL_BITCOUNT_H

#include <simics/base-types.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* This file defines the following functions and macros:

   is_power_of_2(x)

       True if x is a power of 2, false otherwise.

   count_leading_zeros{32,64}(x)
   count_trailing_zeros{32,64}(x)
       
       Count the number of leading or trailing zeros in a 32-bit (or 64-bit)
       word. If the argument is zero, return 32 (or 64).
   
   count_leading_zeros{32,64}_nonzero(x)
   count_trailing_zeros{32,64}_nonzero(x)

       Potentially faster and smaller versions of the above functions that can
       only be used when x is known not to be zero.

   log2_32(x)
   log2_64(x)

       floor(log2(x)); return -1 for x=0. LOG2 is for 32-bit arguments;
       LOG2_64 for 64-bit arguments.

   IS_POWER_OF_2(x)
   COUNT_LEADING_ZEROS{32,64}(x)
   COUNT_TRAILING_ZEROS{32,64}(x)
   COUNT_LEADING_ZEROS{32,64}_NONZERO(x)
   COUNT_TRAILING_ZEROS{32,64}_NONZERO(x)
   LOG2(x)
   LOG2_64(x)

       Macro versions of the above, written to give a constant result when
       the argument is a constant expression.
*/

#undef CONSTANT_P
#if defined(__GNUC__)
 #define CONSTANT_P(x) (__extension__ (__builtin_constant_p(x)))
#else
 #define CONSTANT_P(x) 0
#endif

FORCE_INLINE bool
is_power_of_2(uint64 x)
{
        return x != 0 && (x & (x - 1)) == 0;
}
#undef IS_POWER_OF_2
#define IS_POWER_OF_2(x) (                                              \
                CONSTANT_P((x))                                         \
                ? (x) != 0 && ((uint64)(x) & (((uint64)(x) - 1))) == 0  \
                : is_power_of_2((uint64)(x)))

#if defined __GNUC__

FORCE_INLINE unsigned
bit_count32(uint32 x)
{
        return (unsigned)__builtin_popcount(x);
}

FORCE_INLINE unsigned
bit_count64(uint64 x)
{
        return (unsigned)__builtin_popcountll(x);
}

#else /* !__GNUC__ */

FORCE_INLINE unsigned
bit_count32(uint32 x)
{
        unsigned ret = 0;
        for (int i = 0; i < 32; i += 4) {
                unsigned nibble = (x >> i) & 0xf;
                ret += (0x4332322132212110ULL >> (nibble * 4)) & 0xf;
        }
        return ret;
}

FORCE_INLINE unsigned
bit_count64(uint64 x)
{
        unsigned ret = 0;
        for (int i = 0; i < 64; i += 4) {
                unsigned nibble = (x >> i) & 0xf;
                ret += (0x4332322132212110ULL >> (nibble * 4)) & 0xf;
        }
        return ret;
}

#endif /* __GNUC__ */

/* According to Intel's documentation, the bsr/bsf output register become
   undefined if the input is 0. However, AMD's docs say that it remains
   unchanged in that case, and in practice, all models newer than a 486 work
   that way. The Linux kernel assumes this to be true and one of the Intel CPU
   architects has acknowledged that they are unlikely to change this behaviour.

   Reference: https://lkml.org/lkml/2011/12/15/444 */

#if defined __x86_64__ && defined __GNUC__

FORCE_INLINE unsigned
count_leading_zeros32(uint32 x)
{
        unsigned n;
        __asm__ ("bsrl %1, %0"
                 : "=r" (n)
                 : "rm" (x), "0" (63)
                 : "cc");
        return n ^ 31;
}

FORCE_INLINE unsigned
count_leading_zeros64(uint64 x)
{
        uint64 n;
        __asm__ ("bsrq %1, %0"
                 : "=r" (n)
                 : "rm" (x), "0" (127)
                 : "cc");
        return (unsigned)(n ^ 63);
}

FORCE_INLINE unsigned
count_trailing_zeros32(uint32 x)
{
        unsigned n;
        __asm__ ("bsfl %1, %0"
                 : "=r" (n)
                 : "rm" (x), "0" (32)
                 : "cc");
        return n;

}

FORCE_INLINE unsigned
count_trailing_zeros64(uint64 x)
{
        uint64 n;
        __asm__ ("bsfq %1, %0"
                 : "=r" (n)
                 : "rm" (x), "0" (64)
                 : "cc");
        return (unsigned)n;

}

FORCE_INLINE unsigned
count_leading_zeros32_nonzero(uint32 x)
{
        unsigned n;
        __asm__ ("bsrl %1, %0" : "=r" (n) : "rm" (x) : "cc");
        return n ^ 31;
}

FORCE_INLINE unsigned
count_trailing_zeros32_nonzero(uint32 x)
{
        unsigned n;
        __asm__ ("bsfl %1, %0" : "=r" (n) : "rm" (x) : "cc");
        return n;
}

FORCE_INLINE unsigned
count_leading_zeros64_nonzero(uint64 x)
{
        uint64 n;
        __asm__ ("bsrq %1, %0" : "=r" (n) : "rm" (x) : "cc");
        return (unsigned)(n ^ 63);
}

FORCE_INLINE unsigned
count_trailing_zeros64_nonzero(uint64 x)
{
        uint64 n;
        __asm__ ("bsfq %1, %0" : "=r" (n) : "rm" (x) : "cc");
        return (unsigned)n;
}

#else

/* Generic code for hosts without bitcount instructions */

FORCE_INLINE unsigned
count_leading_zeros32(uint32 x)
{
        unsigned n, y;
        n = 32;
        y = x >> 16; if (y) { n -= 16; x = y; }
        y = x >>  8; if (y) { n -=  8; x = y; }
        y = x >>  4; if (y) { n -=  4; x = y; }
        y = x >>  2; if (y) { n -=  2; x = y; }
        y = x >>  1; if (y) return n - 2;
        return n - x;
}

FORCE_INLINE unsigned
count_leading_zeros64(uint64 x)
{
        unsigned n, y;
        n = 64;
        y = x >> 32; if (y) { n -= 32; x = y; }
        y = x >> 16; if (y) { n -= 16; x = y; }
        y = x >>  8; if (y) { n -=  8; x = y; }
        y = x >>  4; if (y) { n -=  4; x = y; }
        y = x >>  2; if (y) { n -=  2; x = y; }
        y = x >>  1; if (y) return n - 2;
        return n - x;
}

FORCE_INLINE unsigned
count_trailing_zeros32(uint32 x)
{
        unsigned n;
        if (x == 0)
                return 32;
        n = 1;
        if (!(x & 0x0000ffff)) { n += 16; x >>= 16; }
        if (!(x & 0x000000ff)) { n +=  8; x >>=  8; }
        if (!(x & 0x0000000f)) { n +=  4; x >>=  4; }
        if (!(x & 0x00000003)) { n +=  2; x >>=  2; }
        return n - (x & 1);
}

FORCE_INLINE unsigned
count_trailing_zeros64(uint64 x)
{
        unsigned n;
        if (x == 0)
                return 64;
        n = 1;
        if (!(x & 0x00000000ffffffff)) { n += 32; x >>= 32; }
        if (!(x & 0x000000000000ffff)) { n += 16; x >>= 16; }
        if (!(x & 0x00000000000000ff)) { n +=  8; x >>=  8; }
        if (!(x & 0x000000000000000f)) { n +=  4; x >>=  4; }
        if (!(x & 0x0000000000000003)) { n +=  2; x >>=  2; }
        return n - (x & 1);
}

FORCE_INLINE unsigned
count_leading_zeros32_nonzero(uint32 x)
{ return count_leading_zeros32(x); }

FORCE_INLINE unsigned
count_trailing_zeros32_nonzero(uint32 x)
{ return count_trailing_zeros32(x); }

FORCE_INLINE unsigned
count_leading_zeros64_nonzero(uint64 x)
{ return count_leading_zeros64(x); }

FORCE_INLINE unsigned
count_trailing_zeros64_nonzero(uint64 x)
{ return count_trailing_zeros64(x); }

#endif /* generic code */

/* macro for constant expressions */
#define COUNT_LEADING_ZEROS16_CONST(x)          \
    ((x) >= 1u << 8                             \
     ? ((x) >= 1u << 12                         \
        ? ((x) >= 1u << 14                      \
           ? ((x) >= 1u << 15                   \
              ? 0                               \
              : 1)                              \
           : ((x) >= 1u << 13                   \
              ? 2                               \
              : 3))                             \
        : ((x) >= 1u << 10                      \
           ? ((x) >= 1u << 11                   \
              ? 4                               \
              : 5)                              \
           : ((x) >= 1u << 9                    \
              ? 6                               \
              : 7)))                            \
     : ((x) >= 1u << 4                          \
        ? ((x) >= 1u << 6                       \
           ? ((x) >= 1u << 7                    \
              ? 8                               \
              : 9)                              \
           : ((x) >= 1u << 5                    \
              ? 10                              \
              : 11))                            \
        : ((x) >= 1u << 2                       \
           ? ((x) >= 1u << 3                    \
              ? 12                              \
              : 13)                             \
           : ((x) >= 1u << 1                    \
              ? 14                              \
              : ((x) >= 1u << 0                 \
                 ? 15                           \
                 : 16)))))

#define COUNT_LEADING_ZEROS32_CONST(x)                  \
    ((uint32)(x) >= 1u << 16                            \
     ? COUNT_LEADING_ZEROS16_CONST((uint32)(x) >> 16)   \
     : COUNT_LEADING_ZEROS16_CONST((uint16)(x)) + 16u)

#define COUNT_LEADING_ZEROS64_CONST(x)                  \
    ((uint64)(x) >= (1ULL << 32)                        \
     ? COUNT_LEADING_ZEROS32_CONST((uint64)(x) >> 32)   \
     : COUNT_LEADING_ZEROS32_CONST((uint32)(x)) + 32u)

#define COUNT_TRAILING_ZEROS32_CONST(x)                                  \
    (32 - COUNT_LEADING_ZEROS32_CONST(~(uint32)(x) & ((uint32)(x) - 1)))

#define COUNT_TRAILING_ZEROS64_CONST(x)                                 \
    (64 - COUNT_LEADING_ZEROS64_CONST(~(uint64)(x) & ((uint64)(x) - 1)))

/* Inline function wrappers around the constant variants to avoid warnings when
   the argument is not constant but of a small type (uint8, say). */
FORCE_INLINE unsigned
count_leading_zeros32_const(uint32 x)
{ return COUNT_LEADING_ZEROS32_CONST(x); }

FORCE_INLINE unsigned
count_leading_zeros64_const(uint64 x)
{ return COUNT_LEADING_ZEROS64_CONST(x); }

FORCE_INLINE unsigned
count_trailing_zeros32_const(uint32 x)
{ return COUNT_TRAILING_ZEROS32_CONST(x); }

FORCE_INLINE unsigned
count_trailing_zeros64_const(uint64 x)
{ return COUNT_TRAILING_ZEROS64_CONST(x); }

#define COUNT_LEADING_ZEROS32(x)                \
    (CONSTANT_P(x)                              \
     ? COUNT_LEADING_ZEROS32_CONST(x)           \
     : count_leading_zeros32(x))
#define COUNT_LEADING_ZEROS64(x)                \
    (CONSTANT_P(x)                              \
     ? COUNT_LEADING_ZEROS64_CONST(x)           \
     : count_leading_zeros64(x))
#define COUNT_TRAILING_ZEROS32(x)               \
    (CONSTANT_P(x)                              \
     ? count_trailing_zeros32_const(x)          \
     : count_trailing_zeros32(x))
#define COUNT_TRAILING_ZEROS64(x)               \
    (CONSTANT_P(x)                              \
     ? count_trailing_zeros64_const(x)          \
     : count_trailing_zeros64(x))

#define COUNT_LEADING_ZEROS32_NONZERO(x)        \
    (CONSTANT_P(x)                              \
     ? count_leading_zeros32_const(x)           \
     : count_leading_zeros32_nonzero(x))
#define COUNT_LEADING_ZEROS64_NONZERO(x)        \
    (CONSTANT_P(x)                              \
     ? count_leading_zeros64_const(x)           \
     : count_leading_zeros64_nonzero(x))
#define COUNT_TRAILING_ZEROS32_NONZERO(x)       \
    (CONSTANT_P(x)                              \
     ? count_trailing_zeros32_const(x)          \
     : count_trailing_zeros32_nonzero(x))
#define COUNT_TRAILING_ZEROS64_NONZERO(x)       \
    (CONSTANT_P(x)                              \
     ? count_trailing_zeros64_const(x)          \
     : count_trailing_zeros64_nonzero(x))

FORCE_INLINE int
log2_32(uint32 x)
{ return 31 - (int)count_leading_zeros32(x); }

FORCE_INLINE int
log2_64(uint64 x)
{ return 63 - (int)count_leading_zeros64(x); }

#define LOG2(x) (31 - (int)COUNT_LEADING_ZEROS32(x))
#define LOG2_64(x) (63 - (int)COUNT_LEADING_ZEROS64(x))

#if defined(__cplusplus)
}
#endif

#endif
