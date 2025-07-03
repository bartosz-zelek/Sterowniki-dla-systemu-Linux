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

#ifndef SIMICS_HOST_INFO_H
#define SIMICS_HOST_INFO_H

#if !(defined PYWRAP || defined GULP_API_HELP || defined GULP_EXTERN_DECL)
 #define NOTNULL
#endif

/* Use restrict on C99 compilers and GNU C++ */
#if defined(__cplusplus)
 #if defined(__GNUC__)
  #define restrict __restrict__
 #else
  #define restrict
 #endif
#else
 #if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L)
  #define restrict
 #endif
#endif

#ifndef GULP_EXTERN_DECL
 #ifdef __GNUC__
  #define NORETURN __attribute__ ((noreturn))
 #elif defined _MSC_VER
  #define NORETURN __declspec(noreturn)
 #else
  #define NORETURN
 #endif
#endif

#ifdef __GNUC__
 #ifndef UNUSED
  #define UNUSED __attribute__ ((unused))
 #endif
 #ifndef ALIGN
  #define ALIGN(bytes) __attribute__ ((aligned (bytes)))
 #endif
 #define EXPORTED __attribute__((visibility ("default")))
#else
 #define UNUSED
 #define EXPORTED
 #define ALIGN(bytes)
#endif

/* PRINTF_FORMAT(a, b) annotates a function prototype:
   argument number 'a' (starting at 1) is the printf-style format string,
   argument number 'b' and on are arguments to it. */
#ifndef PRINTF_FORMAT
 #if defined __GNUC__
  #define PRINTF_FORMAT(a, b) __attribute__ ((format (gnu_printf, a, b)))
 #else
  #define PRINTF_FORMAT(a, b)
 #endif
#endif

/* Annotating a function, PURE_FUNCTION means that it has no side-effects and
   its return value only depends on its arguments and global variables. */
#if defined __GNUC__
 #define PURE_FUNCTION __attribute__ ((pure))
#else
 #define PURE_FUNCTION
#endif

/*
 * GCC branch annotations: This can be used to give hints to the compiler
 * for code generation purposes. Use like this:
 *
 *     if (likely(some_condition)) { ... }
 *
 * Note that some_condition must be purely boolean (0 or 1), so use explicit
 * comparison operators if necessary.
 * Only use these annotations when one case is MUCH more common than the other;
 * otherwise we are better served by the dynamic prediction of the hardware.
 */
#if !defined(likely)
 #if defined(__GNUC__) && !defined(TURBO_SIMICS)
  #define likely(x) __builtin_expect((x), 1)
  #define unlikely(x) __builtin_expect((x), 0)
 #else
  #define likely(x) (x)
  #define unlikely(x) (x)
 #endif
#endif

#if defined(__linux__) && defined(__x86_64__)
 #define X86_64_LINUX
 #define ANY_LINUX
#elif defined(_WIN64) && defined(__GNUC__)
 #define GCC_WIN64
#elif defined(_WIN64) && defined(_MSC_VER)
  #define MSVC_WIN64
#else
 #error "Cannot recognize the operating system you are compiling for"
#endif

#if defined X86_64_LINUX
 #define SIMICS_HOST_TYPE "linux64"
#elif defined GCC_WIN64 || defined MSVC_WIN64
 #define SIMICS_HOST_TYPE "win64"
#else
 #error Cannot determine the Simics host type
#endif

#ifndef HOST_CACHE_LINE_SIZE
#define HOST_CACHE_LINE_SIZE 64
#endif

#if defined(_WIN32)
 #define DIR_SEP "\\"
 #define DIR_SEP_CHAR '\\'
 #define PATH_SEP ";"
 #define PATH_SEP_CHAR ';'
 #define SO_SFX "dll"
#else
 #define DIR_SEP "/"
 #define DIR_SEP_CHAR '/'
 #define PATH_SEP ":"
 #define PATH_SEP_CHAR ':'
 #define SO_SFX "so"
#endif

#if defined(__x86_64__) || defined(_M_AMD64)
 #define HOST_X86_64
#else
 #error Cannot recognize the host processor architecture you are compiling for
#endif

/* Function inlining macros */
#ifdef __GNUC__
 #ifdef __OPTIMIZE__
  #define FORCE_INLINE static inline __attribute__ ((always_inline))
 #else
  #define FORCE_INLINE static inline
 #endif
#else
 #if defined _MSC_VER && !defined __cplusplus
  #define inline __inline
 #endif
 #define FORCE_INLINE static inline
#endif

#ifdef _MSC_VER
 #define pclose _pclose
 /* These functions require "#include <string.h>" */
 #define strcasecmp stricmp
 #define strncasecmp strnicmp

 #define S_IRUSR _S_IREAD
 #define S_IWUSR _S_IWRITE
#endif /* _MSC_VER */

#endif
