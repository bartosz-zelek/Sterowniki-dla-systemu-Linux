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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TRACE_MONITOR_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TRACE_MONITOR_H

#include <simics/systemc/trace_monitor_interface.h>

namespace unittest {

class MockTraceMonitor : public simics::systemc::TraceMonitorInterface {
  public:
    virtual void subscribeAllDynamic(const char *event_type,
        simics::systemc::EventCallbackInterface *callback) {
    }
    virtual void subscribe(const char *event_type,
                           const void *obj,
                           simics::systemc::EventCallbackInterface *callback,
                           bool trace) {
    }
    virtual void unsubscribeAllDynamic(
            const char *event_type,
            simics::systemc::EventCallbackInterface *callback) {
    }
    virtual void kernel_callback(int kernel_event_type,
                                     const char *event_type,
                                     const char *event_class_type,
                                     void *event_object,
                                     const sc_core::sc_time &ts) {
    }
};

}  // namespace unittest

#endif
