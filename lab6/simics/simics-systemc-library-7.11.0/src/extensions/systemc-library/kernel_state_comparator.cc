// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/kernel_state_comparator.h>
#include <simics/systemc/sim_context_proxy.h>

namespace simics {
namespace systemc {

KernelStateComparator::KernelStateComparator() {
    sc_time_to_pending_activity_ =
        sc_core::simContextProxy::sc_time_to_pending_activity_ignore_async();
    get_timed_event_list_size_ =
        sc_core::simContextProxy::get_timed_event_list_size();
    sc_delta_count_ = sc_core::sc_delta_count();
}

bool KernelStateComparator::operator==(const KernelStateComparator &rhs) const {
    return (sc_time_to_pending_activity_ == rhs.sc_time_to_pending_activity_ &&
            get_timed_event_list_size_ == rhs.get_timed_event_list_size_ &&
            sc_delta_count_ == rhs.sc_delta_count_);
}
bool KernelStateComparator::operator!=(const KernelStateComparator &rhs) const {
    return !(*this == rhs);
}

}  // namespace systemc
}  // namespace simics
