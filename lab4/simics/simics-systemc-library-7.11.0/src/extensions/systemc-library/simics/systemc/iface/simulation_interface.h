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

#ifndef SIMICS_SYSTEMC_IFACE_SIMULATION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SIMULATION_INTERFACE_H

#include <systemc>
#include <simics/conf-object.h>

namespace simics {
namespace systemc {
namespace iface {

/** Interface to the SystemC simulation. */
class SimulationInterface {
  public:
    virtual ~SimulationInterface() {}
    virtual int runDeltaPhase(int count) = 0;
    virtual bool runSimulation(sc_core::sc_time t) = 0;
    virtual void stopSimulation() = 0;
    virtual sc_core::sc_simcontext *context() const = 0;
    virtual ConfObjectRef simics_object() const = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
