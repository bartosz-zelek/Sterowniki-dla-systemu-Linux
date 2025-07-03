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

#ifndef SIMICS_SYSTEMC_KERNEL_STATE_COMPARATOR_H
#define SIMICS_SYSTEMC_KERNEL_STATE_COMPARATOR_H

#include <systemc>

namespace simics {
namespace systemc {

/** \internal
 *
 * A class for comparing the state of the SystemC kernel.
 */
class KernelStateComparator {
  public:
    KernelStateComparator();
    bool operator==(const KernelStateComparator &rhs) const;
    bool operator!=(const KernelStateComparator &rhs) const;

  private:
    sc_core::sc_time sc_time_to_pending_activity_;
    int get_timed_event_list_size_;
    sc_dt::uint64 sc_delta_count_;
};

}  // namespace systemc
}  // namespace simics

#endif
