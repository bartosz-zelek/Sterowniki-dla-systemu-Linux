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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_EVENT_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_EVENT_H

#include <systemc>
#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/iface/sc_event_interface.h>
#include <simics/systemc/instrumentation/tool_controller_event_action.h>
#include <simics/systemc/trace_monitor_interface.h>

namespace simics {
namespace systemc {
namespace awareness {

class ProxyEvent : public Proxy,
                   public instrumentation::ToolControllerEventAction,
                   public EventCallbackInterface,
                   public iface::ScEventInterface {
  public:
    explicit ProxyEvent(ConfObjectRef o);
    virtual void allProxiesInitialized();
    // EventCallbackInterface
    virtual void event_callback(const char *event_type,
                                const char *class_type,
                                void *object,
                                const sc_core::sc_time &ts);
    virtual void connection_list_updated(ConnectionListState state);
    // ScEventInterface
    virtual void notify();
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
