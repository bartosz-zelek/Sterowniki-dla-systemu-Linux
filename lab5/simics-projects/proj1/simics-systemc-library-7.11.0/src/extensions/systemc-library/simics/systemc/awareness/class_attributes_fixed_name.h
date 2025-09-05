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

#ifndef SIMICS_SYSTEMC_AWARENESS_CLASS_ATTRIBUTES_FIXED_NAME_H
#define SIMICS_SYSTEMC_AWARENESS_CLASS_ATTRIBUTES_FIXED_NAME_H

#include <simics/systemc/awareness/class_attributes.h>

#include <algorithm>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ClassAttributesFixedName : public ClassAttributes {
  public:
    explicit ClassAttributesFixedName(const char *name) : name_(name) {}
    virtual std::string name(sc_core::sc_object *object) const {
        return ClassAttributes::name(name_);
    }
  private:
    const char *name_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
