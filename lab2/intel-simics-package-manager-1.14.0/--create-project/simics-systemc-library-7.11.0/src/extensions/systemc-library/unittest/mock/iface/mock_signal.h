// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_SIGNAL_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_SIGNAL_H

#include <simics/systemc/iface/signal_interface.h>

namespace unittest {
namespace iface {

class MockSignal : public simics::systemc::iface::SignalInterface {
  public:
    MockSignal() : raise_(0), lower_(0), proc_(0), out_ctx_(0), in_ctx_(0) {
    }
    virtual void raise() {
        ++raise_;
        out_ctx_ = sc_core::sc_get_curr_simcontext();
        if (in_ctx_) {
            proc_ = sc_core::sc_process_handle(
                    in_ctx_->get_curr_proc_info()->process_handle).proc_kind();
        }
    }
    virtual void lower() {
        ++lower_;
        out_ctx_ = sc_core::sc_get_curr_simcontext();
        if (in_ctx_) {
            proc_ = sc_core::sc_process_handle(
                    in_ctx_->get_curr_proc_info()->process_handle).proc_kind();
        }
    }
    int raise_;
    int lower_;
    int proc_;
    sc_core::sc_simcontext *out_ctx_;  // context going out = NullSimulation
    sc_core::sc_simcontext *in_ctx_;  // context on the inside
};

}  // namespace iface
}  // namespace unittest

#endif
