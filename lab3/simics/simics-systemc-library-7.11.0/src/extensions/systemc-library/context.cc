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

#include <simics/systemc/context.h>

namespace simics {
namespace systemc {

Context::Context(const iface::SimulationInterface *sim)
    : kernel_(sim->context()), report_handler_(sim->simics_object()) {
}

Context::~Context() {
}

sc_core::sc_simcontext *Context::operator->() const {
    return sc_core::sc_get_curr_simcontext();
}

}  // namespace systemc
}  // namespace simics
