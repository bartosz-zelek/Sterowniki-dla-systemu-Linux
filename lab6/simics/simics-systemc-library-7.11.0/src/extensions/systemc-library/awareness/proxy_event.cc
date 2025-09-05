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
#include <simics/systemc/awareness/proxy_event.h>
#include <simics/systemc/iface/instrumentation/event_action_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <string>
#include <sstream>

namespace simics {
namespace systemc {
namespace awareness {

ProxyEvent::ProxyEvent(ConfObjectRef o) : Proxy(o) {}

void ProxyEvent::allProxiesInitialized() {
    connection_list_updated(ToolController::EMPTY);
}

void ProxyEvent::event_callback(const char *event_type,
                                const char *class_type,
                                void *object,
                                const sc_core::sc_time &ts) {
    DISPATCH_TOOL_CHAIN_THIS(iface::instrumentation::EventActionInterface,
                             triggered, this,
                             event_type, class_type, object,
                             const_cast<sc_core::sc_time *>(&ts));
}

void ProxyEvent::connection_list_updated(ConnectionListState state) {
    ScEventObject *event = dynamic_cast<ScEventObject *>(object_);
    if (!event)
        return;

#if INTC_EXT
    Adapter *adapter = static_cast<Adapter *>(SIM_object_data(simics_object()));
    assert(adapter);
    InternalInterface *internal = adapter->internal();

    internal->trace_monitor()->subscribe(
        INTC_TRIGGER_EVENT_TRIGGERED,
        static_cast<void*>(event->get_event()),
        this, state != ToolController::EMPTY);
#endif
}

void ProxyEvent::notify() {
    KernelStateModifier m(simulation());
    if (ScEventObject *event = dynamic_cast<ScEventObject *>(object_)) {
        // When invoked from Simics CLI, we assume kernel is not running and
        // thus immediate notification is not allowed. Notify with ZERO_TIME
        // instead.
        event->get_event()->notify(sc_core::SC_ZERO_TIME);
    }
}

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
