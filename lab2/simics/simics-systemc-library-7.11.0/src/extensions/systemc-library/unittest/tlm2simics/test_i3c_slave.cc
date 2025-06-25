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

#include <simics/devs/i3c.h>
#include <simics/systemc/iface/i3c_slave_extension.h>
#include <simics/systemc/tlm2simics/i3c_slave.h>

#include "test_gasket_environment.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsI3cSlave)

// Implementation of Simics i3c_slave_interface_t interface for testing
class MockI3cSlaveSimicsInterfaceImpl : public i3c_slave_interface_t {
  public:
    MockI3cSlaveSimicsInterfaceImpl()
        : start_called_(false),
          write_called_(false),
          sdr_write_called_(false),
          read_called_(false),
          daa_read_called_(false),
          stop_called_(false),
          ibi_start_called_(false),
          ibi_acknowledge_called_(false),
          start_address_(0),
          write_value_(0),
          ibi_acknowledge_value_(::I3C_noack),
          obj_(NULL) {
        instance_ = this;
        start = mock_start;
        write = mock_write;
        sdr_write = mock_sdr_write;
        read = mock_read;
        daa_read = mock_daa_read;
        stop = mock_stop;
        ibi_start = mock_ibi_start;
        ibi_acknowledge = mock_ibi_acknowledge;
    }
    static void mock_start(conf_object_t *obj, uint8 address) {
        instance_->start_address_ = address;
        instance_->start_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_write(conf_object_t *obj, uint8 value) {
        instance_->write_value_ = value;
        instance_->write_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_sdr_write(conf_object_t *obj, bytes_t data) {
        instance_->sdr_write_value_ = data;
        instance_->sdr_write_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_read(conf_object_t *obj) {
        instance_->read_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_daa_read(conf_object_t *obj) {
        instance_->daa_read_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_stop(conf_object_t *obj) {
        instance_->stop_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_ibi_start(conf_object_t *obj) {
        instance_->ibi_start_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_ibi_acknowledge(conf_object_t *obj, i3c_ack_t ack) {
        instance_->ibi_acknowledge_called_ = true;
        instance_->ibi_acknowledge_value_ = ack;
        instance_->obj_ = obj;
    }
    static MockI3cSlaveSimicsInterfaceImpl *instance_;
    bool start_called_;
    bool write_called_;
    bool sdr_write_called_;
    bool read_called_;
    bool daa_read_called_;
    bool stop_called_;
    bool ibi_start_called_;
    bool ibi_acknowledge_called_;
    uint8 start_address_;
    uint8 write_value_;
    bytes_t sdr_write_value_;
    i3c_ack_t ibi_acknowledge_value_;
    conf_object_t *obj_;
};

MockI3cSlaveSimicsInterfaceImpl *
    MockI3cSlaveSimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<
          simics::systemc::tlm2simics::I3cSlave,
          simics::systemc::iface::I3cSlaveExtension,
          MockI3cSlaveSimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(I3C_SLAVE_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testStart, TestEnv) {
    extension_.start(0x24);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, true);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.start_address_, 0x24);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testWrite, TestEnv) {
    extension_.write(0x55);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, true);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_value_, 0x55);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testSdrwrite, TestEnv) {
    const uint8_t data[2] = {0x55, 0x66};
    simics::types::bytes_t bytes;
    bytes.data = data;
    bytes.len = 2;
    extension_.sdr_write(bytes);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, true);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_value_.data, bytes.data);
    BOOST_CHECK_EQUAL(interface_.sdr_write_value_.len, bytes.len);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testRead, TestEnv) {
    extension_.read();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, true);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testDaaread, TestEnv) {
    extension_.daa_read();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, true);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testStop, TestEnv) {
    extension_.stop();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testIbistart, TestEnv) {
    extension_.ibi_start();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testIbiacknowledge, TestEnv) {
    extension_.ibi_acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.start_called_, false);
    BOOST_CHECK_EQUAL(interface_.write_called_, false);
    BOOST_CHECK_EQUAL(interface_.sdr_write_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_read_called_, false);
    BOOST_CHECK_EQUAL(interface_.stop_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_start_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_acknowledge_value_, I3C_ack);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_AUTO_TEST_SUITE_END()
