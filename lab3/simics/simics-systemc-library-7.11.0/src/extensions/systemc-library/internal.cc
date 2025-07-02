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

#include <simics/systemc/internal.h>
#include <simics/systemc/sim_context_proxy.h>

namespace simics {
namespace systemc {

Internal::Internal(iface::SimulationInterface *simulation)
    : process_stack_(simulation) {
}

TraceMonitorInterface *Internal::trace_monitor() {
    return &trace_monitor_;
}

ProcessStackInterface *Internal::process_stack() {
    return &process_stack_;
}

void Internal::pauseSimulationNoBreak() {
    sc_core::simContextProxy::set_pause_pending(true);
}

}  // namespace systemc
}  // namespace simics
