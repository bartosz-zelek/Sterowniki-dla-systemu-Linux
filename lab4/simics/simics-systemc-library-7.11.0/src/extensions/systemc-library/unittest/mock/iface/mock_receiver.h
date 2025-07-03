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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_RECEIVER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_RECEIVER_H

#include <simics/systemc/iface/receiver_interface.h>

class MockReceiver : public simics::systemc::iface::ReceiverInterface {
  public:
    MockReceiver() : handle_count_(0), probe_return_(false) {}
    bool handle(tlm::tlm_generic_payload *payload) override {
        ++handle_count_;
        return true;
    }
    bool probe(tlm::tlm_generic_payload *payload) override {
        return probe_return_;
    }

    int handle_count_;
    bool probe_return_;
};

#endif
