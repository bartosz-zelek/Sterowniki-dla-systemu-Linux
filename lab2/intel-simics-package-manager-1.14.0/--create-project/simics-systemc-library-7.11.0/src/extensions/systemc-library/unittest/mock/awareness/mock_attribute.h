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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_ATTRIBUTE_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_ATTRIBUTE_H

#include <simics/systemc/awareness/attribute.h>

namespace unittest {
namespace awareness {

using simics::systemc::awareness::Attribute;

class MockAttribute : public Attribute {
  public:
    explicit MockAttribute(int uniqueKey)
        : Attribute(uniqueKey), init_(0), set_return(Sim_Set_Ok) {
    }
    virtual Attribute *create() {
        ++init_;
        return new MockAttribute(*this);
    }
    virtual attr_value_t get() const {
        return get_return;
    }
    virtual set_error_t set(attr_value_t *val) {
        return set_return;
    }
    virtual ~MockAttribute() {
        ++deleted_;
    }
    int init_;
    static int deleted_;
    attr_value_t get_return;
    set_error_t set_return;
};

}  // namespace awareness
}  // namespace unittest

#endif
