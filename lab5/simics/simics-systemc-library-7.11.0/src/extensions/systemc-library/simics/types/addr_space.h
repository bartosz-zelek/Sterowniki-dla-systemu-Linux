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

#ifndef SIMICS_TYPES_ADDR_SPACE_H
#define SIMICS_TYPES_ADDR_SPACE_H

namespace simics {
namespace types {

/// Stand-alone, version of the Simics addr_space_t enum.
typedef enum {
    Sim_Addr_Space_Conf,
    Sim_Addr_Space_IO,
    Sim_Addr_Space_Memory
} addr_space_t;

}  // namespace types
}  // namespace simics

#endif
