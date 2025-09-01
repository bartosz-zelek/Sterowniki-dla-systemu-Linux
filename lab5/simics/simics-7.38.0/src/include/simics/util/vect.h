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

#ifndef SIMICS_UTIL_VECT_H
#define SIMICS_UTIL_VECT_H

/*
 * General vector data type, VECT(T)
 *
 * It has the following properties:
 *
 * - Small enough to be passed around by value
 * - Grows automatically
 * - Initialisation is mandatory, but no storage is allocated until needed
 * - Allows access to underlying C array for low-level manipulation
 * - Although implemented as macros, is actually quite type-safe in practice
 * - This file contains only preprocessor code and is order-independent of
 * other include files.
 *
 */

#include <string.h>
#include <stdio.h>
#include <simics/host-info.h>

#include <simics/util/alloc.h>

#ifdef __COVERITY__
 #define VECT_COVERITY 1
#else
 #define VECT_COVERITY 0
#endif

#if VECT_COVERITY
#include <simics/util/help-macros.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/* Please do not access the members of a VECT() struct directly;
   use the accessors defined below. */
#define VECT(type) struct { int size, used; type *elements; }

#define VNULL { 0, 0, NULL }

/* Initialize vector that has not been initialized yet.
   This is equivalent to initialising it with VNULL. 
   Either VINIT or initialisation with VNULL must be done before a vector
   can be used.*/
#define VINIT(v) do {                                                   \
        (v).size = (v).used = 0;                                        \
        (v).elements = NULL;                                            \
} while (0)

#define VELEMSIZE(v) (sizeof (v).elements[0])

#define VECT_INITIAL_SIZE 64    /* initial size in bytes */
#define VECT_INITIAL_NELEMS(v) (MAX(1, VECT_INITIAL_SIZE / (int)VELEMSIZE(v)))

#define _MEM_SIZE(v, s) ((size_t)(s) * VELEMSIZE(v))
#define VECT_MEM_SIZE(v) _MEM_SIZE((v), (v).size)
#define VECT_MEM_USED(v) _MEM_SIZE((v), VLEN(v))

/* Unfortunately we don't have the element type here; char will have to do. */
#define VREALLOC_BYTES(p, n) ((void *)MM_REALLOC_SZ(p, n, char))
#define VFREE_BYTES(p) MM_FREE(p)

/* The condition that the elements vector has been allocated,
   if we are checked by Coverity which does not know the VECT invariants.  */
#define VECT_ELEMS_PRESENT(v) (!VECT_COVERITY || (v).elements)

#if VECT_COVERITY
#define COV_ASSERT(x) ASSERT(x)
#else
#define COV_ASSERT(x)
#endif

/* Resize v to n elements */
#define VRESIZE(v, n) do {                                                     \
        VASSERT((int)(n) >= 0);                                                \
        (v).used = (int)(n);                                                   \
        if ((v).size < (v).used) {                                             \
		if ((v).size == 0)                                             \
                        (v).size = VECT_INITIAL_NELEMS(v);                     \
                while ((v).size < (v).used)                                    \
                        (v).size *= 2;                                         \
                (v).elements = VREALLOC_BYTES((v).elements, VECT_MEM_SIZE(v)); \
        }                                                                      \
        if ((v).size) {                                                        \
                COV_ASSERT((v).elements);                                      \
        }                                                                      \
} while (0)

/* Resize v to n elements, freeing excessive space */
#define VRESIZE_FREE(v, n) do {                                               \
        VASSERT((int)(n) >= 0);                                               \
        (v).used = (v).size = (int)(n);                                       \
        (v).elements = VREALLOC_BYTES((v).elements, VECT_MEM_SIZE(v));        \
} while (0)

/* Add e last to vector v */                                                  \
#define VADD(v, e) do {                                                       \
        int __n = (int)VLEN(v);                                               \
        if (__n == (v).size) {                                                \
                (v).size = __n == 0 ? VECT_INITIAL_NELEMS(v) : (v).size * 2;  \
                (v).elements = VREALLOC_BYTES((v).elements,                   \
                                              VECT_MEM_SIZE(v));              \
        }                                                                     \
        (v).used = __n + 1;                                                   \
        COV_ASSERT((v).elements);                                             \
        VSET(v, __n, e);                                                      \
} while (0)

#if !defined PYWRAP && !defined GULP_API_HELP
static inline int
_vcontainsp(const void **hay, const void *needle, unsigned size)
{
        for (unsigned i = 0; i < size && (!VECT_COVERITY || hay); i++)
                if (*(hay + i) == needle)
                        return 1;
        return 0;
}

#define VCONTAINSP(v, e) _vcontainsp((const void **)VVEC(v), e, VLEN(v))
#endif

/* Add e to vector if only e is not already present */
#define VADD_NODUPLICATE(v, e) do {             \
        int _add = 1;                           \
        VFORI(v, _i) {                          \
                if (VGET(v, _i) == (e)) {       \
                        _add = 0;               \
                        break;                  \
                }                               \
        }                                       \
        if (_add)                               \
                VADD(v, e);                     \
} while (0)

/* Remove v[i], not keeping vector order (fast). All other elements will remain
   at the same index, except that if v[i] was not the last element, the last
   element will be moved to index i. */
#define VREMOVE(v, i) do {                                              \
        int __idx = (int)(i);                                           \
        (void)VINDEX((v), __idx);                                       \
        if ((v).used-- > __idx)                                         \
                (v).elements[__idx] = (v).elements[(v).used];           \
} while (0)

/* Remove v[i], keeping vector order (slower) */
#define VDELETE_ORDER(v, i) do {                                        \
        int __idx = (int)(i);                                           \
        (void)VINDEX((v), __idx);                                       \
        if (--(v).used > __idx)                                         \
                memmove((v).elements + __idx,                           \
                        (v).elements + __idx + 1,                       \
                        _MEM_SIZE(v, (v).used - __idx));                \
} while (0)

/* Remove e if found in the vector */
#define VREMOVE_FIRST_MATCH(v, e) do {          \
        VFORI(v, _i) {                          \
                if (VGET(v, _i) == (e)) {       \
                        VREMOVE(v, _i);         \
                        break;                  \
                }                               \
        }                                       \
} while (0)

/* Insert e at position i of vector v, keeping vector order */
#define VINSERT(v, i, e) do {                                           \
        int __idx = (int)(i);                                           \
        unsigned __n = VLEN(v);                                         \
        VASSERT(__idx >= 0 && __idx <= __n);                            \
        VRESIZE(v, __n + 1);                                            \
        if (__n - (unsigned)__idx > 0)                                  \
                memmove((v).elements + __idx + 1,                       \
                        (v).elements + __idx,                           \
                        _MEM_SIZE(v, __n - (unsigned)__idx));           \
        VSET(v, __idx, e);                                              \
} while (0)

#define VSET(v, i, e)	((v).elements[VINDEX(v, i)] = (e))
#define VGET(v, i)	((v).elements[VINDEX(v, i)])

/* Remove last element and return it */
#define VPOP(v)		(VASSERT(!VEMPTY(v)), (v).elements[--(v).used])

/* Remove last element (no return value) */
#define VDROPLAST(v)    (VASSERT(!VEMPTY(v)), (v).used--)

#define VFIRST(v)       VGET(v, 0)
#define VLAST(v)        VGET(v, VLEN(v) - 1)
#define VSETLAST(v, e)  VSET(v, VLEN(v) - 1, (e))

#define VVEC(v)		((v).elements)

/* Macros that access the length (VLEN and VEMPTY) are guaranteed
   only to touch the memory in the vect itself, not the
   heap-allocated block. */
#define VLEN(v)		(unsigned)((v).used)
#define VEMPTY(v)       (VLEN(v) == 0)

#define VGROW(v, n)     VRESIZE((v), VLEN(v) + (unsigned)(n))
#define VSHRINK(v, n)   VRESIZE((v), VLEN(v) - (unsigned)(n))

/* Let iter point to each of the elements in turn. */
#define VFOREACH(v, iter)                                               \
	for ((iter) = (v).elements;                                     \
             VECT_ELEMS_PRESENT(v) && (iter) < (v).elements + (v).used; \
             ++(iter))

/* Let iter point to each of the elements in turn. */
#define VFOREACH_T(v, iter_type, iter)                                  \
	for (iter_type *iter = (v).elements;                            \
             VECT_ELEMS_PRESENT(v) && (iter) < (v).elements + (v).used; \
             ++(iter))

/* Let i iterate over all valid indices. Declares i locally.*/
#define VFORI(v, i)                                                     \
        for (int i = 0; i < (v).used && VECT_ELEMS_PRESENT(v); i++)

/* Internal: let e iterate over the elements of v, which must be
   of type VECT(T splat) where splat is zero or more asterisks. */
#define VFORSPLAT(v, T, splat, e)                       \
        for (T splat *_p = (v).elements, splat e;       \
             VECT_ELEMS_PRESENT(v)                      \
             && _p < (v).elements + (v).used             \
             && (e = *_p, 1);                           \
             _p++)

/* Let e iterate over all elements of v, which must be of type VECT(type);
   type must not contain any asterisks. */
#define VFORT(v, type, e) VFORSPLAT(v, type, , e)

/* Let e iterate over all elements of v, which must be of type VECT(type *). */
#define VFORP(v, type, e) VFORSPLAT(v, type, *, e)

/* Free associated storage and reduce size of v to zero */
#define VFREE(v) do {                           \
        VFREE_BYTES((v).elements);              \
        VINIT(v);                               \
} while (0)

/* Truncate vector to shorter size, but do not free any storage */
#define VTRUNCATE(v, n) (VASSERT((int)(n) >= 0), (v).used = (int)(n))

/* Reduce size of v to zero, but do not free any storage */
#define VCLEAR(v) VTRUNCATE(v, 0)

/* Copy the contents of src_v to dst_v. */
#define VCOPY(dst_v, src_v) do {                                        \
        VRESIZE(dst_v, VLEN(src_v));                                    \
        if (VECT_ELEMS_PRESENT(dst_v) && VECT_ELEMS_PRESENT(src_v)) {   \
                if (VECT_MEM_USED(src_v) >= PTRDIFF_MAX)                \
                        __builtin_unreachable();                        \
                memcpy((dst_v).elements,                                \
                       (src_v).elements,                                \
                       VECT_MEM_USED(src_v));                           \
        }                                                               \
        if (VECT_COVERITY && VLEN(src_v))                               \
            ASSERT((dst_v).elements);                                   \
} while (0)

/* Append the contents of src_v to dst_v. */
#define VAPPEND(dst_v, src_v) do {                                      \
        VASSERT(VELEMSIZE(dst_v) == VELEMSIZE(src_v));                  \
        unsigned __dst_len = VLEN(dst_v);                               \
        VRESIZE(dst_v, VLEN(dst_v) + VLEN(src_v));                      \
        if (VECT_ELEMS_PRESENT(dst_v) && VECT_ELEMS_PRESENT(src_v))     \
                memcpy((dst_v).elements + __dst_len,                    \
                       (src_v).elements,                                \
                       VECT_MEM_USED(src_v));                           \
} while (0)

/* Sort a vector in-place. cmp is as for qsort */
#define VSORT(v, cmp) do {                                              \
        if (!VEMPTY(v))                                                 \
                qsort(VVEC(v), VLEN(v), VELEMSIZE(v), cmp);             \
} while (0)

#ifndef VECT_DEBUG
 #define VECT_DEBUG 0
#endif
#if VECT_DEBUG

FORCE_INLINE void
vassert_error(const char *file, int line, const char *cond)
{
        fprintf(stderr, "%s:%d: VECT assertion failure: %s\n",
                file, line, cond);
        abort();
}

FORCE_INLINE int
vassert_index(const char *file, int line, int limit, int idx)
{
        if (idx < 0 || idx >= limit)
                vassert_error(file, line, "index out of bounds");
        return idx;
}

#define VASSERT(cond) ((cond)                                                \
                       ? (void)0 : vassert_error(__FILE__, __LINE__, #cond))
#define VINDEX(v, i) vassert_index(__FILE__, __LINE__, VLEN(v), i)

#else  /* not VECT_DEBUG */

#define VASSERT(cond) ((void)0)
#define VINDEX(v, i) (i)

#endif /* not VECT_DEBUG */

/*
 * Queue data type
 * Can be extended to deque functionality with little effort
 */

/* Please do not access the members of this struct directly. */
#define QUEUE(type)                                                     \
struct {                                                                \
        VECT(type) v;                                                   \
        unsigned mask;          /* circular index mask */               \
        unsigned next_add;      /* where to add next element */         \
        unsigned next_remove;   /* which element to remove next */      \
}

/*
 * Invariants:
 * - VLEN(q.v) is always 0 or a power of 2
 * - q.mask == (VLEN(q.v) == 0) ? 0 : VLEN(q.v) - 1
 * - q.next_add - q.next_remove == number of elements in queue
 * - q.next_add & q.mask is where to add next element
 * - q.next_remove & q.mask is what element to remove next
 */

#define QNULL { VNULL, 0, 0, 0 }

/* Initialize a queue */
#define QINIT(q) do {                                   \
        VINIT((q).v);                                   \
        (q).mask = (q).next_add = (q).next_remove = 0;  \
} while (0)

/* Return true iff q is empty */
#define QEMPTY(q) ((q).next_add == (q).next_remove)

/* Return number of elements in queue */
#define QLEN(q) ((q).next_add - (q).next_remove)

/* Add element to front of queue */
#define QADD(q, e) do {                                                   \
        unsigned __mask = (q).mask;                                       \
        if ((q).next_add - (q).next_remove == __mask) {                   \
                /* queue full - expand */                                 \
                unsigned __newsize = (__mask + 1) << 1;                   \
                (q).next_add &= __mask;                                   \
                (q).next_remove &= __mask;                                \
                VRESIZE((q).v, __newsize);                                \
                if ((q).next_add < (q).next_remove) {                     \
                        /* move lower part of queue up past upper part */ \
                        memcpy(VVEC((q).v) + (__mask + 1),                \
                               VVEC((q).v),                               \
                               _MEM_SIZE((q).v, (q).next_add));           \
                        (q).next_add += __mask + 1;                       \
                }                                                         \
                (q).mask = __mask = __newsize - 1;                        \
        }                                                                 \
        VSET((q).v, (q).next_add & __mask, e);                            \
        (q).next_add++;                                                   \
} while (0)

/* Remove and return element */
#define QREMOVE(q) VGET((q).v, (q).next_remove++ & (q).mask)

/* Return the first element without removing it. */
#define QPEEK(q) QGET(q, 0)

/* Drop the k first elements */
#define QDROP(q, k) do { (q).next_remove += (k); } while (0)

/* retrieve (but do not remove) ith element of q, most recently added last */
#define QGET(q, i) VGET((q).v, ((q).next_remove + (unsigned)(i)) & (q).mask)

/* Retrieve (but do not remove) the most recently added element */
#define QLAST(q) VGET((q).v, ((q).next_add - 1) & (q).mask)

/* Free queue storage; will then be ready for re-use. */
#define QFREE(q)                                        \
do {                                                    \
        VFREE((q).v);                                   \
        (q).mask = (q).next_add = (q).next_remove = 0;  \
} while (0)

/* Clear queue, but do not free storage; will then be ready for re-use. */
#define QCLEAR(q)                                       \
do {                                                    \
        VTRUNCATE((q).v, 0);                            \
        (q).mask = (q).next_add = (q).next_remove = 0;  \
} while (0)

#if defined(__cplusplus)
}
#endif

#endif
