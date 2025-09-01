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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_INTERFACE_PROVIDER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_INTERFACE_PROVIDER_H

#include <simics/systemc/interface_provider.h>

namespace unittest {

class MockInterfaceProvider : public simics::systemc::InterfaceProvider {
  public:
    explicit MockInterfaceProvider(const char *interface_name)
        : InterfaceProvider(interface_name), target_count_(0) {}
    virtual const simics::ConfObjectRef &target() const {
        ++target_count_;
        return target_obj_;
    }
    mutable int target_count_;
};

}  // namespace unittest

#endif
