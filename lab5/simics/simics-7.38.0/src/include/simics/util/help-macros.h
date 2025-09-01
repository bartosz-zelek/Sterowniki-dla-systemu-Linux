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

#ifndef SIMICS_UTIL_HELP_MACROS_H
#define SIMICS_UTIL_HELP_MACROS_H

#include <simics/base-types.h>
#include <simics/util/strbuf.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined __GNUC__
 #define COLD_FUNCTION __attribute__((cold))
#else
 #define COLD_FUNCTION
#endif

#ifndef PYWRAP

NORETURN COLD_FUNCTION void assert_error(int line, const char *file,
                                         const char *mod_date,
                                         const char *message);

#define ASSERT(exp) ASSERT_MSG(exp, NULL)
 
#if defined(HAVE_MODULE_DATE)
 extern const char _module_date[];
 #define ASSERT_MODULE_DATE _module_date
#else
 #define ASSERT_MODULE_DATE NULL
#endif

/* The condition are placed directly inside the if statement without brackets
   in order to enable GCC's warnings about assignments in conditions. */
#define ASSERT_MSG(cond, msg)                                              \
do {                                                                       \
        if (cond)                                                          \
                ;                                                          \
        else                                                               \
                assert_error(__LINE__, __FILE__, ASSERT_MODULE_DATE, msg); \
} while (0)

#ifdef HAVE_VARARG_MACROS
#define ASSERT_FMT(cond, fmt, ...)                                      \
do {                                                                    \
        if (cond)                                                       \
                ;                                                       \
        else {                                                          \
                strbuf_t sb = sb_newf(fmt, __VA_ARGS__);                \
                assert_error(__LINE__, __FILE__, ASSERT_MODULE_DATE,    \
                             sb_str(&sb));                              \
        }                                                               \
} while (0)
#endif  /* HAVE_VARARG_MACROS */

#define ASSERT_VFMT(cond, fmt_args)                                     \
do {                                                                    \
        if (cond)                                                       \
                ;                                                       \
        else {                                                          \
                strbuf_t sb = sb_newf fmt_args;                         \
                assert_error(__LINE__, __FILE__, ASSERT_MODULE_DATE,    \
                             sb_str(&sb));                              \
        }                                                               \
} while (0)

#define ABORT_MSG(msg) ASSERT_MSG(0, msg)
#ifdef HAVE_VARARG_MACROS
 #define ABORT_FMT(fmt, ...) ASSERT_FMT(0, fmt, __VA_ARGS__)
#endif

#define ARG_NOT_NULL(arg)                               \
do {                                                    \
         if (unlikely((arg) == NULL))                   \
                 NULL_ARG_ERROR(__func__, #arg);        \
} while (0)

#define FATAL_ERROR_IF(cond, fmt, ...)                  \
do {                                                    \
         if (cond)                                      \
                 FATAL_ERROR(fmt, ## __VA_ARGS__);      \
} while (0)

NORETURN void null_arg_error(const char *func, const char *arg);
NORETURN void fatal_error(const char *fmt, ...) PRINTF_FORMAT(1, 2);

#define NULL_ARG_ERROR(f, a) null_arg_error(f, a)
#define FATAL_ERROR fatal_error

/* Static (compile-time) assertion that can be used anywhere a declaration
   can occur, both at top level and in functions. */
#if defined(__GNUC__) && !defined(__cplusplus) && __GNUC__ >= 6
#define STATIC_ASSERT(cond) _Static_assert(cond, "")
#else
#define STATIC_ASSERT(cond) \
    UNUSED extern int simics_STATIC_ASSERT[(cond) ? 1 : -1]
#endif

#undef MIN
#undef MAX
#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#define ALEN(a) ((int)(sizeof (a) / sizeof (a)[0]))

/* create a string from the definition of the CPP symbol n */ 
#define SYMBOL_TO_STRING2(n) #n
#define SYMBOL_TO_STRING(n) SYMBOL_TO_STRING2(n)

#endif /* PYWRAP */

#ifdef __cplusplus
}
#endif

#endif
