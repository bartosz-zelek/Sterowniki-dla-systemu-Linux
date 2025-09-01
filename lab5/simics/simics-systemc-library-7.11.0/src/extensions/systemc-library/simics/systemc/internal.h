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

#ifndef SIMICS_SYSTEMC_INTERNAL_H
#define SIMICS_SYSTEMC_INTERNAL_H

#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/process_stack.h>
#include <simics/systemc/trace_monitor.h>

namespace simics {
namespace systemc {

class Internal : public InternalInterface {
  public:
    explicit Internal(iface::SimulationInterface *simulation);
    // InternalInterface (internal)
    TraceMonitorInterface *trace_monitor(); /* override */
    ProcessStackInterface *process_stack(); /* override */
    void pauseSimulationNoBreak(); /* override */

  private:
    TraceMonitor trace_monitor_;
    ProcessStack process_stack_;
};

}  // namespace systemc
}  // namespace simics

#endif
