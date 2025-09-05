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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_SIMICS2TLM_MOCK_GASKET_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_SIMICS2TLM_MOCK_GASKET_H

#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>

#include <string>

namespace unittest {
namespace simics2tlm {

class MockGasket : public simics::systemc::simics2tlm::GasketInterface {
  public:
    MockGasket()
        : trigger_transaction_(0), transaction_result_(true),
          transaction_status_(tlm::TLM_INCOMPLETE_RESPONSE), type_(NULL),
          receiver_(NULL), dmi_(true) {
    }
    void set_receiver(simics::systemc::iface::ReceiverInterface *receiver) {
        receiver_ = receiver;
    }
    virtual bool trigger_transaction() {
        simics::systemc::iface::Transaction t(&transaction_);
        if (receiver_)
            receiver_->handle(t);
        ++trigger_transaction_;
        if (!transaction_result_)
            transaction_.set_response_status(transaction_status_);
        return transaction_result_;
    }
    virtual bool trigger(simics::systemc::iface::Transaction *transaction) {
        if (receiver_)
            receiver_->handle(*transaction);
        ++trigger_transaction_;
        if (!transaction_result_)
            (*transaction)->set_response_status(transaction_status_);
        return transaction_result_;
    }
    virtual tlm::tlm_generic_payload &payload() {
        return transaction_;
    }
    virtual simics::ConfObjectRef &simics_obj() {
        return object_;
    }
    virtual void set_inquiry(bool inquiry) {
    }
    virtual simics::systemc::simics2tlm::DmiDataTable *get_dmi_data_table() {
        return &dmi_data_table_;
    }
    virtual void set_type(simics::systemc::ClassType *type) {
        type_ = type;
    }
    virtual simics::systemc::ClassType *type() {
        return type_;
    }
    virtual sc_core::sc_object *get_target_socket() {
        return NULL;
    }
    virtual void set_dmi(bool enable) {
        dmi_ = enable;
    }
    virtual bool is_dmi_enabled() {
        return dmi_;
    }
    std::string gasket_name() const override {
        return "MockGasket";
    }

    int trigger_transaction_;
    bool transaction_result_;
    tlm::tlm_response_status transaction_status_;  // only if result is false
    simics::ConfObjectRef object_;
    tlm::tlm_generic_payload transaction_;
    simics::systemc::simics2tlm::DmiDataTable dmi_data_table_;
    simics::systemc::ClassType *type_;
    simics::systemc::iface::ReceiverInterface *receiver_;
    bool dmi_;
};

}  // namespace simics2tlm
}  // namespace unittest

#endif
