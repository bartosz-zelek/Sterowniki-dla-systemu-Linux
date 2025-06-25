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

#ifndef SIMICS_SYSTEMC_SIMCONTEXT_H
#define SIMICS_SYSTEMC_SIMCONTEXT_H

#include <simics/base/attr-value.h>
#include <simics/systemc/iface/simcontext_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

namespace simics {
namespace systemc {

/**
 * Provides the interface to the SystemC simulation context.
 * Handles methods to retrieve information about the simulation status
 * and timing schedule. */
class SimContext : public iface::SimContextInterface {
  public:
    explicit SimContext(iface::SimulationInterface *simulation)
        : simulation_(simulation) { }
    virtual uint64 time_stamp();
    virtual uint64 delta_count();
    virtual uint64 time_to_pending_activity();
    virtual int status();
    virtual attr_value_t events();
  private:
    iface::SimulationInterface *simulation_;
};

}  // namespace systemc
}  // namespace simics

#endif
