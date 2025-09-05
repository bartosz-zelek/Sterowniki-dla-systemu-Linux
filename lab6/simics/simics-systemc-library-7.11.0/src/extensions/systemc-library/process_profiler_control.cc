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

#include <simics/systemc/process_profiler_control.h>
#include <simics/systemc/simcontext.h>
#include <simics/systemc/context.h>

#include <sysc/kernel/sc_simcontext.h>

namespace simics {
namespace systemc {

bool ProcessProfilerControl::is_enabled() {
#if INTC_EXT
    Context ctxt(simulation_);
    return ctxt->get_process_profiler_enabled();
#else
    return false;
#endif
}

void ProcessProfilerControl::set_enabled(bool enable) {
#if INTC_EXT
    Context ctxt(simulation_);
    ctxt->set_process_profiler_enabled(enable);
#else
    SIM_LOG_INFO(1, simulation_->simics_object(), 0,
                 "Kernel not built with INTC_EXT=1,"
                 " process profiler not available");
#endif
}

uint64_t ProcessProfilerControl::total_time() {
#if INTC_EXT
    Context ctxt(simulation_);
    return ctxt->get_process_profilers_total_time();
#else
    SIM_LOG_INFO(1, simulation_->simics_object(), 0,
                 "Kernel not built with INTC_EXT=1,"
                 " process profiler not available");
    return 0;
#endif
}

uint64_t ProcessProfilerControl::total_number_of_calls() {
#if INTC_EXT
    Context ctxt(simulation_);
    return ctxt->get_process_profilers_total_number_of_calls();
#else
    SIM_LOG_INFO(1, simulation_->simics_object(), 0,
                 "Kernel not built with INTC_EXT=1,"
                 " process profiler not available");
    return 0;
#endif
}

void ProcessProfilerControl::clear_data() {
#if INTC_EXT
    Context ctxt(simulation_);
    ctxt->clear_process_profiler_data();
#else
    SIM_LOG_INFO(1, simulation_->simics_object(), 0,
                 "Kernel not built with INTC_EXT=1,"
                 " process profiler not available");
#endif
}

}  // namespace systemc
}  // namespace simics

