// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/awareness/proxy_class_name.h>

#include <iomanip>
#include <string>

namespace simics {
namespace systemc {
namespace awareness {

/* Simics Class name grammar
   name ::= id ( '.' name )*
   id ::= [a-zA-Z_] [a-zA-Z0-9_-]*
*/

ProxyClassName::ProxyClassName() {}

ProxyClassName::ProxyClassName(std::string object_name) {
    init(object_name);
    name();
    *this += os_.str();
}

void ProxyClassName::name() {
    int c = 0;
    next(&c);
    id(&c);

    while (c) {
        if (c == '.') {
            next(&c);
            id(&c);
        } else {
            error(&c);
        }
    }

    // Add possible trailing "_"
    next(&c);
}

void ProxyClassName::id(int *c) {
    if (letter(*c) || *c == '_')
        next(c);
    else
        error(c);

    while (*c != '.' && *c != 0) {
        if (letter(*c) || digit(*c) || *c == '_' || *c == '-')
            next(c);
        else
            error(c);
    }
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
