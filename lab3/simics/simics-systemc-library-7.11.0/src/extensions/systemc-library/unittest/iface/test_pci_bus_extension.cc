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

#include <simics/systemc/iface/pci_bus_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_pci_bus.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestPciBusExtension)

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_ = simics::systemc::iface::PciBusExtension::createReceiver(
            &bus_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockPciBus bus_;
    simics::systemc::iface::PciBusExtension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testRaiseInterrupt, TestEnvironment) {
    extension_.raise_interrupt(2);
    BOOST_CHECK_EQUAL(bus_.raise_interrupt_, 1);
    BOOST_CHECK_EQUAL(bus_.raise_interrupt_pin_, 2);
}

BOOST_FIXTURE_TEST_CASE(testLowerInterrupt, TestEnvironment) {
    extension_.lower_interrupt(3);
    BOOST_CHECK_EQUAL(bus_.lower_interrupt_, 1);
    BOOST_CHECK_EQUAL(bus_.lower_interrupt_pin_, 3);
}

BOOST_FIXTURE_TEST_CASE(testInterruptAcknowledge, TestEnvironment) {
    bus_.interrupt_acknowledge_return_ = 2;
    BOOST_CHECK_EQUAL(extension_.interrupt_acknowledge(), 2);
    BOOST_CHECK_EQUAL(bus_.interrupt_acknowledge_, 1);
}

BOOST_FIXTURE_TEST_CASE(testAddMap, TestEnvironment) {
    simics::types::map_info_t info;
    info.base = 4;

    bus_.add_map_return_ = 3;
    BOOST_CHECK_EQUAL(extension_.add_map(
        simics::types::Sim_Addr_Space_IO, info), 3);
    BOOST_CHECK_EQUAL(bus_.add_map_, 1);
    BOOST_CHECK_EQUAL(bus_.add_map_space_, simics::types::Sim_Addr_Space_IO);
    BOOST_CHECK_EQUAL(bus_.add_map_info_.base, 4);
}

BOOST_FIXTURE_TEST_CASE(testRemoveMap, TestEnvironment) {
    bus_.remove_map_return_ = 4;
    BOOST_CHECK_EQUAL(extension_.remove_map(
        simics::types::Sim_Addr_Space_IO, 1), 4);
    BOOST_CHECK_EQUAL(bus_.remove_map_, 1);
    BOOST_CHECK_EQUAL(bus_.remove_map_space_, simics::types::Sim_Addr_Space_IO);
    BOOST_CHECK_EQUAL(bus_.remove_map_function_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSetBusNumber, TestEnvironment) {
    extension_.set_bus_number(2);
    BOOST_CHECK_EQUAL(bus_.set_bus_number_, 1);
    BOOST_CHECK_EQUAL(bus_.set_bus_number_bus_id_, 2);
}

BOOST_FIXTURE_TEST_CASE(testSetSubBusNumber, TestEnvironment) {
    extension_.set_sub_bus_number(12);
    BOOST_CHECK_EQUAL(bus_.set_sub_bus_number_, 1);
    BOOST_CHECK_EQUAL(bus_.set_sub_bus_number_bus_id_, 12);
}

BOOST_FIXTURE_TEST_CASE(testAddDefault, TestEnvironment) {
    simics::types::map_info_t info;
    info.base = 10;
    extension_.add_default(simics::types::Sim_Addr_Space_IO, info);
    BOOST_CHECK_EQUAL(bus_.add_default_, 1);
    BOOST_CHECK_EQUAL(bus_.add_default_space_,
                      simics::types::Sim_Addr_Space_IO);
    BOOST_CHECK_EQUAL(bus_.add_default_info_.base, 10);
}

BOOST_FIXTURE_TEST_CASE(testRemoveDefault, TestEnvironment) {
    extension_.remove_default(simics::types::Sim_Addr_Space_IO);
    BOOST_CHECK_EQUAL(bus_.remove_default_, 1);
    BOOST_CHECK_EQUAL(bus_.remove_default_space_,
                      simics::types::Sim_Addr_Space_IO);
}

BOOST_FIXTURE_TEST_CASE(testBusReset, TestEnvironment) {
    extension_.bus_reset();
    BOOST_CHECK_EQUAL(bus_.bus_reset_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSpecialCycle, TestEnvironment) {
    extension_.special_cycle(2);
    BOOST_CHECK_EQUAL(bus_.special_cycle_, 1);
    BOOST_CHECK_EQUAL(bus_.special_cycle_value_, 2);
}

BOOST_FIXTURE_TEST_CASE(testSystemError, TestEnvironment) {
    extension_.system_error();
    BOOST_CHECK_EQUAL(bus_.system_error_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGetBusAddress, TestEnvironment) {
    bus_.get_bus_address_return_ = 5;
    BOOST_CHECK_EQUAL(extension_.get_bus_address(), 5);
    BOOST_CHECK_EQUAL(bus_.get_bus_address_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSetDeviceStatus, TestEnvironment) {
    extension_.set_device_status(2, 3, 4);
    BOOST_CHECK_EQUAL(bus_.set_device_status_, 1);
    BOOST_CHECK_EQUAL(bus_.set_device_status_device_, 2);
    BOOST_CHECK_EQUAL(bus_.set_device_status_function_, 3);
    BOOST_CHECK_EQUAL(bus_.set_device_status_enabled_, 4);
}

BOOST_AUTO_TEST_SUITE_END()
