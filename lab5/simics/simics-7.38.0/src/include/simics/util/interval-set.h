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

#ifndef SIMICS_UTIL_INTERVAL_SET_H
#define SIMICS_UTIL_INTERVAL_SET_H

#include <simics/host-info.h>
#include <simics/base-types.h>
#include <simics/util/vect.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined(TURBO_SIMICS)
 typedef struct interval_set interval_set_t;
 typedef struct interval_set_iter interval_set_iter_t;
 #define HAVE_INTERVAL_SET_T
#endif /* !TURBO_SIMICS */

typedef struct { 
        uint64 start;
        uint64 end;
        void *ptr;
} range_node_t;

struct interval_section {
        uint64 start;           /* start of this section */
        /* section ends right before start of the next section, or
           at 0xffffffffffffffff if it is the last */
        struct interval_bucket *intervals; /* NULL if no intervals here */
};

struct interval_set {
        VECT(struct interval_section) sections;
};

/* Return true iff interval set is empty. */
FORCE_INLINE int
interval_set_empty(interval_set_t *is)
{
        return VLEN(is->sections) == 1
                && VGET(is->sections, 0).intervals == NULL;
}

void init_interval(interval_set_t *is, int allow_overlap);

struct interval_set_iter {
        const interval_set_t *is;
        uint64 start, end;
        unsigned sect;          /* section index */
        unsigned inum;          /* interval index */
};

interval_set_t *new_interval(int dummy);
void insert_interval(interval_set_t *is, uint64 start, uint64 end,
                     void *ptr);
void *get_interval_ptr(interval_set_t *is, uint64 address);
unsigned get_interval_vector(interval_set_t *is, uint64 address,
                             range_node_t **res);
unsigned get_interval_vector_and_range(interval_set_t *is, uint64 address,
                                       range_node_t **res,
                                       uint64 *first_address,
                                       uint64 *last_address);
void *get_lower_interval_ptr(interval_set_t *is, uint64 address);
void *get_higher_interval_ptr(interval_set_t *is, uint64 address);

int touch_interval(interval_set_t *is, uint64 start, uint64 end);

typedef void (*intervals_func_t)(uint64 start, uint64 end,
                                 void *el, void *data);

void for_all_intervals(interval_set_t *is, intervals_func_t f, void *data);
void for_some_intervals(interval_set_t *is, uint64 start, uint64 end,
			intervals_func_t f, void *data);

void remove_interval(interval_set_t *is, uint64 start, uint64 end,
                     void *ptr);
void clear_interval(interval_set_t *is);
void free_interval(interval_set_t *is);

void interval_set_new_iter(const interval_set_t *is, interval_set_iter_t *it,
                           uint64 start, uint64 end);

range_node_t *interval_set_iter_next(interval_set_iter_t *it);

#if defined(__cplusplus)
}
#endif

#endif
