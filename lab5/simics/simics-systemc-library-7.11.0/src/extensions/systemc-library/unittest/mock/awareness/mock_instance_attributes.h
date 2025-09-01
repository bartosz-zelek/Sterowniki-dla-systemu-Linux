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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_INSTANCE_ATTRIBUTES_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_INSTANCE_ATTRIBUTES_H

#include <simics/systemc/awareness/instance_attributes_interface.h>

#include <string>

namespace unittest {
namespace awareness {

using simics::systemc::awareness::InstanceAttributesInterface;

class MockInstanceAttributes : public InstanceAttributesInterface {
  public:
    virtual std::string name(sc_core::sc_object *object) const {
        if (name_ == "")
            return object->name();

        return name_;
    }
    virtual std::string basename(sc_core::sc_object *object) const {
        if (basename_ == "")
            return object->basename();

        return basename_;
    }

    std::string name_;
    std::string basename_;
};

}  // namespace awareness
}  // namespace unittest

#endif
