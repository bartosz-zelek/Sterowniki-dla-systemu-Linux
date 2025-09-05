// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT

#include <boost/test/unit_test.hpp>

#include <simics/systemc/awareness/tlm_spy_factory.h>
#include <simics/systemc/awareness/tlm_spy_factory_registry.h>

#include <unittest/simcontext_wrapper.h>
#include <unittest/mock/mock_initiator_socket.h>
#include <unittest/mock/mock_target_socket.h>

using simics::systemc::awareness::TlmSpyFactory;
using simics::systemc::awareness::TlmSpyFactoryRegistry;

BOOST_AUTO_TEST_SUITE(TestTlmSpyFactoryRegistry)

class TestEnvironment {
  public:
    TestEnvironment() {
        initiator_ = new unittest::MockTlmInitiatorSocket<> ("TestInitiator");
        target_ = new unittest::MockTlmTargetSocket<>("TestTarget");
        target_->target_socket(initiator_->initiator_socket);
    }
    ~TestEnvironment() {
        delete target_;
        delete initiator_;
    }

    unittest::SimContextWrapper wrapper;
    unittest::MockTlmInitiatorSocket<> *initiator_;
    unittest::MockTlmTargetSocket<> *target_;
};

BOOST_FIXTURE_TEST_CASE(testRegistryCleanUp, TestEnvironment) {
    TlmSpyFactoryRegistry static_registry;
    static_registry.createSpyFactory<tlm::tlm_base_protocol_types>();

    tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> *if1 =
        initiator_->initiator_socket.get_base_port().operator->();
    tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> *if2 =
        initiator_->initiator_socket.get_base_export().operator->();
    tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> *if3 =
        target_->target_socket.get_base_port().operator->();
    tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> *if4 =
        target_->target_socket.get_base_export().operator->();

    TlmSpyFactoryRegistry adapter_registry = static_registry;

    adapter_registry.traverseAll();

    tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> *spy_if1 =
        initiator_->initiator_socket.get_base_port().operator->();
    tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> *spy_if2 =
        initiator_->initiator_socket.get_base_export().operator->();
    tlm::tlm_bw_transport_if<tlm::tlm_base_protocol_types> *spy_if3 =
        target_->target_socket.get_base_port().operator->();
    tlm::tlm_fw_transport_if<tlm::tlm_base_protocol_types> *spy_if4 =
        target_->target_socket.get_base_export().operator->();

    BOOST_CHECK(if1 != spy_if1);
    BOOST_CHECK(if2 != spy_if2);
    BOOST_CHECK(if3 != spy_if3);
    BOOST_CHECK(if4 != spy_if4);
}

BOOST_AUTO_TEST_SUITE_END()
#endif  // INTC_EXT
