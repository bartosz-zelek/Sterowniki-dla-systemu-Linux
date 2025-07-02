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

#ifndef SIMICS_SYSTEMC_AWARENESS_CLASS_ATTRIBUTES_H
#define SIMICS_SYSTEMC_AWARENESS_CLASS_ATTRIBUTES_H

#include <simics/systemc/awareness/class_attributes_interface.h>

#include <string>

namespace simics {
namespace systemc {
namespace awareness {

class ClassAttributes : public ClassAttributesInterface {
  public:
    virtual std::string name(std::string class_name) const;
    virtual std::string name(sc_core::sc_object *object) const;
    virtual std::string uniqueName(sc_core::sc_object *object) const;
    virtual std::string description(sc_core::sc_object *object) const;
    virtual std::string documentation(sc_core::sc_object *object) const;
    static void set_adapter_class_name(const std::string &name);

  private:
    static std::string adapter_class_name_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
