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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_SC_OBJECT_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_SC_OBJECT_H


#include <systemc>

namespace unittest {

class MockScObject : public sc_core::sc_module {
  public:
    explicit MockScObject(sc_core::sc_module_name name)
        : sc_core::sc_module(name), print_string_(NULL) {
    }
    virtual void print(::std::ostream& os = ::std::cout) const {
        if (print_string_)
            os << print_string_;
        else
            sc_core::sc_module::print(os);
    }
    const char *print_string_;
};

}  // namespace unittest

#endif
