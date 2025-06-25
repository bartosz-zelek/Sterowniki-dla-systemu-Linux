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

#ifndef SIMICS_SYSTEMC_TRACE_MONITOR_H
#define SIMICS_SYSTEMC_TRACE_MONITOR_H

#include <systemc>
#include <simics/systemc/trace_monitor_interface.h>

#include <set>
#include <map>
#include <utility>

namespace simics {
namespace systemc {

class EventCallbackAdapter;
class EventCallbackInterface;

/** \internal */
class TraceMonitor : public TraceMonitorInterface {
  public:
    TraceMonitor();
    TraceMonitor(const TraceMonitor&) = delete;
    TraceMonitor& operator=(const TraceMonitor&) = delete;
    virtual ~TraceMonitor();
    virtual void subscribeAllDynamic(const char *event_type,
                                     EventCallbackInterface *callback);
    virtual void subscribe(const char *event_type, const void *obj,
                           EventCallbackInterface *callback,
                           bool trace);
    virtual void unsubscribeAllDynamic(const char *event_type,
                                       EventCallbackInterface *callback);
    virtual void kernel_callback(int kernel_event_type,
                                 const char *event_type,
                                 const char *event_class_type,
                                 void *event_object,
                                 const sc_core::sc_time &ts);

  private:
    typedef std::pair<EventCallbackInterface *, bool> entry;

    void add_callback(int event_id, EventCallbackInterface *callback);
    void remove_callback(int event_id, EventCallbackInterface *callback);

    // Mapping between event/process object and proxy callback (not dynamic).
    // This map is also used to filter objects for the dynamic callbacks.
    // If the object is part of this map, no dynamic callback is invoked.
    std::map<const void *, entry> trace_callbacks_;
    // Mapping between kernel event ID and dynamic events/processes callbacks.
    // These callbacks are invoked for dynamically created events and process
    // that do not have a proxy object.
    std::map<int, EventCallbackInterface *> trace_dynamic_callbacks_;
    // This map contains entries for enabled callbacks (trace=true) and
    // dynamic callbacks. If the map does not contain an entry for an kernel
    // event ID, no tool is connected and the event can just be ignored.
    std::multimap<int, EventCallbackInterface *> trace_all_callbacks_;
    // Set to do early return in the kernel event callback (optimization to
    // avoid count on multimap).
    std::set<int> is_event_traced_;
    EventCallbackAdapter *adapter_;
};

}  // namespace systemc
}  // namespace simics

#endif
