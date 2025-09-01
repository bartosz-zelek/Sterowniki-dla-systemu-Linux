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

#include <simics/systemc/event.h>

#include <simics/util/help-macros.h>

#include <string>

namespace simics {
namespace systemc {

Event::Event(const sc_core::sc_time &when, const std::string &obj_name,
             const Type &type, const std::string &kind)
    : when_(when), obj_name_(obj_name), kind_(kind), type_(type) {
    FATAL_ERROR_IF(type > AND_LIST_TIMEOUT, "Event description out of bounds");
}

Event::Event(const sc_core::sc_time &when, const std::string &obj_name,
             int type, const std::string &kind)
    : when_(when), obj_name_(obj_name), kind_(kind) {
    type_ = static_cast<Type>(type);
    FATAL_ERROR_IF(type_ > AND_LIST_TIMEOUT, "Event description out of bounds");
}

sc_core::sc_time Event::when() const {
    return when_;
}

std::string Event::what() const {
    std::string result(obj_name_);
    if (!kind_.empty())
        result += " (" + kind_ + ")";

    return result;
}

Event::Type Event::type() const {
    return type_;
}

std::string Event::kind() const {
    return kind_;
}

std::string Event::name() const {
    return obj_name_;
}

}    // namespace systemc
}    // namespace simics

