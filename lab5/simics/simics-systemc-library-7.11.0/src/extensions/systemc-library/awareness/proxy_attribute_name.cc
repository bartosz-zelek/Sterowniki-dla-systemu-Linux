// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/proxy_attribute_name.h>

#include <iomanip>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

ProxyAttributeName::ProxyAttributeName()
    : skip_error_header_(false),
      error_occurred_(false),
      transformed_(false) {}

ProxyAttributeName::ProxyAttributeName(std::string attribute_name) {
    init(attribute_name);

    name();

    *this += os_.str();
}

bool ProxyAttributeName::transformed() {
    return transformed_;
}

void ProxyAttributeName::init(const std::string &name) {
    skip_error_header_ = false;
    error_occurred_ = false;
    transformed_ = false;
    it_ = name.begin();
    it_end_ = name.end();
}

bool ProxyAttributeName::drop(int *c) {
    if (it_ != it_end_) {
        *c = *it_++;
        return true;
    }

    *c = 0;
    return false;
}

bool ProxyAttributeName::next(int *c) {
    if (skip_error_header_) {
        os_ << std::dec;
        os_ << std::nouppercase;
        os_ << "_";
    }

    skip_error_header_ = false;

    // Skip initial 0
    if (*c)
        os_ << static_cast<char>(*c);

    return drop(c);
}

bool ProxyAttributeName::error(int *c) {
    if (!skip_error_header_) {
        os_ << std::uppercase;
        os_ << std::hex;
        os_ << "_0x" << std::setw(2);
        error_occurred_ = true;
    }

    transformed_ = true;
    skip_error_header_ = true;
    os_ << int((unsigned char) *c);

    return drop(c);
}

bool ProxyAttributeName::letter(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool ProxyAttributeName::digit(int c) {
    return c >= '0' && c <= '9';
}

void ProxyAttributeName::name() {
    int c = 0;
    next(&c);

    if (letter(c) || c == '_')
        next(&c);
    else
        error(&c);

    while (c != 0) {
        if (letter(c) || digit(c) || c == '_')
            next(&c);
        else
            error(&c);
    }

    // Add possible trailing "_"
    next(&c);
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
