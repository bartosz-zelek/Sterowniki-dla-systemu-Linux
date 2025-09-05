// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_STATE_HANDLER_H
#define SIMICS_SYSTEMC_STATE_HANDLER_H

#include <systemc>

#include <simics/systemc/internals.h>

namespace simics {
namespace systemc {

class StateHandler : public sc_core::sc_module {
  public:
    StateHandler()
        : sc_module(sc_core::sc_module_name("state_handler")) {
    }

    void end_of_simulation() {
        SIM_break_simulation("SystemC end-of-simulation intercepted");
    }
};

}  // namespace systemc
}  // namespace simics

#endif  // SIMICS_SYSTEMC_STATE_HANDLER_H
