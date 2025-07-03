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


#include <simics/systemc/simics2tlm/i2c_slave_v2.h>
#include <simics/systemc/simics2tlm/i2c_slave_v2_gasket_adapter.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>
#include <simics/systemc/iface/i2c_slave_v2_simics_adapter.h>

#include "mock/iface/mock_i2c_slave_v2.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestI2cSlaveV2)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockI2cSlaveV2,
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

BOOST_AUTO_TEST_CASE(testI2cSlaveV2) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I2cSlaveV2 device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockI2cSlaveV2 mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::I2cSlaveV2Extension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    device.start(0x80);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    device.read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    device.write(5);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    device.stop();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    std::vector<uint8_t> input {8, 88};
    mock_device.addresses_return_value_ = input;
    std::vector<uint8_t> addresses = device.addresses();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    BOOST_CHECK_EQUAL(addresses.size(), 2);
    BOOST_CHECK_EQUAL_COLLECTIONS(addresses.begin(), addresses.end(),
                                  input.begin(), input.end());
}

BOOST_AUTO_TEST_CASE(testWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I2cSlaveV2 device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    device.start(0x80);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    device.read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
    device.write(5);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 3);
    device.stop();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 4);
    std::vector<uint8_t> addresses = device.addresses();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 5);
    BOOST_CHECK_EQUAL(addresses.size(), 0);
}

BOOST_FIXTURE_TEST_CASE(testSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::I2cSlaveV2SimicsAdapter<MockDevice> simicsAdapter;
    const i2c_slave_v2_interface_t *interface =
        static_cast<const i2c_slave_v2_interface_t *>(simicsAdapter.cstruct());

    interface->start(&conf_, 0x80);
    BOOST_CHECK_EQUAL(device_.start_, 1);
    interface->read(&conf_);
    BOOST_CHECK_EQUAL(device_.read_, 1);
    interface->write(&conf_, 2);
    BOOST_CHECK_EQUAL(device_.write_, 1);
    interface->stop(&conf_);
    BOOST_CHECK_EQUAL(device_.stop_, 1);
    interface->addresses(&conf_);
    BOOST_CHECK_EQUAL(device_.addresses_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::I2cSlaveV2GasketAdapter adapter(&device_,
                                                                 &simulation_);

    adapter.start(0x80);
    BOOST_CHECK_EQUAL(device_.start_, 1);
    BOOST_CHECK_EQUAL(device_.start_address_, 0x80);
    adapter.read();
    BOOST_CHECK_EQUAL(device_.read_, 1);
    adapter.write(5);
    BOOST_CHECK_EQUAL(device_.write_, 1);
    BOOST_CHECK_EQUAL(device_.write_value_, 5);
    adapter.stop();
    BOOST_CHECK_EQUAL(device_.stop_, 1);;
    adapter.addresses();
    BOOST_CHECK_EQUAL(device_.addresses_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
