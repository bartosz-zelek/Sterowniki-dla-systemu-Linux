// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_TLM2SIMICS_TEST_GASKET_ENVIRONMENT_H
#define SYSTEMC_LIBRARY_UNITTEST_TLM2SIMICS_TEST_GASKET_ENVIRONMENT_H

#include <tlm>

#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/tlm2simics/transaction_handler_interface.h>

#include "environment.h"

template<class TGasket, class TExtension, class TSimicsInterfaceImpl>
class TestGasketEnvironment
    : public Environment,
      public simics::systemc::iface::ExtensionSenderInterface {
  public:
    explicit TestGasketEnvironment(const char* interface_name)
        : response_(tlm::TLM_INCOMPLETE_RESPONSE) {
        register_interface(interface_name, &interface_);
        device_.set_target(simulation_.simics_object());
        extension_.init(this);
    }
    virtual simics::systemc::iface::Transaction transaction() {
        return simics::systemc::iface::Transaction(&transaction_);
    }
    virtual void send_extension(
            simics::systemc::iface::Transaction *transaction) {
        simics::systemc::tlm2simics::TransactionHandlerInterface &handler =
            device_;
        simics::ConfObjectRef obj = simulation_.simics_object();
        response_ = handler.simics_transaction(obj, *transaction);
    }
    virtual void send_failed(simics::systemc::iface::Transaction *transaction) {
        BOOST_ERROR("Gasket did not dispatch extension.");
    }
    TGasket device_;
    TExtension extension_;
    TSimicsInterfaceImpl interface_;
    tlm::tlm_generic_payload transaction_;
    tlm::tlm_response_status response_;
};

#endif
