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

#include <simics/devs/i2c.h>
#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/tlm2simics/i2c_slave_v2.h>

#include <stdint.h>
#include <vector>

#include "test_gasket_environment.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsI2cSlaveV2)

// Implementation of Simics i2c_slave_v2_interface_t interface for testing
class MockI2cSlaveV2SimicsInterfaceImpl : public i2c_slave_v2_interface_t {
  public:
    MockI2cSlaveV2SimicsInterfaceImpl()
        : start_called_(false),
          read_called_(false),
          write_called_(false),
          stop_called_(false),
          addresses_called_(false),
          start_address_(0),
          write_value_(0),
          obj_(NULL) {
        instance_ = this;
        start = mock_start;
        read = mock_read;
        write = mock_write;
        stop = mock_stop;
        addresses = mock_addresses;
    }
    static void mock_start(conf_object_t *obj, uint8_t address) {
        instance_->start_address_ = address;
        instance_->start_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_read(conf_object_t *obj) {
        instance_->read_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_write(conf_object_t *obj, uint8_t value) {
        instance_->write_value_ = value;
        instance_->write_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_stop(conf_object_t *obj) {
        instance_->stop_called_ = true;
        instance_->obj_ = obj;
    }
    static attr_value_t mock_addresses(conf_object_t *obj) {
        instance_->addresses_called_ = true;
        instance_->obj_ = obj;
        attr_value_t attrs = SIM_make_attr_list(
            2, SIM_make_attr_uint64(0xf1), SIM_make_attr_uint64(0xf2));
        return attrs;
    }
    static MockI2cSlaveV2SimicsInterfaceImpl *instance_;
    bool start_called_;
    bool read_called_;
    bool write_called_;
    bool stop_called_;
    bool addresses_called_;
    uint8_t start_address_;
    uint8_t write_value_;
    conf_object_t *obj_;
};

MockI2cSlaveV2SimicsInterfaceImpl *
    MockI2cSlaveV2SimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<
          simics::systemc::tlm2simics::I2cSlaveV2,
          simics::systemc::iface::I2cSlaveV2Extension,
          MockI2cSlaveV2SimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(I2C_SLAVE_V2_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testStart, TestEnv) {
    extension_.start(0x24);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, true);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.addresses_called_, false);
    BOOST_CHECK_EQUAL(interface_.start_address_, 0x24);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testRead, TestEnv) {
    extension_.read();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, true);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.addresses_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testWrite, TestEnv) {
    extension_.write(0x55);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, true);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.addresses_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_value_, 0x55);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testStop, TestEnv) {
    extension_.stop();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, true);
    BOOST_CHECK_EQUAL(interface_.addresses_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testAddresses, TestEnv) {
    std::vector<uint8_t> val = extension_.addresses();
    BOOST_CHECK_EQUAL(val.size(), 2);
    BOOST_CHECK_EQUAL(val.front(), 0xf1);
    BOOST_CHECK_EQUAL(val.back(), 0xf2);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.addresses_called_, true);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_AUTO_TEST_SUITE_END()
