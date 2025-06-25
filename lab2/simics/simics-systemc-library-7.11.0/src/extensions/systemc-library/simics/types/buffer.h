// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPES_BUFFER_H
#define SIMICS_TYPES_BUFFER_H

#include <cstddef>  // size_t
#include <cstdint>

namespace simics {
namespace types {

/* This is a stand-alone subset of the Simics buffer_t struct
   it is copied from simics/base/types.h and for internal use only
 */

struct buffer_t {
    uint8_t *data;
    size_t len;
};

}  // namespace types
}  // namespace simics

#endif
