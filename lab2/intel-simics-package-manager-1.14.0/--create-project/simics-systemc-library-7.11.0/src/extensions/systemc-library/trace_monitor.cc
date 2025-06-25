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

#include <simics/systemc/trace_monitor.h>
#include <simics/systemc/internals.h>
#if INTC_EXT
#include <sysc/intc/kernel/event_callback_interface.h>
#endif
#include <systemc>

#include <map>
#include <string>
#include <utility>

namespace simics {
namespace systemc {

#if INTC_EXT
intc::EventType subscriptionToKernelEvent(const char *subscription) {
    std::string str(subscription);
    if (str == INTC_TRIGGER_PROCESS_ACTIVATION)
        return intc::INTC_EVENT_PROCESS_ACTIVATION;

    if (str == INTC_TRIGGER_EVENT_TRIGGERED)
        return intc::INTC_EVENT_EVENT_TRIGGERED;

    if (str == INTC_TRIGGER_ASYNC_REQUEST_UPDATE)
        return intc::INTC_EVENT_ASYNC_REQUEST_UPDATE;

    if (str == INTC_TRIGGER_DELTA_CYCLE_COMPLETED)
        return intc::INTC_EVENT_DELTA_CYCLE_COMPLETED;

    assert(false);
    // Added return to keep Windows build happy.
    return intc::INTC_EVENT_DELTA_CYCLE_COMPLETED;
}

const char* kernelEventToSubscription(intc::EventType event_type) {
    switch (event_type) {
        case intc::INTC_EVENT_PROCESS_ACTIVATION: {
            return INTC_TRIGGER_PROCESS_ACTIVATION;
        }
        break;
        case intc::INTC_EVENT_EVENT_TRIGGERED: {
            return INTC_TRIGGER_EVENT_TRIGGERED;
        }
        break;
        case intc::INTC_EVENT_ASYNC_REQUEST_UPDATE: {
            return INTC_TRIGGER_ASYNC_REQUEST_UPDATE;
        }
        break;
        case intc::INTC_EVENT_DELTA_CYCLE_COMPLETED: {
            return INTC_TRIGGER_DELTA_CYCLE_COMPLETED;
        }
        break;
        default:
            assert(false);
    }

    // Added return to keep Windows build happy.
    return INTC_TRIGGER_DELTA_CYCLE_COMPLETED;
}

class EventCallbackAdapter : public intc::event_callback_interface {
  public:
    explicit EventCallbackAdapter(TraceMonitorInterface *callback)
        : callback_(callback) {
        sc_core::sc_get_curr_simcontext()->set_event_callback(this);
    }
    virtual ~EventCallbackAdapter() {
        sc_core::sc_get_curr_simcontext()->remove_event_callback();
    }
    virtual void event_callback(intc::EventType event_type,
                                const char *event_class_type,
                                void *event_object,
                                const sc_core::sc_time &ts) {
        callback_->kernel_callback(event_type,
                                   kernelEventToSubscription(event_type),
                                   event_class_type,
                                   event_object,
                                   ts);
    }

  private:
    TraceMonitorInterface *callback_;
};
#endif

TraceMonitor::TraceMonitor() : adapter_(NULL) {
#if INTC_EXT
    adapter_ = new EventCallbackAdapter(this);
#endif
}

TraceMonitor::~TraceMonitor() {
#if INTC_EXT
    delete adapter_;
#endif
}

void TraceMonitor::subscribeAllDynamic(const char *event_type,
                                       EventCallbackInterface *callback) {
#if INTC_EXT
    int event_id = subscriptionToKernelEvent(event_type);
    trace_dynamic_callbacks_[event_id] = callback;
    add_callback(event_id, callback);
#endif
}

void TraceMonitor::subscribe(const char *event_type,
                             const void *obj,
                             EventCallbackInterface *callback,
                             bool trace) {
#if INTC_EXT
    int event_id = subscriptionToKernelEvent(event_type);
    trace_callbacks_[obj] = entry(callback, trace);
    if (trace)
        add_callback(event_id, callback);
    else
        remove_callback(event_id, callback);
#endif
}

void TraceMonitor::unsubscribeAllDynamic(const char *event_type,
                                         EventCallbackInterface *callback) {
#if INTC_EXT
    int event_id = subscriptionToKernelEvent(event_type);
    trace_dynamic_callbacks_.erase(event_id);
    remove_callback(event_id, callback);
#endif
}

void TraceMonitor::kernel_callback(int kernel_event_type,
                                   const char *event_type,
                                   const char *class_type,
                                   void *object,
                                   const sc_core::sc_time &ts) {
    if (is_event_traced_.find(kernel_event_type) == is_event_traced_.end())
        return;

#if INTC_EXT
    if (kernel_event_type == intc::INTC_EVENT_EVENT_TRIGGERED) {
        sc_core::sc_event *e = static_cast<sc_core::sc_event *>(object);
        if (sc_core::sc_get_curr_simcontext()->is_kernel_event(e))
            return;
    }
#endif
    std::map<const void *, entry>::iterator i = trace_callbacks_.find(object);
    if (i != trace_callbacks_.end()) {
        if (i->second.second)
            i->second.first->event_callback(event_type, class_type, object, ts);

        return;
    }

    std::map<int, EventCallbackInterface *>::iterator j;
    j = trace_dynamic_callbacks_.find(kernel_event_type);
    if (j != trace_dynamic_callbacks_.end()) {
        j->second->event_callback(event_type, class_type, object, ts);
    }
}

void TraceMonitor::add_callback(int event_id,
                                EventCallbackInterface *callback) {
    trace_all_callbacks_.emplace(event_id, callback);
    is_event_traced_.insert(event_id);
}

void TraceMonitor::remove_callback(int event_id,
                                   EventCallbackInterface *callback) {
    std::pair<std::multimap<int, EventCallbackInterface *>::iterator,
              std::multimap<int, EventCallbackInterface *>::iterator> range;
    range = trace_all_callbacks_.equal_range(event_id);
    std::multimap<int, EventCallbackInterface *>::iterator it;
    for (it = range.first; it != range.second; ++it) {
        if (it->second == callback) {
            trace_all_callbacks_.erase(it);
            break;
        }
    }

    if (trace_all_callbacks_.count(event_id) <= 0)
        is_event_traced_.erase(event_id);
}

}  // namespace systemc
}  // namespace simics
