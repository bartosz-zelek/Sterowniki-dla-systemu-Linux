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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_IO_MEMORY_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_IO_MEMORY_H

#include <simics/systemc/iface/io_memory_interface.h>

namespace unittest {
namespace iface {

class MockIoMemory : public simics::systemc::iface::IoMemoryInterface {
  public:
    MockIoMemory()
        : operation_(0), operation_return_(Sim_PE_No_Exception) {
    }
    virtual exception_type_t operation(generic_transaction_t *mem_op,
                                       const simics::types::map_info_t &info) {
        ++operation_;
        return operation_return_;
    }
    int operation_;
    exception_type_t operation_return_;
};

}  // namespace iface
}  // namespace unittest

#endif
