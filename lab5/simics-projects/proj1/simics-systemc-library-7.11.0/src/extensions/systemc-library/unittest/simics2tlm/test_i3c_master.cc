// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

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


#include <simics/systemc/simics2tlm/i3c_master.h>
#include <simics/systemc/simics2tlm/i3c_master_gasket_adapter.h>
#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/systemc/iface/i3c_master_simics_adapter.h>

#include "mock/iface/mock_i3c_master.h"
#include "mock/mock_simulation.h"
#include "mock/simics2tlm/mock_gasket.h"
#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestI3cMaster)

using simics::systemc::simics2tlm::GasketInterface;

class MockDevice : public simics::ConfObject,
                   public MockI3cMaster,
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

BOOST_AUTO_TEST_CASE(testI3cMaster) {
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I3cMaster device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    // extension needs a receiver that sets the return value
    MockI3cMaster mock_device;
    simics::systemc::iface::ReceiverInterface *receiver
        = simics::systemc::iface::I3cMasterExtension::createReceiver(
            &mock_device);
    gasket->set_receiver(receiver);

    device.acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    device.read_response(0xff, true);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    device.daa_response(0xffffffffffffffff, 0xff, 0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    device.ibi_request();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    device.ibi_address(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
}

BOOST_AUTO_TEST_CASE(testWithoutReceiver) {
    Stubs stubs;
    unittest::simics2tlm::MockGasket *gasket
        = new unittest::simics2tlm::MockGasket;
    simics::systemc::simics2tlm::I3cMaster device;
    device.set_gasket(GasketInterface::Ptr(gasket));

    device.acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
    device.read_response(0xff, true);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 2);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
    device.daa_response(0xffffffffffffffff, 0xff, 0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 3);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 3);
    device.ibi_request();
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 4);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 4);
    device.ibi_address(0xff);
    BOOST_CHECK_EQUAL(gasket->trigger_transaction_, 5);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 5);
}

BOOST_FIXTURE_TEST_CASE(testSimicsAdapter, TestEnvironment) {
    simics::systemc::iface::I3cMasterSimicsAdapter<MockDevice> simicsAdapter;
    const i3c_master_interface_t *interface =
        static_cast<const i3c_master_interface_t *>(simicsAdapter.cstruct());

    interface->acknowledge(&conf_, ::I3C_noack);
    BOOST_CHECK_EQUAL(device_.acknowledge_, 1);
    interface->read_response(&conf_, 0xff, true);
    BOOST_CHECK_EQUAL(device_.read_response_, 1);
    interface->daa_response(&conf_, 0xffffffffffffffff, 0xff, 0xff);
    BOOST_CHECK_EQUAL(device_.daa_response_, 1);
    interface->ibi_request(&conf_);
    BOOST_CHECK_EQUAL(device_.ibi_request_, 1);
    interface->ibi_address(&conf_, 0xff);
    BOOST_CHECK_EQUAL(device_.ibi_address_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGasketAdapter, TestEnvironment) {
    simics::systemc::simics2tlm::I3cMasterGasketAdapter adapter(&device_,
                                                                &simulation_);

    adapter.acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(device_.acknowledge_, 1);
    BOOST_CHECK_EQUAL(device_.acknowledge_ack_, simics::types::I3C_ack);
    adapter.read_response(0xff, true);
    BOOST_CHECK_EQUAL(device_.read_response_, 1);
    BOOST_CHECK_EQUAL(device_.read_response_value_, 0xff);
    BOOST_CHECK_EQUAL(device_.read_response_more_, true);
    adapter.daa_response(0xffffffffffffffff, 0xff, 0xff);
    BOOST_CHECK_EQUAL(device_.daa_response_, 1);
    BOOST_CHECK_EQUAL(device_.daa_response_id_, 0xffffffffffffffff);
    BOOST_CHECK_EQUAL(device_.daa_response_bcr_, 0xff);
    BOOST_CHECK_EQUAL(device_.daa_response_dcr_, 0xff);
    adapter.ibi_request();
    BOOST_CHECK_EQUAL(device_.ibi_request_, 1);
    adapter.ibi_address(0xff);
    BOOST_CHECK_EQUAL(device_.ibi_address_, 1);
    BOOST_CHECK_EQUAL(device_.ibi_address_address_, 0xff);
}

BOOST_AUTO_TEST_SUITE_END()
