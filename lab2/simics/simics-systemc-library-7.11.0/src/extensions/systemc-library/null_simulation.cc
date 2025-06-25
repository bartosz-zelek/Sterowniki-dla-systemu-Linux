// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/kernel.h>
#include <simics/systemc/null_simulation.h>
#include <simics/systemc/sim_context_proxy.h>

namespace {
class StoppedScSimcontext : public sc_core::sc_simcontext {
  public:
    StoppedScSimcontext() {
        simics::systemc::Kernel k(this);
        sc_core::simContextProxy::set_simulation_status(sc_core::SC_STOPPED);
    }
};
}

namespace simics {
namespace systemc {

int NullSimulation::runDeltaPhase(int count) {
    return 0;
}

bool NullSimulation::runSimulation(sc_core::sc_time t) {
    return false;
}

void NullSimulation::stopSimulation() {}

sc_core::sc_simcontext* NullSimulation::context() const {
    static StoppedScSimcontext c;
    return &c;
}

ConfObjectRef NullSimulation::simics_object() const {
    return ConfObjectRef();
}

}  // namespace systemc
}  // namespace simics
