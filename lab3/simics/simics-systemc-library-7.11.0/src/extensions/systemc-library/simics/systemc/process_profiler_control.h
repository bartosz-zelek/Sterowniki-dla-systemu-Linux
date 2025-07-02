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

#ifndef SIMICS_SYSTEMC_PROCESS_PROFILER_CONTROL_H
#define SIMICS_SYSTEMC_PROCESS_PROFILER_CONTROL_H

#include <simics/systemc/iface/sc_process_profiler_control_interface.h>
#include <simics/systemc/iface/simulation_interface.h>

namespace simics {
namespace systemc {

class ProcessProfilerControl : public iface::ScProcessProfilerControlInterface {
  public:
    explicit ProcessProfilerControl(iface::SimulationInterface *simulation):
        simulation_(simulation) {}
    virtual bool is_enabled();
    virtual void set_enabled(bool enable);
    virtual uint64_t total_time();
    virtual uint64_t total_number_of_calls();
    virtual void clear_data();
  private:
    iface::SimulationInterface *simulation_;
};

}  // namespace systemc
}  // namespace simics

#endif
