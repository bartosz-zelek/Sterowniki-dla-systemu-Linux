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

#include <simics/base/types.h>


#include <simics/systemc/simics2tlm/pci_device.h>
#include <simics/systemc/simics2tlm/pci_device_gasket_adapter.h>
#include <simics/systemc/iface/pci_device_interface.h>
#include <simics/systemc/iface/pci_device_simics_adapter.h>

#include "mock/iface/mock_pci_device.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestPciDevice)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockPciDevice,
                   public unittest::MockSimulation {
  public:
    explicit MockDevice(simics::ConfObjectRef objectRef)
        : simics::ConfObject(objectRef) {
    }
};

class TestEnvironment : public Environment {
  public:
    TestEnvironment()
        : object_(&conf_), device_(object_) {
        conf_.instance_data = &device_;
    }
    conf_object_t conf_;
    simics::ConfObjectRef object_;
    MockDevice device_;
};

BOOST_AUTO_TEST_CASE(testPciDevice) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PciDevice pci_device;
    pci_device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockPciDevice mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::PciDeviceExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    pci_device.bus_reset();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    pci_device.system_error();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    pci_device.interrupt_raised(2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    pci_device.interrupt_lowered(4);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
}

BOOST_AUTO_TEST_CASE(testPciDeviceInterruptRaisedWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PciDevice pci_device;
    pci_device.set_gasket(GasketInterface::Ptr(gasket));

    pci_device.bus_reset();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_log_error_count_);
    pci_device.system_error();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(2, Stubs::instance_.SIM_log_error_count_);
    pci_device.interrupt_raised(2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(3, Stubs::instance_.SIM_log_error_count_);
    pci_device.interrupt_lowered(4);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    BOOST_CHECK_EQUAL(4, Stubs::instance_.SIM_log_error_count_);
}

BOOST_FIXTURE_TEST_CASE(testPciDeviceSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::PciDeviceSimicsAdapter<MockDevice> simicsAdapter;
    const pci_device_interface_t *interface =
        static_cast<const pci_device_interface_t *>(simicsAdapter.cstruct());

    BOOST_CHECK(interface->_deprecated_interrupt_acknowledge == NULL);
    BOOST_CHECK(interface->_deprecated_special_cycle == NULL);

    interface->bus_reset(&conf_);
    BOOST_CHECK_EQUAL(device_.bus_reset_, 1);
    interface->system_error(&conf_);
    BOOST_CHECK_EQUAL(device_.system_error_, 1);
    interface->interrupt_raised(&conf_, 2);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_, 1);
    interface->interrupt_lowered(&conf_, 4);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_, 1);
}

BOOST_FIXTURE_TEST_CASE(testPciDeviceGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::PciDeviceGasketAdapter adapter(&device_,
                                                                &simulation_);

    adapter.bus_reset();
    BOOST_CHECK_EQUAL(device_.bus_reset_, 1);
    adapter.system_error();
    BOOST_CHECK_EQUAL(device_.system_error_, 1);
    adapter.interrupt_raised(2);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_raised_pin_, 2);
    adapter.interrupt_lowered(4);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_, 1);
    BOOST_CHECK_EQUAL(device_.interrupt_lowered_pin_, 4);
}

BOOST_AUTO_TEST_SUITE_END()
