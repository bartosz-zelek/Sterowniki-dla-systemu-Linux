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

#ifndef SIMICS_SYSTEMC_SIMULATION_INTERFACE_PROXY_H
#define SIMICS_SYSTEMC_SIMULATION_INTERFACE_PROXY_H

#include <simics/systemc/iface/simulation_interface.h>

#include <systemc>

namespace simics {
namespace systemc {

/** Interface to the SystemC simulation. */
class SimulationInterfaceProxy : public iface::SimulationInterface {
  public:
    SimulationInterfaceProxy() : simulation_(NULL) {
    }

    void setSimulation(iface::SimulationInterface *simulation) {
        simulation_ = simulation;
    }

    // SimulationInterface
    virtual int runDeltaPhase(int count) {
        return simulation_->runDeltaPhase(count);
    }
    virtual bool runSimulation(sc_core::sc_time t) {
        return simulation_->runSimulation(t);
    }
    virtual void stopSimulation() {
        return simulation_->stopSimulation();
    }
    virtual sc_core::sc_simcontext *context() const {
        return simulation_->context();
    }
    virtual ConfObjectRef simics_object() const {
        if (simulation_) {
            return simulation_->simics_object();
        } else {
            return nullptr;
        }
    }

  private:
    iface::SimulationInterface *simulation_;
};

}  // namespace systemc
}  // namespace simics

#endif
