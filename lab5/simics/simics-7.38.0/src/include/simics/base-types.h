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

/* underscores in header guard to distinguish from <simics/base/types.h> */
#ifndef _SIMICS_BASE_TYPES_H
#define _SIMICS_BASE_TYPES_H

#include <simics/module-host-config.h>

#if !defined(TURBO_SIMICS)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __STDC_CONSTANT_MACROS
#include <stdint.h>
#include <sys/types.h>
#include <limits.h>

#endif /* !TURBO_SIMICS */

#include <simics/host-info.h>

#ifdef HAVE_STDBOOL_H
 #include <stdbool.h>
#else
 #if !defined __cplusplus && !defined __bool_true_false_are_defined
  #ifdef _MSC_VER
   #define bool char
  #else
   #define bool _Bool
  #endif
  #define true 1
  #define false 0
  #define __bool_true_false_are_defined 1
 #endif
#endif

/* C23 has a bool keyword
   GCC 14.1 still returns an old value of __SDC_VERSION__. */
#if !defined(__cplusplus) && __STDC_VERSION__ < 202000 && !defined __bool_true_false_are_defined
#define bool _Bool
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#if defined __cplusplus && !defined _Bool && !defined _MSC_VER
/* for compatibility with the C type used in headers. MSVC already uses
   a typedef to define _Bool as a synonym for bool. */
 #define _Bool bool
#endif

#if !defined(TURBO_SIMICS)
#ifdef _WIN32
 /* Really SOCKET, but we don't want to include the heavy Windows headers
    here. */
 typedef uintptr_t socket_t;
#else
 typedef int socket_t;
#endif /* !_WIN32 */

#endif /* !TURBO_SIMICS */

#ifndef HAVE_INT8
 typedef signed char int8;
 #define HAVE_INT8 1
#endif
#ifndef HAVE_UINT8
 typedef unsigned char uint8;
 #define HAVE_UINT8 1
#endif

#ifndef HAVE_INT16
 typedef short int16;
 #define HAVE_INT16 1
#endif
#ifndef HAVE_UINT16
 typedef unsigned short uint16;
 #define HAVE_UINT16 1
#endif

#ifndef HAVE_INT32
 typedef int int32;
 #define HAVE_INT32 1
#endif
#ifndef HAVE_UINT32
 typedef unsigned int uint32;
 #define HAVE_UINT32 1
#endif

#if defined(_WIN32) && !defined(TURBO_SIMICS)

 #ifndef HAVE_UINT64
  typedef unsigned __int64 uint64;
  #define HAVE_UINT64 1
 #endif
 #ifndef HAVE_INT64
  typedef __int64 int64;
  #define HAVE_INT64 1
 #endif

#else

 #ifndef HAVE_UINT64
  typedef unsigned long long uint64;
  #define HAVE_UINT64 1
 #endif
 #ifndef HAVE_INT64
  typedef long long int64;
  #define HAVE_INT64 1
 #endif

#endif

#if !defined(HAVE_INTPTR_T) || defined(TURBO_SIMICS)
typedef int64 intptr_t;
#endif

#if !defined(HAVE_UINTPTR_T) || defined(TURBO_SIMICS)
typedef uint64 uintptr_t;
#endif

#if defined(_MSC_VER) && !defined(TURBO_SIMICS)
typedef __int64 ssize_t;
#endif

#if defined(GULP_API_HELP) || defined(GULP_PYTHON)
 typedef struct _object PyObject;
 typedef struct _typeobject PyTypeObject;
 typedef PyObject SIM_PYOBJECT;
 typedef PyTypeObject SIM_PYTYPEOBJECT;
#elif !defined(TURBO_SIMICS)
 typedef struct _object SIM_PYOBJECT;
 typedef struct _typeobject SIM_PYTYPEOBJECT;
#endif

#if defined(__cplusplus)
}
#endif

#endif
