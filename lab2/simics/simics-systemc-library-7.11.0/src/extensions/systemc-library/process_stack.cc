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

#include <simics/systemc/process_stack.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <simics/util/help-macros.h>

namespace simics {
namespace systemc {

ProcessStack::ProcessStack(iface::SimulationInterface *simulation)
    : return_event_(sc_core::sc_event::kernel_event, "process_stack"),
      simulation_(simulation) {
}

void ProcessStack::push() {
    stack_.push(sc_core::sc_get_current_process_handle());
}

void ProcessStack::pop() {
    stack_.pop();
    KernelStateModifier m(simulation_);
    if (sc_core::sc_get_status() == sc_core::SC_RUNNING)
        return_event_.notify();
    else
        return_event_.notify(sc_core::SC_ZERO_TIME);
}

void ProcessStack::waitForCurrentProcessOnTop() {
    while (stack_.size()
           && stack_.top() != sc_core::sc_get_current_process_handle()) {
        sc_core::wait(return_event_);
    }
}

void ProcessStack::runUntilStackEmpty() {
    while (stack_.size() && sc_core::sc_pending_activity()) {
        if (!simulation_->runSimulation(sc_core::sc_time_to_pending_activity()))
            break;
    }
    if (stack_.size()) {
        // We cannot rely on log error here, as a broken C-semantic could cause
        // Simics to segfault before showing the error message - hence the use
        // of FATAL_ERROR (and it avoids the need of a conf object)
        FATAL_ERROR("Dead-lock detected: no more pending activity while process"
                    " stack is not empty.");
    }
}

// Wait for exit condition, unwind the process stack and preserve FILO order
void ProcessStack::waitForExitCondition(
        sc_core::sc_curr_proc_kind enter_process_kind) {
    switch (enter_process_kind) {
    case sc_core::SC_THREAD_PROC_:
        // fall through
    case sc_core::SC_CTHREAD_PROC_:
        waitForCurrentProcessOnTop();
        break;
    case sc_core::SC_NO_PROC_:
        // No thread => cannot wait, must run SC forward to unwind the stack
        // No process => run until stack is empty (same as simics event entry)
        runUntilStackEmpty();
        break;
    case sc_core::SC_METHOD_PROC_:
        // Method process => re-entry means stack will never be empty
        // On the other hand, SC guarantees that wait() is not called from
        // method process so we can simply ignore the stack here as it will
        // unwind correctly through direct function calls.
        break;
    }
}

}  // namespace systemc
}  // namespace simics
