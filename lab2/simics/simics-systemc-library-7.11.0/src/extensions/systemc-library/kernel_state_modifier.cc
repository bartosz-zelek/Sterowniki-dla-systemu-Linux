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

#include <simics/base/log.h>
#include <simics/systemc/adapter_log_groups.h>  // Log_Scheduling
#include <simics/systemc/kernel_state_comparator.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <unordered_map>
#include <utility>  // move

namespace simics {
namespace systemc {

std::unordered_map<sc_core::sc_simcontext *,
                   KernelStateComparator> KernelStateModifier::comparators_;

KernelStateModifier::KernelStateModifier(
        iface::SimulationInterface *simulation) {
    // Elaboration not done yet.
    if (sc_core::sc_get_status() != sc_core::SC_PAUSED) {
        // Remove state of a former simulation. This is mainly to pass tests
        // where consecutive sc_simcontext instances end up at the same
        // address in memory.
        comparators_.erase(sc_core::sc_curr_simcontext);
        return;
    }

    auto i = comparators_.find(sc_core::sc_curr_simcontext);
    if (i == comparators_.end())
        return;

    KernelStateComparator k;
    if (i->second != k) {
        SIM_LOG_ERROR(simulation->simics_object(), Log_Scheduling,
                      "SystemC kernel state has been changed outside supported "
                      "entry points.");
        // Skip consecutive logs for the current state.
        i->second = std::move(k);
    }
}

KernelStateModifier::~KernelStateModifier() {
    // Elaboration not done yet.
    if (sc_core::sc_get_status() != sc_core::SC_PAUSED)
        return;

    comparators_[sc_core::sc_curr_simcontext] = KernelStateComparator();
}

}  // namespace systemc
}  // namespace simics
