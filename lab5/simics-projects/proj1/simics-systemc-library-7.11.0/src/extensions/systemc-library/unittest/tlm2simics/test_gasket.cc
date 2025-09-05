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

#include <systemc>
#include <tlm>
#include <simics/systemc/tlm2simics/gasket.h>
#include <simics/systemc/tlm2simics/null_transaction_handler.h>

#include "mock/mock_simulation.h"
#include "mock/mock_initiator_socket.h"
#include "mock/tlm2simics/mock_transaction_handler.h"
#include "simcontext_wrapper.h"


BOOST_AUTO_TEST_SUITE(TesttTlm2SimicsGasket)

// Todo(onergardx): This should probably be refactored to a separate
// class. in the unittest/mock directory.

class MockScThread : public sc_core::sc_module {
  public:
    typedef MockScThread SC_CURRENT_USER_MODULE;
    MockScThread() :
        sc_core::sc_module(sc_core::sc_module_name("MockScThread")), count_(0),
        delay_(sc_core::sc_time(1, sc_core::SC_PS)), manual_(false) {
        SC_THREAD(loop);
        sensitive << event_;
    }
    void trigger_event() {
        manual_ = true;
        event_.notify(delay_);
    }
    void loop() {
        while (true) {
            if (manual_ == false) {
                event_.notify(delay_);
            }
            wait();
            ++count_;
            action();
        }
    }
    virtual void action() {
    }
    sc_core::sc_event event_;
    int count_;
    sc_core::sc_time delay_;
    bool manual_;
};

class MockScThreadSendTransaction : public MockScThread {
  public:
    explicit MockScThreadSendTransaction(sc_core::sc_module_name
                                          = "MockScThreadSendTransaction") {
    }
    void action() /* override */ {
        device_->initiator_socket->b_transport(*transaction_,
                                               transaction_delay_);
    }
    sc_core::sc_time transaction_delay_;
    tlm::tlm_generic_payload *transaction_;
    unittest::MockTlmInitiatorSocket<> *device_;
};

class TestEnvironment {
  public:
    TestEnvironment()
        : device_("MockDevice"),
          gasket_("Gasket", simulation_.simics_object()) {
        gasket_.init(&simulation_);
        gasket_.bind(device_.initiator_socket);
    }
    void set_transaction_handler() {
        gasket_.set_transaction_handler(&transactionHandler_);
    }
    void register_nb_transport_fw() {
        gasket_.register_nb_transport_fw();
    }

    unittest::MockSimulation simulation_;
    MockScThreadSendTransaction thread_;
    unittest::MockTlmInitiatorSocket<> device_;
    simics::systemc::tlm2simics::Gasket<> gasket_;
    unittest::tlm2simics::MockTransactionHandler transactionHandler_;
    tlm::tlm_generic_payload transaction_;
};

BOOST_FIXTURE_TEST_CASE(TestSimicsTransaction, TestEnvironment) {
    set_transaction_handler();
    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    device_.initiator_socket->b_transport(transaction_, t);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler_.update_dmi_allowed_cnt_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestSimicsTransactionWithWait, TestEnvironment) {
    set_transaction_handler();
    thread_.manual_ = true;
    thread_.device_ = &device_;
    thread_.transaction_ = &transaction_;
    thread_.transaction_delay_ = sc_core::sc_time(10, sc_core::SC_NS);
    thread_.trigger_event();
    sc_core::sc_start(9, sc_core::SC_NS);
    BOOST_CHECK_EQUAL(thread_.count_, 1);
    // the transaction is still waiting to complete.
    BOOST_CHECK_EQUAL(thread_.transaction_delay_,
                      sc_core::sc_time(10, sc_core::SC_NS));
    sc_core::sc_start(2, sc_core::SC_NS);
    BOOST_CHECK_EQUAL(sc_core::sc_time_stamp(),
                      sc_core::sc_time(11, sc_core::SC_NS));
    // the transaction completed at 10000 and the gasket synchronizes the
    // local time and clears the time parameter before sending the transaction
    // to Simics side.
    BOOST_CHECK_EQUAL(thread_.transaction_delay_, sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler_.update_dmi_allowed_cnt_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestDebugTransaction, TestEnvironment) {
    set_transaction_handler();
    int bytes = device_.initiator_socket->transport_dbg(transaction_);
    BOOST_CHECK_EQUAL(transactionHandler_.debug_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(bytes, 1);
}

BOOST_FIXTURE_TEST_CASE(TestDMI, TestEnvironment) {
    set_transaction_handler();
    tlm::tlm_dmi dmi = tlm::tlm_dmi();

    BOOST_CHECK_EQUAL(
        device_.initiator_socket->get_direct_mem_ptr(transaction_, dmi), true);
    BOOST_CHECK_EQUAL(transactionHandler_.get_direct_mem_ptr_cnt_, 1);

    transactionHandler_.get_direct_mem_ptr_ret_ = false;
    BOOST_CHECK_EQUAL(
        device_.initiator_socket->get_direct_mem_ptr(transaction_, dmi), false);
    BOOST_CHECK_EQUAL(transactionHandler_.get_direct_mem_ptr_cnt_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestNullTransactionHandlerCount, TestEnvironment) {
    BOOST_CHECK_EQUAL(
        simics::systemc::tlm2simics::NullTransactionHandler::instances(), 1);
    set_transaction_handler();
    BOOST_CHECK_EQUAL(
        simics::systemc::tlm2simics::NullTransactionHandler::instances(), 0);
}

BOOST_FIXTURE_TEST_CASE(TestByteEnableSupport, TestEnvironment) {
    set_transaction_handler();
    unsigned char enabled[0] = {};
    transaction_.set_byte_enable_ptr(enabled);

    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    device_.initiator_socket->b_transport(transaction_, t);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 0);
    BOOST_CHECK_EQUAL(transactionHandler_.byte_enable_supported_cnt_, 1);

    transactionHandler_.byte_enable_supported_ret_ = true;

    device_.initiator_socket->b_transport(transaction_, t);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler_.byte_enable_supported_cnt_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestNbTransaction, TestEnvironment) {
    set_transaction_handler();
    register_nb_transport_fw();
    tlm::tlm_phase p = tlm::BEGIN_REQ;
    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    device_.initiator_socket->nb_transport_fw(transaction_, p, t);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler_.update_dmi_allowed_cnt_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestNbTransactionResponse, TestEnvironment) {
    set_transaction_handler();
    register_nb_transport_fw();
    tlm::tlm_phase p = tlm::END_RESP;
    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    auto ret = device_.initiator_socket->nb_transport_fw(transaction_, p, t);
    BOOST_CHECK_EQUAL(ret, tlm::TLM_COMPLETED);
    BOOST_CHECK_EQUAL(transaction_.get_response_status(),
                      tlm::TLM_OK_RESPONSE);

    // Invalid phase
    p = tlm::END_REQ;
    ret = device_.initiator_socket->nb_transport_fw(transaction_, p, t);
    BOOST_CHECK_EQUAL(ret, tlm::TLM_COMPLETED);
    BOOST_CHECK_EQUAL(transaction_.get_response_status(),
                      tlm::TLM_COMMAND_ERROR_RESPONSE);
    p = tlm::BEGIN_RESP;
    ret = device_.initiator_socket->nb_transport_fw(transaction_, p, t);
    BOOST_CHECK_EQUAL(ret, tlm::TLM_COMPLETED);
    BOOST_CHECK_EQUAL(transaction_.get_response_status(),
                      tlm::TLM_COMMAND_ERROR_RESPONSE);
}

BOOST_AUTO_TEST_SUITE_END()
