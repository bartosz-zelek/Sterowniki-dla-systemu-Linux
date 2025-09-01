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

#ifndef SIMICS_UTIL_PROF_DATA_H
#define SIMICS_UTIL_PROF_DATA_H

#include <simics/util/hashtab.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct prof_data prof_data_t;

#if !defined PYWRAP && !defined GULP_API_HELP

typedef uint64 prof_data_address_t;
typedef uint64 prof_data_counter_t;

typedef struct {
        prof_data_t *pd;
        uint64 start, end;

        ht_int_iter_t hit;
        int index;                         /* position in current chunk */
} prof_data_iter_t;

prof_data_t *prof_data_create(unsigned granularity);
void prof_data_delete(prof_data_t *pd);
void prof_data_clear(prof_data_t *pd);
unsigned prof_data_granularity_log2(prof_data_t *pd);
void prof_data_set(prof_data_t *pd, prof_data_address_t address,
                   prof_data_counter_t value);
void prof_data_add(prof_data_t *pd, prof_data_address_t address,
                   prof_data_counter_t value);

prof_data_counter_t prof_data_get(prof_data_t *pd,
                                  prof_data_address_t address);

void prof_data_new_iter(prof_data_t *pd, prof_data_iter_t *it,
                        prof_data_address_t start, prof_data_address_t end);
prof_data_counter_t prof_data_iter_next(prof_data_iter_t *it);
prof_data_address_t prof_data_iter_addr(prof_data_iter_t *it);

int prof_data_save(prof_data_t *pd, FILE *f);
int prof_data_load(prof_data_t *pd, FILE *f);

#endif /* not PYWRAP and not GULP_API_HELP */

#if defined(__cplusplus)
}
#endif

#endif
