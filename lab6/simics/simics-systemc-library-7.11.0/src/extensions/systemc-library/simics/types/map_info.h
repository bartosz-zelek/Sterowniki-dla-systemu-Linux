// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPES_MAP_INFO_H
#define SIMICS_TYPES_MAP_INFO_H

#include <stdint.h>

namespace simics {
namespace types {

/// Reduced, stand-alone, version of the Simics map_info_t struct.
struct map_info_t {
    typedef uint64_t physical_address_t;
    physical_address_t base;
    physical_address_t start;
    physical_address_t length;
    int                function;

    map_info_t() : base(0), start(0), length(0), function(0) {}
    map_info_t(physical_address_t base,
               physical_address_t start,
               physical_address_t length,
               int function)
        : base(base), start(start), length(length), function(function) {}
};

}  // namespace types
}  // namespace simics

#endif
