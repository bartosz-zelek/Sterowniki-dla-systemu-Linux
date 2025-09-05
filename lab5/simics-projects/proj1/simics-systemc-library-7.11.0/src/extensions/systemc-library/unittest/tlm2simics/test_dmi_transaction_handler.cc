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

#include <boost/test/unit_test.hpp>

#include <simics/systemc/tlm2simics/dmi_transaction_handler.h>
#include <simics/systemc/interface_provider.h>

#include "mock/iface/mock_receiver.h"
#include "mock/mock_interface_provider.h"
#include "mock/tlm2simics/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

using simics::systemc::InterfaceProvider;
using simics::systemc::iface::ReceiverInterface;
using simics::systemc::tlm2simics::GasketInterface;

BOOST_AUTO_TEST_SUITE(TestDmiTransactionHandler)

class MockInterface {
  public:
};

class MockTransactionHandler
    : public simics::systemc::tlm2simics::DmiTransactionHandler {
  public:
    MockTransactionHandler(InterfaceProvider *interface_provider,
                           ReceiverInterface *ignore_receiver,
                           InterfaceProvider *dmi_interface_provider)
        : DmiTransactionHandler(interface_provider,
                                ignore_receiver,
                                dmi_interface_provider),
          simics_transaction_cnt_(0), debug_transaction_cnt_(0) {
    }
    virtual tlm::tlm_response_status simics_transaction(
            simics::ConfObjectRef &simics_obj,  // NOLINT(runtime/references)
            tlm::tlm_generic_payload *trans) {
        ++simics_transaction_cnt_;
        return trans->get_response_status();
    }
    virtual unsigned int debug_transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                                           tlm::tlm_generic_payload *trans) {
        ++debug_transaction_cnt_;
        return debug_transaction_cnt_;
    }
    simics::systemc::iface::ReceiverInterface *receiver() override {
        return NULL;
    }

    int simics_transaction_cnt_;
    int debug_transaction_cnt_;
};

class TestEnvironment : public Environment  {
  public:
    TestEnvironment()
        : obj_(&conf_), dmi_provider_("MockInterface"),
          provider_("MockInterface"), mock_receiver_(new MockReceiver),
          handler_(&provider_, mock_receiver_, &dmi_provider_) {
        gasket_ = new unittest::tlm2simics::MockGasket();
        handler_.set_gasket(GasketInterface::Ptr(gasket_));
        provider_.set_target(obj_);
        dmi_provider_.set_target(obj_);
        register_interface("MockInterface", &mock_);
    }

    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    tlm::tlm_generic_payload transaction_;
    unittest::MockInterfaceProvider dmi_provider_;
    unittest::MockInterfaceProvider provider_;
    MockReceiver *mock_receiver_;
    MockTransactionHandler handler_;
    unittest::tlm2simics::MockGasket *gasket_;
    MockInterface mock_;
    Stubs stubs_;
};

BOOST_FIXTURE_TEST_CASE(TestSimicsTransaction, TestEnvironment) {
    gasket_->transaction_handler_->simics_transaction(obj_, &transaction_);
    BOOST_CHECK_EQUAL(handler_.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestDebugTransaction, TestEnvironment) {
    BOOST_CHECK_EQUAL(gasket_->transaction_handler_->debug_transaction(obj_,
        &transaction_), 1);
    BOOST_CHECK_EQUAL(handler_.debug_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestSimicsTransactionNullObject, TestEnvironment) {
    Stubs stubs;
    simics::ConfObjectRef obj;
    provider_.set_target(obj);
    gasket_->transaction_handler_->simics_transaction(obj_, &transaction_);
    BOOST_CHECK_EQUAL(handler_.simics_transaction_cnt_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 1);

    provider_.set_optional(true);
    gasket_->transaction_handler_->simics_transaction(obj_, &transaction_);
    BOOST_CHECK_EQUAL(handler_.simics_transaction_cnt_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestDebugTransactionNullObject, TestEnvironment) {
    Stubs stubs;
    simics::ConfObjectRef obj;
    provider_.set_target(obj);
    gasket_->transaction_handler_->debug_transaction(obj_, &transaction_);
    BOOST_CHECK_EQUAL(handler_.debug_transaction_cnt_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 1);

    provider_.set_optional(true);
    gasket_->transaction_handler_->debug_transaction(obj_, &transaction_);
    BOOST_CHECK_EQUAL(handler_.debug_transaction_cnt_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestGetDirectMemPtrNullObject, TestEnvironment) {
    Stubs stubs;
    tlm::tlm_dmi dmi_data;
    simics::ConfObjectRef obj;
    provider_.set_target(obj);
    BOOST_CHECK_EQUAL(gasket_->transaction_handler_->get_direct_mem_ptr(obj_,
        transaction_, dmi_data), false);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);

    provider_.set_optional(true);
    BOOST_CHECK_EQUAL(gasket_->transaction_handler_->get_direct_mem_ptr(obj_,
        transaction_, dmi_data), false);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestGetDirectMemPtrNoDirectMemoryInterface,
                        TestEnvironment) {
    Stubs stubs;
    tlm::tlm_dmi dmi_data;
    simics::ConfObjectRef obj;
    dmi_provider_.set_target(obj);
    BOOST_CHECK_EQUAL(gasket_->transaction_handler_->get_direct_mem_ptr(obj_,
        transaction_, dmi_data), false);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestGetDirectMemPtrNoDmiSupport, TestEnvironment) {
    register_interface("direct_memory", nullptr);

    tlm::tlm_dmi dmi_data;
    BOOST_CHECK_EQUAL(gasket_->transaction_handler_->get_direct_mem_ptr(obj_,
        transaction_, dmi_data), false);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 1);
    BOOST_CHECK_EQUAL(mock_receiver_->handle_count_, 0);
}

BOOST_AUTO_TEST_SUITE_END()
