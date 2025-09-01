// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/class_attributes.h>

#include <algorithm>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

std::string ClassAttributes::adapter_class_name_;  // NOLINT(runtime/string)

std::string ClassAttributes::name(std::string class_name) const {
    if (!adapter_class_name_.empty())
        class_name = adapter_class_name_ + "_" + class_name;

    std::size_t found = class_name.find("::");
    if (found != std::string::npos)
        return class_name.replace(found, 2, "_");

    return class_name;
}

std::string ClassAttributes::name(sc_core::sc_object *object) const {
    return name(object->kind());
}

std::string ClassAttributes::uniqueName(sc_core::sc_object *object) const {
    std::string s = name(object) + "_" + object->name();
    std::replace(s.begin(), s.end(), '.', '_');
    return s;
}

std::string ClassAttributes::description(sc_core::sc_object *object) const {
    return std::string("SystemC ") + object->kind();
}

std::string ClassAttributes::documentation(sc_core::sc_object *object) const {
    return std::string("The <class>") + name(object)
        + "</class> class corresponds to the generic " + description(object)
        + " class.";
}

void ClassAttributes::set_adapter_class_name(const std::string &name) {
    adapter_class_name_ = name;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
