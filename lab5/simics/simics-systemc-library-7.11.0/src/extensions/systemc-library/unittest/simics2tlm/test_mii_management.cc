// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

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


#include <simics/systemc/simics2tlm/mii_management.h>
#include <simics/systemc/simics2tlm/mii_management_gasket_adapter.h>
#include <simics/systemc/iface/mii_management_interface.h>
#include <simics/systemc/iface/mii_management_simics_adapter.h>

#include "mock/iface/mock_mii_management.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestMiiManagement)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockMiiManagement,
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

BOOST_AUTO_TEST_CASE(testMiiManagement) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::MiiManagement device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockMiiManagement mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::MiiManagementExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    BOOST_CHECK_EQUAL(device.serial_access(0xc0f, 0xfee), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(device.read_register(0xab, 0xcd), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    device.write_register(0xde, 0xad, 0xbeef);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
}

BOOST_AUTO_TEST_CASE(testWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::MiiManagement device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    BOOST_CHECK_EQUAL(device.serial_access(0xc0f, 0xfee), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    BOOST_CHECK_EQUAL(device.read_register(0xab, 0xcd), 0);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
    device.write_register(0xde, 0xad, 0xbeef);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 3);
}

BOOST_FIXTURE_TEST_CASE(testSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::MiiManagementSimicsAdapter<
            MockDevice> simicsAdapter;
    const mii_management_interface_t *interface =
        static_cast<const mii_management_interface_t *>(
                simicsAdapter.cstruct());

    BOOST_CHECK_EQUAL(interface->serial_access(&conf_, 0xc0f, 0xfee), 0);
    BOOST_CHECK_EQUAL(device_.serial_access_, 1);
    BOOST_CHECK_EQUAL(interface->read_register(&conf_, 0xab, 0xcd), 0);
    BOOST_CHECK_EQUAL(device_.read_register_, 1);
    device_.write_register(0xde, 0xad, 0xbeef);
    BOOST_CHECK_EQUAL(device_.write_register_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::MiiManagementGasketAdapter adapter(
            &device_, &simulation_);

    BOOST_CHECK_EQUAL(adapter.read_register(0xab, 0xcd), 0);
    BOOST_CHECK_EQUAL(device_.read_register_, 1);
    BOOST_CHECK_EQUAL(device_.read_register_phy_, 0xab);
    BOOST_CHECK_EQUAL(device_.read_register_reg_, 0xcd);
    adapter.write_register(0xde, 0xad, 0xbeef);
    BOOST_CHECK_EQUAL(device_.write_register_, 1);
    BOOST_CHECK_EQUAL(device_.write_register_phy_, 0xde);
    BOOST_CHECK_EQUAL(device_.write_register_reg_, 0xad);
    BOOST_CHECK_EQUAL(device_.write_register_value_, 0xbeef);
    BOOST_CHECK_EQUAL(adapter.serial_access(0xc0f, 0xfee), 0);
    BOOST_CHECK_EQUAL(device_.serial_access_data_in_, 0xc0f);
    BOOST_CHECK_EQUAL(device_.serial_access_clock_, 0xfee);
}

BOOST_AUTO_TEST_SUITE_END()
