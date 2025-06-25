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

#include <simics/base/types.h>


#include <simics/systemc/simics2tlm/pcie_device.h>
#include <simics/systemc/simics2tlm/pcie_device_gasket_adapter.h>
#include <simics/systemc/iface/pcie_device_interface.h>
#include <simics/systemc/iface/pcie_device_simics_adapter.h>

#include "mock/iface/mock_pcie_device.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestPcieDevice)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockPcieDevice,
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

BOOST_AUTO_TEST_CASE(testPcieDevice) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PcieDevice pcie_device;
    pcie_device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockPcieDevice mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::PcieDeviceExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    pcie_device.connected(nullptr, 2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    pcie_device.disconnected(nullptr, 2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    pcie_device.hot_reset();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
}

BOOST_AUTO_TEST_CASE(testPcieDeviceWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::PcieDevice pcie_device;
    pcie_device.set_gasket(GasketInterface::Ptr(gasket));

    pcie_device.connected(nullptr, 2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.SIM_log_error_count_);
    pcie_device.disconnected(nullptr, 2);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(2, Stubs::instance_.SIM_log_error_count_);
    pcie_device.hot_reset();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(3, Stubs::instance_.SIM_log_error_count_);
}

BOOST_FIXTURE_TEST_CASE(testPcieDeviceSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::PcieDeviceSimicsAdapter<MockDevice> simicsAdapter;
    auto *interface =
        static_cast<const pcie_device_interface_t *>(simicsAdapter.cstruct());

    interface->connected(&conf_, nullptr, 2);
    BOOST_CHECK_EQUAL(device_.connected_, 1);
    interface->disconnected(&conf_, nullptr, 2);
    BOOST_CHECK_EQUAL(device_.disconnected_, 1);
    interface->hot_reset(&conf_);
    BOOST_CHECK_EQUAL(device_.hot_reset_, 1);
}

BOOST_FIXTURE_TEST_CASE(testPcieDeviceGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::PcieDeviceGasketAdapter adapter(
        &device_, &simulation_);

    adapter.connected(nullptr, 2);
    BOOST_CHECK_EQUAL(device_.connected_, 1);
    adapter.disconnected(nullptr, 2);
    BOOST_CHECK_EQUAL(device_.disconnected_, 1);
    adapter.hot_reset();
    BOOST_CHECK_EQUAL(device_.hot_reset_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
