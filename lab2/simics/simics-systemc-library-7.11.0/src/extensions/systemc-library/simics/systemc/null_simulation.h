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

#ifndef SIMICS_SYSTEMC_NULL_SIMULATION_H
#define SIMICS_SYSTEMC_NULL_SIMULATION_H

#include <systemc>


#include <simics/systemc/iface/simulation_interface.h>

namespace simics {
namespace systemc {

/** \internal */
class NullSimulation : public iface::SimulationInterface {
  public:
    // SimulationInterface
    int runDeltaPhase(int count);
    bool runSimulation(sc_core::sc_time t);
    void stopSimulation();
    sc_core::sc_simcontext* context() const;
    ConfObjectRef simics_object() const;
};

}  // namespace systemc
}  // namespace simics

#endif
