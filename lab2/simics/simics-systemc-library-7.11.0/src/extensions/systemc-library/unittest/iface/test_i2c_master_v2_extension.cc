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

#include <simics/systemc/iface/i2c_master_v2_extension.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/simics2tlm/extension_sender.h>

#include "mock/iface/mock_i2c_master_v2.h"
#include "mock/simics2tlm/mock_gasket.h"

BOOST_AUTO_TEST_SUITE(TestI2cMasterV2Extension)

namespace sif = simics::systemc::iface;

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::simics2tlm::GasketInterface::Ptr gasket(
                new unittest::simics2tlm::MockGasket);
        sender_.init(gasket);
        extension_.init(&sender_);
        receiver_ = sif::I2cMasterV2Extension::createReceiver(&master_);
        static_cast<unittest::simics2tlm::MockGasket *>(
                gasket.get())->set_receiver(receiver_);
    }
    ~TestEnvironment() {
        delete receiver_;
    }
    simics::systemc::simics2tlm::ExtensionSender sender_;
    MockI2cMasterV2 master_;
    simics::systemc::iface::I2cMasterV2Extension extension_;
    simics::systemc::iface::ReceiverInterface *receiver_;
};

BOOST_FIXTURE_TEST_CASE(testAcknowledge, TestEnvironment) {
    extension_.acknowledge(simics::types::I2C_ack);
    BOOST_CHECK_EQUAL(master_.acknowledge_, 1);
    BOOST_CHECK_EQUAL(master_.acknowledge_ack_, simics::types::I2C_ack);
}

BOOST_FIXTURE_TEST_CASE(testReadResponse, TestEnvironment) {
    extension_.read_response(3);
    BOOST_CHECK_EQUAL(master_.read_response_, 1);
    BOOST_CHECK_EQUAL(master_.read_response_value_, 3);
}

BOOST_AUTO_TEST_SUITE_END()
