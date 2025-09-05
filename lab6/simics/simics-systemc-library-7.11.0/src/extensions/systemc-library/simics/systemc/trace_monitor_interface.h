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

#ifndef SIMICS_SYSTEMC_TRACE_MONITOR_INTERFACE_H
#define SIMICS_SYSTEMC_TRACE_MONITOR_INTERFACE_H

#include <systemc>

namespace simics {
namespace systemc {

#define INTC_TRIGGER_PROCESS_ACTIVATION    "process activation"
#define INTC_TRIGGER_EVENT_TRIGGERED       "event triggered"
#define INTC_TRIGGER_ASYNC_REQUEST_UPDATE  "async_request_update"
#define INTC_TRIGGER_DELTA_CYCLE_COMPLETED "delta_cycle_completed"

/** \internal */
class EventCallbackInterface {
  public:
    virtual ~EventCallbackInterface() {}
    virtual void event_callback(const char *event_type,
                                const char *class_type,
                                void *object,
                                const sc_core::sc_time &ts) = 0;
};

/** \internal */
class TraceMonitorInterface {
  public:
    virtual void subscribeAllDynamic(const char *event_type,
                                     EventCallbackInterface *callback) = 0;
    virtual void subscribe(const char *event_type,
                           const void *obj,
                           EventCallbackInterface *callback,
                           bool trace) = 0;
    virtual void unsubscribeAllDynamic(const char *event_type,
                                       EventCallbackInterface *callback) = 0;
    virtual void kernel_callback(int kernel_event_type,
                                 const char *event_type,
                                 const char *event_class_type,
                                 void *event_object,
                                 const sc_core::sc_time &ts) = 0;
    virtual ~TraceMonitorInterface() {}
};

}  // namespace systemc
}  // namespace simics

#endif

