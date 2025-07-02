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

#include <boost/test/unit_test.hpp>

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/iface/pci_express_extension.h>
#include "mock/iface/mock_pci_express.h"
#undef SIMICS_6_API
#endif
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/extension_dispatcher.h>

#include "simcontext_wrapper.h"

#include <vector>

BOOST_AUTO_TEST_SUITE(TestPciExpressExtension_standalone)

namespace sif = simics::systemc::iface;

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") {
        sender_.init(&initiator);
        extension_.init(&sender_);

        dispatcher_.subscribe(
            sif::PciExpressExtension::createReceiver(&device_));

        target.register_b_transport(this, &TestModule::b_transport);
    }

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        dispatcher_.handle(&trans);
    }

    // sockets
    tlm_utils::simple_initiator_socket<TestModule> initiator;
    tlm_utils::simple_target_socket<TestModule> target;

    // sender and extension
    sif::ExtensionSender<
        tlm_utils::simple_initiator_socket<TestModule> > sender_;
    sif::PciExpressExtension extension_;

    // receiver and interface implementation
    MockPciExpress device_;
    sif::ExtensionDispatcher dispatcher_;
};

class TestEnvironment {
  public:
    unittest::SimContextWrapper context_;
};

BOOST_FIXTURE_TEST_CASE(testScMain, TestEnvironment) {
    TestModule producer("producer");
    TestModule consumer("consumer");

    producer.initiator.bind(consumer.target);
    consumer.initiator.bind(producer.target);

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(consumer.device_.send_message_, 0);
    producer.extension_.send_message(8, std::vector<uint8_t>());
    BOOST_CHECK_EQUAL(consumer.device_.send_message_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
