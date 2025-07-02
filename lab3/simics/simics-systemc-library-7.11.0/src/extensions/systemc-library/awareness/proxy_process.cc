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

#include <simics/systemc/adapter.h>
#include <simics/systemc/awareness/proxy_process.h>
#include <simics/systemc/context.h>
#include <simics/systemc/iface/instrumentation/process_action_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <simics/systemc/memory_profiling.h>
#include <simics/systemc/sim_context_proxy.h>
#include <simics/util/help-macros.h>  // STATIC_ASSERT

#include <simics/systemc/internals.h>

#include <string>
#include <functional>

STATIC_ASSERT(sizeof(uint64_t) >= sizeof(uintptr_t));  // NOLINT (runtime/sizeof)

namespace simics {
namespace systemc {
namespace awareness {

ProxyProcess::ProxyProcess(ConfObjectRef o)
    : Proxy(o), process_(NULL) {
}

void ProxyProcess::init(sc_core::sc_object *obj,
                        iface::SimulationInterface *simulation) {
    // Assert that object_ is a subclass of sc_core::sc_process_b:
    process_ = dynamic_cast<sc_core::sc_process_b*>(obj);
    FATAL_ERROR_IF(!process_, "Unable to cast to process_");

    Proxy::init(obj, simulation);
    connection_list_updated(ToolController::EMPTY);
}

attr_value_t ProxyProcess::events() const {
    Context context(this);
    EventVector events =
        sc_core::simContextProxy::get_timed_event_list(object_);

    attr_value_t result = SIM_alloc_attr_list(events.size());
    for (unsigned int i = 0; i < events.size(); ++i) {
        simics::systemc::Event e = events[i];
        SIM_attr_list_set_item(
            &result, i,
            SIM_make_attr_list(
                3,
                SIM_make_attr_uint64(e.when().value()),
                SIM_make_attr_string(e.name().c_str()),
                SIM_make_attr_string(e.kind().c_str())));
    }
    return result;
}

const char *ProxyProcess::file() const {
    return process_->file;
}

int ProxyProcess::line() const {
    return process_->lineno;
}

int ProxyProcess::process_id() const {
    return process_->proc_id;
}

char *ProxyProcess::dump_state() const {
    // The returned char* is malloced and lingers on, but must eventually be
    // freed using free().
    return MM_STRDUP(process_->dump_state().c_str());
}

bool ProxyProcess::initialize() const {
    return !process_->dont_initialize();
}

int ProxyProcess::state() const {
    return process_->current_state();
}

uint64_t ProxyProcess::min_time() const {
#if INTC_EXT
    return process_->get_process_profiler()->get_min_time();
#else
    return 0;  // user notified through ProcessProfilerControl::set_enabled()
#endif
}

uint64_t ProxyProcess::max_time() const {
#if INTC_EXT
    return process_->get_process_profiler()->get_max_time();
#else
    return 0;  // user notified through ProcessProfilerControl::set_enabled()
#endif
}

uint64_t ProxyProcess::total_time() const {
#if INTC_EXT
    return process_->get_process_profiler()->get_total_time();
#else
    return 0;  // user notified through ProcessProfilerControl::set_enabled()
#endif
}

uint64_t ProxyProcess::number_of_calls() const {
#if INTC_EXT
    return process_->get_process_profiler()->get_number_of_calls();
#else
    return 0;  // user notified through ProcessProfilerControl::set_enabled()
#endif
}

void ProxyProcess::run() {
#if INTC_EXT
    KernelStateModifier m(simulation());
    if (process_->current_state() & sc_core::sc_process_b::ps_bit_zombie) {
        SIM_LOG_INFO(1, simics_object(), 0,
                     "Process has ceased to exist. Cannot run it.");
        return;
    }

    if (process_->current_state() & sc_core::sc_process_b::ps_bit_disabled) {
        SIM_LOG_INFO(1, simics_object(), 0,
                     "Enabling disabled process.");
        sc_core::sc_process_handle h(process_);
        h.enable();
    }

    if (process_->current_state() & sc_core::sc_process_b::ps_bit_suspended) {
        SIM_LOG_INFO(1, simics_object(), 0,
                     "Resuming suspended process.");
        sc_core::sc_process_handle h(process_);
        h.resume();
    }

    sc_core::simContextProxy::trigger_static(process_);
    return;

#endif
    SIM_LOG_ERROR(simics_object(), 0,
                  "Process cannot be run. INTC extensions not enabled.");
}

void ProxyProcess::event_callback(const char *event_type,
                                  const char *class_type,
                                  void *object,
                                  const sc_core::sc_time &ts) {
    DISPATCH_TOOL_CHAIN_THIS(iface::instrumentation::ProcessActionInterface,
                             process_state_change,
                             this,
                             event_type, class_type, object,
                             const_cast<sc_core::sc_time *>(&ts),
                             false);
}

void ProxyProcess::connection_list_updated(ConnectionListState state) {
#if INTC_EXT
    Adapter *adapter = static_cast<Adapter *>(SIM_object_data(simics_object()));
    assert(adapter);
    InternalInterface *internal = adapter->internal();

    internal->trace_monitor()->subscribe(INTC_TRIGGER_PROCESS_ACTIVATION,
                                         static_cast<void*>(object_), this,
                                         state != ToolController::EMPTY);
#endif
}

attr_value_t ProxyProcess::allocations() const {
    AllocationMap *allocations = getMemoryProfiler()->allocations(process_);
    assert(allocations);

    attr_value_t attr = SIM_alloc_attr_list(allocations->size());
    typedef AllocationMap::iterator It;
    for (std::pair<It, std::size_t> p(allocations->begin(), 0);
         p.first != allocations->end(); ++p.first, ++p.second) {
        Allocation *allocation = &p.first->second;
        SIM_attr_list_set_item(
            &attr, p.second, SIM_make_attr_list(
                3,
                SIM_make_attr_uint64(
                    reinterpret_cast<uint64_t>(allocation->addr)),
                SIM_make_attr_uint64(allocation->size),
                SIM_make_attr_boolean(allocation->array_form)));
    }
    return attr;
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
