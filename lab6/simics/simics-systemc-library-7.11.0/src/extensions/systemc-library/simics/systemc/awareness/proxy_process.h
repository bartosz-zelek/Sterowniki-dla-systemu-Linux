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

#ifndef SIMICS_SYSTEMC_AWARENESS_PROXY_PROCESS_H
#define SIMICS_SYSTEMC_AWARENESS_PROXY_PROCESS_H



#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/iface/sc_memory_profiler_interface.h>
#include <simics/systemc/iface/sc_process_interface.h>
#include <simics/systemc/iface/sc_process_profiler_interface.h>
#include <simics/systemc/instrumentation/tool_controller_process_action.h>

namespace simics {
namespace systemc {
namespace awareness {

/** \internal
 * Class ProxyProcess responsible for mapping class sc_process to ConfObject.
 */
class ProxyProcess : public Proxy,
                     public iface::ScMemoryProfilerInterface,
                     public iface::ScProcessInterface,
                     public iface::ScProcessProfilerInterface,
                     public instrumentation::ToolControllerProcessAction,
                     public EventCallbackInterface {
  public:
    explicit ProxyProcess(simics::ConfObjectRef o);
    virtual void init(sc_core::sc_object *obj,
                      iface::SimulationInterface *simulation);

    // ScMemoryProfilerInterface
    attr_value_t allocations() const;

    // ScProcessInterface
    attr_value_t events() const;
    const char *file() const;
    int line() const;
    int process_id() const;
    char *dump_state() const;
    bool initialize() const;
    int state() const;
    const char *name() const;
    void run();

    // ScProcessProfilerInterface
    uint64_t min_time() const;
    uint64_t max_time() const;
    uint64_t total_time() const;
    uint64_t number_of_calls() const;

    // TraceMonitorCallback
    virtual void event_callback(const char *event_type,
                                const char *event_class_type,
                                void *event_object,
                                const sc_core::sc_time &ts);

    virtual void connection_list_updated(ConnectionListState state);

  private:
    sc_core::sc_process_b *process_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
