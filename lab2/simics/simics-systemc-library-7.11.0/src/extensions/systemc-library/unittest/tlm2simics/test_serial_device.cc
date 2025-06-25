// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/devs/serial-device.h>
#include <simics/systemc/iface/serial_device_extension.h>
#include <simics/systemc/tlm2simics/serial_device.h>

#include "test_gasket_environment.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsSerialDevice)

// Implementation of Simics serial_device_interface_t interface for testing
class MockSerialDeviceSimicsInterfaceImpl : public serial_device_interface_t {
  public:
    MockSerialDeviceSimicsInterfaceImpl()
     : write_called_(false), write_value_(0), receive_ready_called_(false),
       obj_(NULL) {
        instance_ = this;
        write = mock_write;
        receive_ready = mock_receive_ready;
    }
    static int mock_write(conf_object_t *obj, int value) {
        instance_->write_value_ = value;
        instance_->write_called_ = true;
        instance_->obj_ = obj;
        return 42;
    }
    static void mock_receive_ready(conf_object_t *obj) {
        instance_->receive_ready_called_ = true;
        instance_->obj_ = obj;
    }
    static MockSerialDeviceSimicsInterfaceImpl *instance_;
    bool write_called_;
    int write_value_;
    bool receive_ready_called_;
    conf_object_t *obj_;
};

MockSerialDeviceSimicsInterfaceImpl *
    MockSerialDeviceSimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<
          simics::systemc::tlm2simics::SerialDevice,
          simics::systemc::iface::SerialDeviceExtension,
          MockSerialDeviceSimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(SERIAL_DEVICE_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testWrite, TestEnv) {
    BOOST_CHECK_EQUAL(extension_.write(24), 42);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.receive_ready_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, true);
    BOOST_CHECK_EQUAL(interface_.write_value_, 24);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testReceiveReady, TestEnv) {
    extension_.receive_ready();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.receive_ready_called_, true);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_AUTO_TEST_SUITE_END()
