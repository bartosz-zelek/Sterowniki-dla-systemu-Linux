// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/adapter.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/trace_event_all_dynamic.h>

namespace simics {
namespace systemc {

void TraceEventAllDynamic::connection_list_updated(ConnectionListState state) {
#if INTC_EXT
    Adapter *adapter = static_cast<Adapter *>(SIM_object_data(simics_object()));
    InternalInterface *internal = adapter->internal();
    assert(internal);

    if (state != EMPTY) {
        internal->trace_monitor()->subscribeAllDynamic(
            INTC_TRIGGER_EVENT_TRIGGERED, this);
    } else {
        internal->trace_monitor()->unsubscribeAllDynamic(
            INTC_TRIGGER_EVENT_TRIGGERED, this);
    }
#endif
}

}  // namespace systemc
}  // namespace simics
