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

#ifndef SIMICS_UTIL_ALLOC_H
#define SIMICS_UTIL_ALLOC_H

#include <stdlib.h>
#include <string.h>

#include <simics/module-host-config.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

/* The gcc 'malloc' attribute just tells the compiler that a function
   returns an alias-free pointer. Note that this is true even for realloc. */
#ifndef ATTRIBUTE_MALLOC
 #if defined __GNUC__ && __GNUC__ >= 3
  #define ATTRIBUTE_MALLOC __attribute__ ((__malloc__))
 #else
  #define ATTRIBUTE_MALLOC
 #endif
#endif

void mm_free(void *ptr);
void *mm_malloc(size_t size, size_t typesize, const char *type,
		const char *file, int line) ATTRIBUTE_MALLOC;
void *mm_zalloc(size_t size, size_t typesize, const char *type,
		const char *file, int line) ATTRIBUTE_MALLOC;
void *mm_realloc(void *ptr, size_t size, size_t typesize, const char *type,
		 const char *file, int line) ATTRIBUTE_MALLOC;
char *mm_strdup(const char *str, const char *file, int line) ATTRIBUTE_MALLOC;

/*
  <add-macro id="vtmem macros">
  <short>free allocation</short>

  <fun>MM_FREE</fun> frees an allocation previously made with
  <fun>MM_MALLOC</fun>, <fun>MM_MALLOC_SZ</fun>, <fun>MM_ZALLOC</fun>,
  <fun>MM_ZALLOC_SZ</fun>, <fun>MM_REALLOC</fun>, <fun>MM_REALLOC_SZ</fun>
  or <fun>MM_STRDUP</fun>.

  A null pointer argument is legal, in which case nothing happens.
  </add-macro>
*/
#define MM_FREE(p)							\
    mm_free(p)

/*
  <add-macro id="vtmem macros">
  <short>allocate memory</short>

  <fun>MM_MALLOC</fun> allocates <arg>nelems</arg> objects of type
  <arg>type</arg>. <fun>MM_MALLOC_SZ</fun> specifies the total allocation size
  in bytes.

  <fun>MM_ZALLOC</fun> and <fun>MM_ZALLOC_SZ</fun> do the same thing as
  <fun>MM_MALLOC</fun> and <fun>MM_ZALLOC</fun> respectively but in addition
  fill the allocated memory with null bytes.

  If <arg>nelems</arg> or <arg>size</arg> are zero, either a null pointer
  or a pointer to a zero-sized allocation is returned.

  <doc-item name="RETURN VALUE">Pointer to the allocated object(s).</doc-item>
  <di name="EXECUTION CONTEXT">All contexts (including Threaded Context)</di>
  </add-macro>
*/
#define MM_MALLOC(nelems, type)						\
        (type *)mm_malloc(sizeof(type) * (size_t)(nelems),              \
                          sizeof(type), #type,                          \
                          __FILE__, __LINE__)
/* <append-macro id="MM_MALLOC"/> */
#define MM_MALLOC_SZ(size, type)					\
    (type *)mm_malloc(size, sizeof(type), #type,			\
    		      __FILE__, __LINE__)
/* <append-macro id="MM_MALLOC"/> */
#define MM_ZALLOC(nelems, type)						\
        (type *)mm_zalloc(sizeof(type) * (size_t)(nelems),              \
                          sizeof(type), #type,                          \
                          __FILE__, __LINE__)
/* <append-macro id="MM_MALLOC"/> */
#define MM_ZALLOC_SZ(size, type)					\
    (type *)mm_zalloc(size, sizeof(type), #type,			\
                      __FILE__, __LINE__)

/*
  <add-macro id="vtmem macros">
  <short>reallocate memory</short>

  <fun>MM_REALLOC</fun> changes the size of an allocated memory block to
  <arg>nelems</arg> elements. <fun>MM_REALLOC_SZ</fun> specifies the new size
  in bytes.

  The allocation must originally have been made by a call to
  <fun>MM_MALLOC</fun>, <fun>MM_MALLOC_SZ</fun>, <fun>MM_ZALLOC</fun>,
  <fun>MM_ZALLOC_SZ</fun>, <fun>MM_REALLOC</fun>, <fun>MM_REALLOC_SZ</fun>
  or <fun>MM_STRDUP</fun>.

  If the passed pointer is null, then these macros are equivalent to an
  allocation of the desired amount. If <arg>nelems</arg> or <arg>size</arg> is
  zero, either a null pointer or a pointer to a zero-sized allocation is
  returned, and the original allocation is freed.

  The allocation must be freed using <fun>MM_FREE</fun>.

  <doc-item name="RETURN VALUE">Pointer to the reallocated object(s).
  </doc-item>
  <di name="EXECUTION CONTEXT">All contexts (including Threaded Context)</di>
  </add-macro>
*/
#define MM_REALLOC(p, nelems, type)					\
    (type *)mm_realloc(p, sizeof(type) * (nelems), sizeof(type), #type,	\
	               __FILE__, __LINE__)
/* <append-macro id="MM_REALLOC"/> */
#define MM_REALLOC_SZ(p, size, type)					\
    (type *)mm_realloc(p, size, sizeof(type), #type,			\
	               __FILE__, __LINE__)

/*
  <add-macro id="vtmem macros">
  <short>duplicate a string</short>

  Allocates and initializes a copy of the null-terminated string
  <arg>str</arg>. The allocation must be freed with <fun>MM_FREE</fun>.

  <doc-item name="RETURN VALUE">Pointer to the newly allocated
  string.</doc-item>
  <di name="EXECUTION CONTEXT">All contexts (including Threaded Context)</di>

  </add-macro>
*/
#define MM_STRDUP(str)							\
    mm_strdup(str, __FILE__, __LINE__)

#ifdef SIM_MALLOC_REDEFINE

#undef malloc
#undef calloc
#undef realloc
#undef free
#undef strdup

#define malloc(size) mm_malloc(size, 1, "char", __FILE__, __LINE__)
#define calloc(size, n) mm_zalloc((size) * (n), 1, "char",	\
				  __FILE__, __LINE__)
#define realloc(p, size) mm_realloc(p, size, 1, "char", __FILE__, __LINE__)
#define free(p) mm_free(p)
#define strdup(s) mm_strdup(s, __FILE__, __LINE__)

#endif /* SIM_MALLOC_REDEFINE */

void init_vtmem(void);

#endif /* !defined(PYWRAP) */

#ifdef __cplusplus
}
#endif

#endif
