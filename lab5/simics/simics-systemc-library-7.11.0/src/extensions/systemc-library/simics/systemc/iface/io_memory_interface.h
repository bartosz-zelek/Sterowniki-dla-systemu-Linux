// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_IO_MEMORY_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_IO_MEMORY_INTERFACE_H

#include <simics/devs/io-memory.h>
#include <simics/types/map_info.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics io_memory interface. */
class IoMemoryInterface {
  public:
    virtual exception_type_t operation(generic_transaction_t *mem_op,
                                       const types::map_info_t &info) = 0;
    virtual ~IoMemoryInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
