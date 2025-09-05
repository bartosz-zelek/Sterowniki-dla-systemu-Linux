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

#include <simics/systemc/simics2tlm/gasket_factory.h>

#include "environment.h"
#include "mock/mock_target_socket.h"
#include "mock/mock_simulation.h"

BOOST_AUTO_TEST_SUITE(TestGasketFactory)

class TestEnvironment : public Environment {
  public:
    template<typename DEVICE>
    void createGasket(DEVICE *device) {
        const simics::ConfObjectRef obj(simulation_.simics_object());
        gasket_ = simics::systemc::simics2tlm::createGasket(
            &device->target_socket, obj);
    }
    void createGasketByName(std::string name) {
        const simics::ConfObjectRef obj(simulation_.simics_object());
        gasket_ = simics::systemc::simics2tlm::createGasketByName(name, obj);
    }

    simics::systemc::simics2tlm::GasketInterface::Ptr gasket_;
};

BOOST_FIXTURE_TEST_CASE(testCreateGasketForTlmTargetSocket,
                        TestEnvironment) {
    unittest::MockTlmTargetSocket<> device("mock_s_f_1");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_1_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForTlmTargetSocketByName,
                        TestEnvironment) {
    simics::systemc::simics2tlm::GasketFactory<32,
        tlm::tlm_base_protocol_types> factory;

    unittest::MockTlmTargetSocket<> device("mock_s_f_1");
    createGasketByName("mock_s_f_1.target_socket");

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_1_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForTlmTargetSocketParams,
                        TestEnvironment) {
    unittest::MockTlmTargetSocket<
        16, tlm::tlm_base_protocol_types, 2,
        sc_core::SC_ZERO_OR_MORE_BOUND> device("mock_s_f_1_1");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_1_1_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForSimpleTargetSocket,
                        TestEnvironment) {
    unittest::MockSimpleTargetSocket<> device("mock_s_f_2");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_2_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForSimpleTargetSocketTagged,
                        TestEnvironment) {
    unittest::MockSimpleTargetSocketTagged<> device("mock_s_f_3");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_3_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForPassthroughTargetSocket,
                        TestEnvironment) {
    unittest::MockPassthroughTargetSocket<> device("mock_s_f_4");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_4_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForPassthroughTargetSocketTagged,
                        TestEnvironment) {
    unittest::MockPassthroughTargetSocketTagged<> device("mock_s_f_5");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_5_target_socket", object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForMultiPassthroughTargetSocket,
                        TestEnvironment) {
    unittest::MockMultiPassthroughTargetSocket<> device("mock_s_f_6");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_6_target_socket",
                      object->basename());
}

BOOST_FIXTURE_TEST_CASE(testCreateGasketForMultiPassthroughTargetSocketParams,
                        TestEnvironment) {
    unittest::MockMultiPassthroughTargetSocket<
        16, tlm::tlm_base_protocol_types, 2,
        sc_core::SC_ZERO_OR_MORE_BOUND> device("mock_s_f_6_1");
    createGasket(&device);

    sc_core::sc_object *object = dynamic_cast<sc_core::sc_object*>(
            gasket_.get());
    BOOST_REQUIRE(object != NULL);
    BOOST_CHECK_EQUAL("gasket_mock_s_f_6_1_target_socket",
                      object->basename());
}

BOOST_AUTO_TEST_SUITE_END()
