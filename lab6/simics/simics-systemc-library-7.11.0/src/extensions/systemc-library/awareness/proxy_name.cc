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

#include <simics/systemc/awareness/proxy_name.h>

#include <iomanip>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

const char *ProxyName::error_prefix_name_ = NULL;

void ProxyName::set_error_prefix_name(const char *error_prefix_name) {
    error_prefix_name_ = error_prefix_name;
}

ProxyName::ProxyName() {}

ProxyName::ProxyName(std::string object_name) {
    init(object_name);

    name();
    if (error_occurred_ && error_prefix_name_)
        assign(error_prefix_name_);

    *this += os_.str();
}

void ProxyName::name() {
    int c = 0;
    next(&c);
    part(&c);

    while (c) {
        if (c == '.') {
            next(&c);
            part(&c);
        } else {
            error(&c);
        }
    }

    // Add possible trailing "_"
    next(&c);
}

void ProxyName::part(int *c) {
    id(c);

    // Array validity already checked in id()
    if (*c == '[') {
        next(c);

        while (digit(*c))
            next(c);

        if (*c == ']')
            next(c);
        else
            error(c);
    }
}

void ProxyName::id(int *c) {
    if (letter(*c) || *c == '_')
        next(c);
    else
        error(c);

    while (*c != '.' && *c != 0) {
        if (letter(*c) || digit(*c) || *c == '_')
            next(c);
        else if (array(*c))
            return;
        else
            error(c);
    }
}

bool ProxyName::array(int c) {
    if (c != '[')
        return false;

    int digits = 0;
    std::string::const_iterator it;
    for (it = it_; it != it_end_; ++it) {
        if (*it == ']') {
            // Check that array is not empty.
            if (digits == 0)
                return false;

            // Check that array is at the end.
            ++it;
            if (it != it_end_ && *it != '.')
                return false;

            return true;
        }

        // Check that the array contains only digits.
        if (digit(*it))
            ++digits;
        else
            return false;
    }

    return false;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
