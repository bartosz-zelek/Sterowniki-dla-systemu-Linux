/*
  Copyright 2015-2022 Intel Corporation

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef KTYPES_H
#define KTYPES_H

#ifdef __KERNEL__
#include <linux/version.h>
#include <linux/compiler.h>
#include <linux/types.h>

/* RHEL_<NUM>_GREATER_EQUAL defines enable conditional compilation for Red Hat
   Enterprise Linux (RHEL) kernels. These kernels fix major, minor, and patch
   numbers, and only rhel_release is incremented. */
#define RHEL_X_GREATER_EQUAL_TEMPLATE(major, minor, patch, rhel_release)      \
        (defined RHEL_RELEASE_AS_NUM                                          \
         && LINUX_VERSION_CODE == KERNEL_VERSION(major, minor, patch)         \
         && RHEL_RELEASE_AS_NUM >= rhel_release)

/* Define for Red Hat Enterprise Linux 9 that is based on 5.14.0 kernel: */
#define RHEL_9_GREATER_EQUAL(rhel_release)              \
        RHEL_X_GREATER_EQUAL_TEMPLATE(5, 14, 0, rhel_release)

#endif  /* __KERNEL__ */

#if !defined(__linux__)
#include <ntdef.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef __GNUC__
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;

#else /* !__GNUC__ */
#define inline __inline
typedef signed __int8   s8;
typedef unsigned __int8  u8;
typedef signed __int16  s16;
typedef unsigned __int16 u16;
typedef signed __int32  s32;
typedef unsigned __int32 u32;
typedef signed __int64  s64;
typedef unsigned __int64 u64;

#endif /* !__GNUC__*/

#ifdef __INTEL_COMPILER
typedef _Bool bool;
#define true    1
#define false   0
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#if defined(__linux__)
typedef int fdesc_t;
#else
typedef HANDLE fdesc_t;
#endif

#ifndef __user
/* Annotations for Sparse static analysis tool */
#ifdef __CHECKER__
# define __acquires(x) __attribute__((context(x,0,1)))
# define __releases(x) __attribute__((context(x,1,0)))
# define __user        __attribute__((noderef, address_space(1)))
#else
# define __acquires(x)
# define __releases(x)
# define __user
#endif /* __CHECKER__ */
#endif // __user

#if defined(__cplusplus)
}
#endif

#endif   /* KTYPES_H */
