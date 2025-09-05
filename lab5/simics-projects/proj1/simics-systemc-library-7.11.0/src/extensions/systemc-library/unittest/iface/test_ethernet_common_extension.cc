// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/iface/ethernet_common_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_ethernet_common.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestEthernetCommonExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_
            = simics::systemc::iface::EthernetCommonExtension::createReceiver(
                &device_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockEthDevice device_;
    simics::systemc::iface::EthernetCommonExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testFrame, TestEnvironment) {
    BOOST_CHECK_EQUAL(device_.frame_, 0);
    BOOST_CHECK_EQUAL(device_.frame_crc_ok_, 0);
    simics::types::frags_t frame;
    unsigned char frame_data = 0x79;
    frags_init_add(&frame, &frame_data, 1);

    extension_.frame(&frame, 1);

    BOOST_CHECK_EQUAL(device_.frame_, 1);
    BOOST_CHECK_EQUAL(device_.frame_frame_, &frame);
    unsigned char received_data;
    simics::types::frags_extract(device_.frame_frame_, &received_data);
    BOOST_CHECK_EQUAL(received_data, 0x79);
    BOOST_CHECK_EQUAL(device_.frame_crc_ok_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
