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


#include <simics/systemc/simics2tlm/i3c_slave.h>
#include <simics/systemc/simics2tlm/i3c_slave_gasket_adapter.h>
#include <simics/systemc/iface/i3c_slave_interface.h>
#include <simics/systemc/iface/i3c_slave_simics_adapter.h>

#include "mock/iface/mock_i3c_slave.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestI3cSlave)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockI3cSlave,
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

BOOST_AUTO_TEST_CASE(testI3cSlave) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I3cSlave device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockI3cSlave mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::I3cSlaveExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    device.start(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    device.write(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    const uint8_t data[2] = {0x55, 0x66};
    simics::types::bytes_t bytes;
    bytes.data = data;
    bytes.len = 2;
    device.sdr_write(bytes);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    device.read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    device.daa_read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    device.stop();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 6);
    device.ibi_start();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 7);
    device.ibi_acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 8);
}

BOOST_AUTO_TEST_CASE(testWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I3cSlave device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    device.start(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    device.write(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
    const uint8_t data[2] = {0x55, 0x66};
    simics::types::bytes_t bytes;
    bytes.data = data;
    bytes.len = 2;
    device.sdr_write(bytes);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 3);
    device.read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 4);
    device.daa_read();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 5);
    device.stop();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 6);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 6);
    device.ibi_start();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 7);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 7);
    device.ibi_acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 8);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 8);
}

BOOST_FIXTURE_TEST_CASE(testSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::I3cSlaveSimicsAdapter<MockDevice> simicsAdapter;
    const i3c_slave_interface_t *interface =
        static_cast<const i3c_slave_interface_t *>(simicsAdapter.cstruct());

    interface->start(&conf_, 0xff);
    BOOST_CHECK_EQUAL(device_.start_, 1);
    interface->write(&conf_, 0xff);
    BOOST_CHECK_EQUAL(device_.write_, 1);
    const uint8_t data[2] = {0x55, 0x66};
    ::bytes_t bytes;
    bytes.data = data;
    bytes.len = 2;
    interface->sdr_write(&conf_, bytes);
    BOOST_CHECK_EQUAL(device_.sdr_write_, 1);
    interface->read(&conf_);
    BOOST_CHECK_EQUAL(device_.read_, 1);
    interface->daa_read(&conf_);
    BOOST_CHECK_EQUAL(device_.daa_read_, 1);
    interface->stop(&conf_);
    BOOST_CHECK_EQUAL(device_.stop_, 1);
    interface->ibi_start(&conf_);
    BOOST_CHECK_EQUAL(device_.ibi_start_, 1);
    interface->ibi_acknowledge(&conf_, ::I3C_ack);
    BOOST_CHECK_EQUAL(device_.ibi_acknowledge_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::I3cSlaveGasketAdapter adapter(&device_,
                                                               &simulation_);

    adapter.start(0xff);
    BOOST_CHECK_EQUAL(device_.start_, 1);
    BOOST_CHECK_EQUAL(device_.start_address_, 0xff);
    adapter.write(0xff);
    BOOST_CHECK_EQUAL(device_.write_, 1);
    BOOST_CHECK_EQUAL(device_.write_value_, 0xff);
    const uint8_t data[2] = {0x55, 0x66};
    simics::types::bytes_t bytes;
    bytes.data = data;
    bytes.len = 2;
    adapter.sdr_write(bytes);
    BOOST_CHECK_EQUAL(device_.sdr_write_, 1);
    adapter.read();
    BOOST_CHECK_EQUAL(device_.read_, 1);
    adapter.daa_read();
    BOOST_CHECK_EQUAL(device_.daa_read_, 1);
    adapter.stop();
    BOOST_CHECK_EQUAL(device_.stop_, 1);
    adapter.ibi_start();
    BOOST_CHECK_EQUAL(device_.ibi_start_, 1);
    adapter.ibi_acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(device_.ibi_acknowledge_, 1);
    BOOST_CHECK_EQUAL(device_.ibi_acknowledge_ack_, simics::types::I3C_ack);
}

BOOST_AUTO_TEST_SUITE_END()
