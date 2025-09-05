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

#include <simics/systemc/tlm2simics/gasket_factory.h>
#include <simics/systemc/tlm2simics/transaction_handler_interface.h>
#include <simics/systemc/simulation_interface_proxy.h>

#include "mock/mock_simulation.h"
#include "mock/mock_initiator_socket.h"
#include "mock/tlm2simics/mock_transaction_handler.h"

BOOST_AUTO_TEST_SUITE(TesttTlm2SimicsGasketFactory)

using simics::systemc::tlm2simics::GasketInterface;
using simics::systemc::tlm2simics::Gasket;
using simics::systemc::tlm2simics::TransactionHandlerInterface;

class TestEnvironment {
  public:
    template<unsigned int BUSWIDTH, typename TYPES,
             int N, sc_core::sc_port_policy POL>
    void initInternal(
            tlm::tlm_initiator_socket<BUSWIDTH, TYPES, N,
                                      POL> *initiator_socket,
            GasketInterface::Ptr gasket) {
        Gasket<BUSWIDTH, TYPES> *impl = dynamic_cast<Gasket<BUSWIDTH, TYPES> *>(
                gasket.get());
        impl->init(&simulation_);
    }
    virtual ~TestEnvironment() {
        simics::systemc::tlm2simics::GasketDispatcherBase::cleanCache();
    }
    template<typename DEVICE>
    void createGasket(DEVICE *device) {
        gasket_ = simics::systemc::tlm2simics::createGasket(
            &device->initiator_socket, simulation_.simics_object());
        gasket_->set_transaction_handler(&transactionHandler_);
        initInternal<>(&device->initiator_socket, gasket_);
        sc_core::sc_start(0, sc_core::SC_PS);
    }
    void createGasketByName(std::string name) {
        gasket_ = simics::systemc::tlm2simics::createGasketByName(name,
            simulation_.simics_object());
        gasket_->set_transaction_handler(&transactionHandler_);
        initInternal<32, tlm::tlm_base_protocol_types, 1,
                     sc_core::SC_ONE_OR_MORE_BOUND>(NULL, gasket_);
        sc_core::sc_start(0, sc_core::SC_PS);
    }
    unittest::MockSimulation simulation_;
    sc_core::sc_time t_;
    GasketInterface::Ptr gasket_;
    unittest::tlm2simics::MockTransactionHandler transactionHandler_;
    tlm::tlm_generic_payload transaction_;
};

BOOST_FIXTURE_TEST_CASE(testGasketTlmInitiatorSocket, TestEnvironment) {
    unittest::MockTlmInitiatorSocket<> device("mock_t_f_1");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_1_initiator_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketTlmInitiatorSocketByName, TestEnvironment) {
    simics::systemc::tlm2simics::GasketFactory<32,
        tlm::tlm_base_protocol_types> factory;

    unittest::MockTlmInitiatorSocket<> device("mock_t_f_1");
    createGasketByName("mock_t_f_1.initiator_socket");

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_1_initiator_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketTlmInitiatorSocketParams, TestEnvironment) {
    unittest::MockTlmInitiatorSocket<
        16, tlm::tlm_base_protocol_types, 2,
        sc_core::SC_ZERO_OR_MORE_BOUND> device("mock_t_f_1_1");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_1_1_initiator_socket",
                      object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketSimpleInitiatorSocket, TestEnvironment) {
    unittest::MockSimpleInitiatorSocket<> device("mock_t_f_2");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_2_initiator_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketSimpleInitiatorSocketTagged,
                        TestEnvironment) {
    unittest::MockSimpleInitiatorSocketTagged<> device("mock_t_f_3");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_3_initiator_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketMultiPassthroughInitiatorSocket,
                        TestEnvironment) {
    unittest::MockMultiPassthroughInitiatorSocket<> device("mock_t_f_4");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_4_initiator_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testGasketMultiPassthroughInitiatorSocketParams,
                        TestEnvironment) {
    unittest::MockMultiPassthroughInitiatorSocket<
        16, tlm::tlm_base_protocol_types, 2,
        sc_core::SC_ZERO_OR_MORE_BOUND> device("mock_t_f_4_1");
    createGasket(&device);

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler_.simics_transaction_cnt_, 1);
    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_4_1_initiator_socket",
                      object->basename());
}

BOOST_FIXTURE_TEST_CASE(testMultiGasketTlmInitiatorSocket, TestEnvironment) {
    unittest::MockTlmInitiatorSocket<> device("mock_t_f_1");
    GasketInterface::Ptr gasket1 =
            simics::systemc::tlm2simics::createMultiGasket(
                    &device.initiator_socket, simulation_.simics_object());

    GasketInterface::Ptr gasket2 =
            simics::systemc::tlm2simics::createMultiGasket(
                    &device.initiator_socket, simulation_.simics_object());

    unittest::tlm2simics::MockTransactionHandler transactionHandler1;
    unittest::tlm2simics::MockTransactionHandler transactionHandler2;

    gasket1->set_transaction_handler(&transactionHandler1);
    gasket2->set_transaction_handler(&transactionHandler2);

    initInternal<>(&device.initiator_socket, gasket1);
    initInternal<>(&device.initiator_socket, gasket2);

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    sc_core::sc_object *object1 = dynamic_cast<sc_core::sc_object*>(
            gasket1.get());
    BOOST_REQUIRE(object1 != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_1_initiator_socket",
                      object1->basename());

    sc_core::sc_object *object2 = dynamic_cast<sc_core::sc_object*>(
            gasket2.get());
    BOOST_REQUIRE(object2 != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_t_f_1_initiator_socket_0",
                      object2->basename());

    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler1.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler2.simics_transaction_cnt_, 0);

    transactionHandler2.receiver_.probe_return_ = true;
    device.initiator_socket->b_transport(transaction_, t_);
    BOOST_CHECK_EQUAL(transactionHandler1.simics_transaction_cnt_, 1);
    BOOST_CHECK_EQUAL(transactionHandler2.simics_transaction_cnt_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
