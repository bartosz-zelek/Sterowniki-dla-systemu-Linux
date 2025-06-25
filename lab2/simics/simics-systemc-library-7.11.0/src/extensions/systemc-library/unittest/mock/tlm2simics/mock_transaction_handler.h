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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_TLM2SIMICS_MOCK_TRANSACTION_HANDLER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_TLM2SIMICS_MOCK_TRANSACTION_HANDLER_H

#include <simics/systemc/tlm2simics/transaction_handler_interface.h>

#include "mock/iface/mock_receiver.h"

namespace unittest {
namespace tlm2simics {

class MockTransactionHandler
    : public simics::systemc::tlm2simics::TransactionHandlerInterface {
  public:
    MockTransactionHandler()
        : get_direct_mem_ptr_cnt_(0), get_direct_mem_ptr_ret_(true),
          simics_transaction_cnt_(0), debug_transaction_cnt_(0),
          update_dmi_allowed_cnt_(0), byte_enable_supported_cnt_(0),
          byte_enable_supported_ret_(false) {
    }
    void set_target(const simics::ConfObjectRef &obj) {
        obj_ = obj;
    }
    const simics::ConfObjectRef &target() const {
        return obj_;
    }
    bool get_direct_mem_ptr(simics::ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload &trans,
                            tlm::tlm_dmi& dmi_data) override {
        ++get_direct_mem_ptr_cnt_;
        return get_direct_mem_ptr_ret_;
    }
    tlm::tlm_response_status simics_transaction(
            simics::ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override {
        ++simics_transaction_cnt_;
        return trans->get_response_status();
    }
    unsigned int debug_transaction(simics::ConfObjectRef &simics_obj,
                                   tlm::tlm_generic_payload *trans) override {
        return ++debug_transaction_cnt_;
    }
    void update_dmi_allowed(simics::ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload *trans) override {
        ++update_dmi_allowed_cnt_;
    }
    bool byte_enable_supported(simics::ConfObjectRef &simics_obj,
                               tlm::tlm_generic_payload *trans) override {
        ++byte_enable_supported_cnt_;
        return byte_enable_supported_ret_;
    }
    const simics::systemc::tlm2simics::GasketOwner *
    gasket_owner() const override {
        return NULL;
    }
    const simics::systemc::InterfaceProvider *
    interface_provider() const override {
        return NULL;
    }
    simics::systemc::iface::ReceiverInterface *receiver() override {
        return &receiver_;
    }

    int get_direct_mem_ptr_cnt_;
    bool get_direct_mem_ptr_ret_;
    int simics_transaction_cnt_;
    int debug_transaction_cnt_;
    int update_dmi_allowed_cnt_;
    int byte_enable_supported_cnt_;
    bool byte_enable_supported_ret_;
    simics::ConfObjectRef obj_;
    MockReceiver receiver_;
};

}  // namespace tlm2simics
}  // namespace unittest

#endif
