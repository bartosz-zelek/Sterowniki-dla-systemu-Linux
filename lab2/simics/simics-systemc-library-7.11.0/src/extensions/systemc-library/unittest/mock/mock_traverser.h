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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TRAVERSER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TRAVERSER_H

#include <simics/systemc/traverser_interface.h>
#include <vector>

namespace unittest {

class MockTraverser : public simics::systemc::TraverserInterface {
  public:
    MockTraverser() : done_count_(0) {}
    virtual void applyOn(sc_core::sc_object *obj) {
        apply_on_.push_back(obj);
    }
    virtual void done() {
        ++done_count_;
    }
    std::vector <sc_core::sc_object*> apply_on_;
    int done_count_;
};

}  // namespace unittest

#endif
