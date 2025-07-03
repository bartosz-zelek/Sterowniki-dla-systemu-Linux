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

#include "simcontext_wrapper.h"

namespace unittest {

SimContextWrapper::SimContextWrapper() {
    former_context_ = sc_core::sc_curr_simcontext;
    context_ = new sc_core::sc_simcontext();
    sc_core::sc_default_global_context = context_;
    sc_core::sc_curr_simcontext = context_;
}

SimContextWrapper::~SimContextWrapper() {
    sc_core::sc_default_global_context = former_context_;
    sc_core::sc_curr_simcontext = former_context_;
    delete context_;
}

sc_core::sc_simcontext *SimContextWrapper::simcontext() const {
    return context_;
}
}  // namespace unittest
