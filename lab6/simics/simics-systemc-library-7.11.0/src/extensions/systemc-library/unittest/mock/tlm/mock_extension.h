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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_TLM_MOCK_EXTENSION_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_TLM_MOCK_EXTENSION_H


#include <tlm>

namespace unittest {

class MockExtension : public tlm::tlm_extension<MockExtension> {
  public:
    //  tlm_extension
    virtual tlm::tlm_extension_base *clone() const {
        return new MockExtension(*this);
    }
    virtual void copy_from(tlm::tlm_extension_base const &ext) {
        *this = static_cast<const MockExtension &>(ext);
    }
};

}  // namespace unittest

#endif
