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

#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/iface/pci_express_extension.h>
#include "mock/iface/mock_pci_express.h"
#undef SIMICS_6_API
#endif
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestPciExpressExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_ = simics::systemc::iface::PciExpressExtension::createReceiver(
            &device_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockPciExpress device_;
    simics::systemc::iface::PciExpressExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testSendMessage, TestEnvironment) {
    device_.send_message_return_ = 11;
    int type = 0x20;
    std::vector<uint8_t> payload {8};

    BOOST_CHECK_EQUAL(extension_.send_message(type, payload), 11);
    BOOST_CHECK_EQUAL(device_.type_, type);
    BOOST_CHECK(device_.payload_.size() == payload.size());
    BOOST_CHECK(device_.payload_.at(0) == payload.at(0));
}

BOOST_AUTO_TEST_SUITE_END()
