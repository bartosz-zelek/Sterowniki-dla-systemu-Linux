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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_CLASS_ATTRIBUTES_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_CLASS_ATTRIBUTES_H

#include <simics/systemc/awareness/class_attributes_interface.h>

#include <string>

namespace unittest {
namespace awareness {

using simics::systemc::awareness::ClassAttributesInterface;

class MockClassAttributes : public ClassAttributesInterface {
  public:
    MockClassAttributes()
        : name_("Name"), uniqueName_("UniqueName"), objectName_("ObjectName"),
          description_("Description"), documentation_("Documentation") {
    }
    virtual std::string name(std::string class_name) const {
        return name_;
    }
    virtual std::string name(sc_core::sc_object *object) const {
        return name_;
    }
    virtual std::string uniqueName(sc_core::sc_object *object) const {
        return uniqueName_;
    }
    virtual std::string description(sc_core::sc_object *object) const {
        return description_;
    }
    virtual std::string documentation(sc_core::sc_object *object) const {
        return documentation_;
    }

    std::string name_;
    std::string uniqueName_;
    std::string objectName_;
    std::string description_;
    std::string documentation_;
};

}  // namespace awareness
}  // namespace unittest

#endif
