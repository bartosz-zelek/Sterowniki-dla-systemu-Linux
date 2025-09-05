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

#include <simics/systemc/iface/pci_device_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_pci_device.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestPciDeviceExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_ = simics::systemc::iface::PciDeviceExtension::createReceiver(
            &device_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockPciDevice device_;
    simics::systemc::iface::PciDeviceExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testBusReset, TestEnvironment) {
    extension_.bus_reset();
    BOOST_CHECK_EQUAL(device_.bus_reset_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSystemError, TestEnvironment) {
    extension_.system_error();
    BOOST_CHECK_EQUAL(device_.system_error_, 1);
}

BOOST_FIXTURE_TEST_CASE(testInterruptRaised, TestEnvironment) {
    extension_.interrupt_raised(2);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_pin_, 2);
}

BOOST_FIXTURE_TEST_CASE(testInterruptLowered, TestEnvironment) {
    extension_.interrupt_lowered(3);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_pin_, 3);
}

BOOST_FIXTURE_TEST_CASE(testExtensionReuse, TestEnvironment) {
    extension_.interrupt_raised(2);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_pin_, 2);

    extension_.interrupt_lowered(3);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_pin_, 3);
}

BOOST_AUTO_TEST_SUITE_END()
