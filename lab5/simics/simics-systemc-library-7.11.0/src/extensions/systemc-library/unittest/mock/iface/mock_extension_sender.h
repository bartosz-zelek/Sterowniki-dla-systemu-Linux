// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_EXTENSION_SENDER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_EXTENSION_SENDER_H

#include <tlm>

#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/iface/transaction.h>

class MockExtensionSender
    : public simics::systemc::iface::ExtensionSenderInterface {
  public:
    explicit MockExtensionSender(tlm::tlm_generic_payload *payload)
        : receiver_(NULL), payload_(payload), send_failed_cnt_(0) {}
    void set_receiver(simics::systemc::iface::ReceiverInterface *receiver) {
        receiver_ = receiver;
    }
    virtual simics::systemc::iface::Transaction transaction() {
        return simics::systemc::iface::Transaction(payload_);
    }
    virtual void send_extension(
            simics::systemc::iface::Transaction *transaction) {
        if (receiver_)
            receiver_->handle(*transaction);
    }
    virtual void send_failed(simics::systemc::iface::Transaction *transaction) {
        ++send_failed_cnt_;
    }
    simics::systemc::iface::ReceiverInterface *receiver_;
    tlm::tlm_generic_payload *payload_;
    int send_failed_cnt_;
};

#endif
