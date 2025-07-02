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

#ifndef SIMICS_UTIL_GENRAND_H
#define SIMICS_UTIL_GENRAND_H

#include <simics/base-types.h>
#include <simics/base/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct rand_state rand_state_t;

rand_state_t *genrand_init(uint32 s);
rand_state_t *genrand_init_array(uint32 *init_key, unsigned key_length);
void genrand_destroy(rand_state_t *rs);

uint32 genrand_uint32(rand_state_t *rs);
uint64 genrand_uint64(rand_state_t *rs);
uint64 genrand_range(rand_state_t *rs, uint64 max);
double genrand_double(rand_state_t *rs);

bytes_t genrand_serialization(const rand_state_t *rs);
bool genrand_restore(rand_state_t *rs, bytes_t serialization);

#if defined(__cplusplus)
}
#endif

#endif
