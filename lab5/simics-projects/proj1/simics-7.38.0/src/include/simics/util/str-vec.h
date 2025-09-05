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

#ifndef SIMICS_UTIL_STR_VEC_H
#define SIMICS_UTIL_STR_VEC_H

#include <simics/host-info.h>
#include <simics/util/vect.h>

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef PYWRAP

typedef struct {
        VECT(char *) v;
} str_vec_t;

#define STR_VEC_NULL {VNULL}

FORCE_INLINE void
str_vec_init(str_vec_t *sv) { VINIT(sv->v); }

FORCE_INLINE unsigned
str_vec_len(const str_vec_t *sv) { return VLEN(sv->v); }

FORCE_INLINE const char *
str_vec_get(const str_vec_t *sv, unsigned n) { return VGET(sv->v, n); }

FORCE_INLINE char **
str_vec_vec(str_vec_t *sv) { return VVEC(sv->v); }

FORCE_INLINE void
str_vec_clear(str_vec_t *sv) { VCLEAR(sv->v); }

/* like VFOREACH but declares the iteration variable itself */
#define STR_VEC_FOREACH(sv, iter)                       \
    for (char **iter = VVEC((sv)->v);                   \
         (iter) < VVEC((sv)->v) + VLEN((sv)->v);        \
         ++(iter))

void str_vec_free(str_vec_t *sv);
void str_vec_add(str_vec_t *sv, const char *s);
void str_vec_add_stealing(str_vec_t *sv, char *s);
void str_vec_sort(str_vec_t *sv);
const char *str_vec_find(const str_vec_t *sv, const char *s);
void str_vec_remove(str_vec_t *sv, const char *s);

#endif /* not PYWRAP */

#if defined(__cplusplus)
}
#endif

#endif
