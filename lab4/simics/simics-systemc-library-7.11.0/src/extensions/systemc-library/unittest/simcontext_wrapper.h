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

#ifndef SYSTEMC_LIBRARY_UNITTEST_SIMCONTEXT_WRAPPER_H
#define SYSTEMC_LIBRARY_UNITTEST_SIMCONTEXT_WRAPPER_H

#include <boost/core/noncopyable.hpp>

#include <systemc>

namespace unittest {

class SimContextWrapper : private boost::noncopyable {
  public:
    SimContextWrapper();
    ~SimContextWrapper();

    sc_core::sc_simcontext *simcontext() const;

  private:
    mutable sc_core::sc_simcontext *context_;
    sc_core::sc_simcontext *former_context_;
};

}  // namespace unittest

#endif
