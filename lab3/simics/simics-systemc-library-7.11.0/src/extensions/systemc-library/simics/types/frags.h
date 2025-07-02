// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPES_FRAGS_H
#define SIMICS_TYPES_FRAGS_H

#include <stdint.h>
#include <string.h>

namespace simics {
namespace types {

#define MAX_FRAGS_FRAGS 8

/* This is a stand-alone subset of the Simics frags_t struct
   it is copied from simics/util/frags.h and for internal use only
   User should use the two functions to extract and form the frags_t
   struct instead of operate on the bare struct.
 */

struct frags_frag_t {
    const uint8_t *start;
    size_t len;
};

struct frags_t {
    size_t len;
    unsigned nfrags;
    frags_frag_t fraglist[MAX_FRAGS_FRAGS];
};

inline void frags_extract(const frags_t *buf, void *vdst) {
    uint8_t *dst = static_cast<uint8_t*>(vdst);
    // No need to use an explicit iterator, since this is a very
    // simple special case
    for (unsigned i = 0; i < buf->nfrags; i++) {
        memcpy(dst, buf->fraglist[i].start, buf->fraglist[i].len);
        dst += buf->fraglist[i].len;
    }
}

inline void frags_init_add(simics::types::frags_t *buf,
                           const void *data, size_t len) {
    buf->len = len;
    buf->nfrags = 1;
    buf->fraglist[0].start = (const uint8_t *)data;
    buf->fraglist[0].len = len;
}

}  // namespace types
}  // namespace simics

#endif
