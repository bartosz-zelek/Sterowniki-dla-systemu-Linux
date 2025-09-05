// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

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


#include <simics/systemc/simics2tlm/i2c_master_v2.h>
#include <simics/systemc/simics2tlm/i2c_master_v2_gasket_adapter.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/systemc/iface/i2c_master_v2_simics_adapter.h>

#include "mock/iface/mock_i2c_master_v2.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestI2cMasterV2)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockI2cMasterV2,
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

BOOST_AUTO_TEST_CASE(testI2cMasterV2) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I2cMasterV2 device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockI2cMasterV2 mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::I2cMasterV2Extension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    device.acknowledge(simics::types::I2C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    device.read_response(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
}

BOOST_AUTO_TEST_CASE(testWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I2cMasterV2 device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    device.acknowledge(simics::types::I2C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    device.read_response(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::I2cMasterV2SimicsAdapter<MockDevice> simicsAdapter;
    const i2c_master_v2_interface_t *interface =
        static_cast<const i2c_master_v2_interface_t *>(simicsAdapter.cstruct());

    interface->acknowledge(&conf_, ::I2C_noack);
    BOOST_CHECK_EQUAL(device_.acknowledge_, 1);
    interface->read_response(&conf_, 0xff);
    BOOST_CHECK_EQUAL(device_.read_response_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::I2cMasterV2GasketAdapter adapter(&device_,
                                                                  &simulation_);

    adapter.acknowledge(simics::types::I2C_ack);
    BOOST_CHECK_EQUAL(device_.acknowledge_, 1);
    BOOST_CHECK_EQUAL(device_.acknowledge_ack_, simics::types::I2C_ack);
    adapter.read_response(0xff);
    BOOST_CHECK_EQUAL(device_.read_response_, 1);
    BOOST_CHECK_EQUAL(device_.read_response_value_, 0xff);
}

BOOST_AUTO_TEST_SUITE_END()
