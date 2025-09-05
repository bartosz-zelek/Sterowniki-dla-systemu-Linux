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

// MockSimulation.h

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_SIMULATION_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_SIMULATION_H

#include <boost/core/noncopyable.hpp>

#include <simics/base/log.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/kernel.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <unittest/simcontext_wrapper.h>
#include "mock_trace_monitor.h"

namespace unittest {

class MockProcessStack : public simics::systemc::ProcessStackInterface {
    void push() /* override */ {
    }
    void pop() /* override */ {
    }
    void waitForCurrentProcessOnTop() /* override */ {
    }
    void runUntilStackEmpty() /* override */ {
    }
    void waitForExitCondition(
        sc_core::sc_curr_proc_kind enter_process_kind) /* override */ {
    }
};

class MockSimulation : public simics::systemc::iface::SimulationInterface,
                       public simics::systemc::InternalInterface,
                       private boost::noncopyable {
  public:
    MockSimulation() : run_simulation_(0), acquire_kernel_(0) {
        conf_.instance_data = this;
    }
    ~MockSimulation() {
    }
    int runDeltaPhase(int count) /* override */ {
        simics::systemc::Kernel kernel(context());
        simics::systemc::KernelStateModifier modifier(this);
        int delta_count = 0;
        while (sc_core::sc_pending_activity_at_current_time()) {
            sc_core::sc_start(sc_core::SC_ZERO_TIME);
            ++delta_count;
            if (--count == 0)
                break;
        }
        return delta_count;
    }
    bool runSimulation(sc_core::sc_time t) /* override */ {
        simics::systemc::Kernel kernel(context());
        bool succeeded = true;
        if (sc_core::sc_get_status() != sc_core::SC_PAUSED)
            return false;

        simics::systemc::KernelStateModifier modifier(this);
        try {
            sc_core::sc_start(t);
        } catch(const sc_core::sc_report &e) {
            // The SC kernel transforms all exception types to sc_report object,
            // thus there are no other catch blocks here
            SIM_log_critical(&conf_, 1,
                             "exception caught in runSimulation(): %s",
                             e.what());
            // SIM_log_critical invokes stop on the simulation, which in turn
            // invokes sc_pause.
            sc_core::sc_pause();
            succeeded = false;
        }
        ++run_simulation_;
        return succeeded;
    }
    void stopSimulation() /* override */ {
    }
    sc_core::sc_simcontext *context() const /* override */ {
        ++acquire_kernel_;
        return context_.simcontext();
    }
    simics::ConfObjectRef simics_object() const /* override */ {
        return simics::ConfObjectRef(&conf_);
    }
    simics::systemc::TraceMonitorInterface *trace_monitor() /* override */ {
        return &monitor_;
    }
    simics::systemc::ProcessStackInterface *process_stack() /* override */ {
        return &process_stack_;
    }
    void pauseSimulationNoBreak() /* override */ {
        simics::systemc::Kernel kernel(context());
        sc_core::sc_pause();
    }

    mutable conf_object_t conf_;
    int run_simulation_;
    mutable int acquire_kernel_;
    mutable SimContextWrapper context_;
    MockTraceMonitor monitor_;
    MockProcessStack process_stack_;
};

}  // namespace unittest

#endif
