// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/iface/pcie_device_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_pcie_device.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestPcieDeviceExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_ = simics::systemc::iface::PcieDeviceExtension::createReceiver(
            &device_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockPcieDevice device_;
    simics::systemc::iface::PcieDeviceExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
    conf_object_t conf_;
};

BOOST_FIXTURE_TEST_CASE(testConnected, TestEnvironment) {
    extension_.connected(&conf_, 0xbeef);
    BOOST_CHECK_EQUAL(device_.connected_, 1);
    BOOST_CHECK_EQUAL(device_.connected_port_obj_, &conf_);
    BOOST_CHECK_EQUAL(device_.connected_device_id_, 0xbeef);
}

BOOST_FIXTURE_TEST_CASE(testDisconnected, TestEnvironment) {
    extension_.disconnected(&conf_, 0xbeef);
    BOOST_CHECK_EQUAL(device_.disconnected_, 1);
    BOOST_CHECK_EQUAL(device_.disconnected_port_obj_, &conf_);
    BOOST_CHECK_EQUAL(device_.disconnected_device_id_, 0xbeef);
}

BOOST_FIXTURE_TEST_CASE(testHotReset, TestEnvironment) {
    extension_.hot_reset();
    BOOST_CHECK_EQUAL(device_.hot_reset_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
