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

#include <simics/systemc/simcontext.h>
#include <simics/systemc/sim_context_proxy.h>
#include <simics/systemc/context.h>

#include <sysc/kernel/sc_simcontext.h>

namespace simics {
namespace systemc {

uint64 SimContext::time_stamp() {
    Context ctxt(simulation_);
    return ctxt->time_stamp().value();
}
uint64 SimContext::delta_count() {
    Context ctxt(simulation_);
    return sc_core::sc_delta_count();  // context::delta_count() is deprecated
}
uint64 SimContext::time_to_pending_activity() {
    Context ctxt(simulation_);
    return sc_core::sc_time_to_pending_activity().value();
}
int SimContext::status() {
    Context ctxt(simulation_);
    return ctxt->get_status();
}

attr_value_t SimContext::events() {
    Context ctxt(simulation_);
    simics::systemc::EventVector events;
    events = sc_core::simContextProxy::get_timed_event_list(NULL);
    int n = events.size();
    attr_value_t val = SIM_alloc_attr_list(n);
    for (int i = 0; i < n; ++i) {
        simics::systemc::Event e = events[i];
        SIM_attr_list_set_item(
            &val, i,
            SIM_make_attr_list(
                4,
                SIM_make_attr_object(simulation_->simics_object()),
                SIM_make_attr_string(e.what().c_str()),
                SIM_make_attr_uint64(e.when().value()),
                SIM_make_attr_string("")));
    }
    return val;
}

}  // namespace systemc
}  // namespace simics

