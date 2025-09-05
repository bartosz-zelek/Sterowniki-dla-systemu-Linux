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

#include <simics/devs/i3c.h>
#include <simics/systemc/iface/i3c_master_extension.h>
#include <simics/systemc/tlm2simics/i3c_master.h>

#include "test_gasket_environment.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsI3cMaster)

// Implementation of Simics i3c_master_interface_t interface for testing
class MockI3cMasterSimicsInterfaceImpl : public i3c_master_interface_t {
  public:
    MockI3cMasterSimicsInterfaceImpl()
        : acknowledge_called_(false),
          read_response_called_(false),
          daa_response_called_(false),
          ibi_request_called_(false),
          ibi_address_called_(false),
          acknowledge_value_(::I3C_noack),
          read_response_value_(0),
          read_response_more_(false),
          daa_response_id_(0),
          daa_response_bcr_(0),
          daa_response_dcr_(0),
          ibi_address_address_(0),
          obj_(NULL) {
        instance_ = this;
        acknowledge = mock_acknowledge;
        read_response = mock_read_response;
        daa_response = mock_daa_response;
        ibi_request = mock_ibi_request;
        ibi_address = mock_ibi_address;
    }
    static void mock_acknowledge(conf_object_t *obj, ::i3c_ack_t ack) {
        instance_->acknowledge_value_ = ack;
        instance_->acknowledge_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_read_response(conf_object_t *obj, uint8 value, bool more) {
        instance_->read_response_value_ = value;
        instance_->read_response_more_ = more;
        instance_->read_response_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_daa_response(conf_object_t *obj, uint64 id, uint8 bcr,
                                  uint8 dcr) {
        instance_->daa_response_id_ = id;
        instance_->daa_response_bcr_ = bcr;
        instance_->daa_response_dcr_ = dcr;
        instance_->daa_response_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_ibi_request(conf_object_t *obj) {
        instance_->ibi_request_called_ = true;
        instance_->obj_ = obj;
    }
    static void mock_ibi_address(conf_object_t *obj, uint8 address) {
        instance_->ibi_address_address_ = address;
        instance_->ibi_address_called_ = true;
        instance_->obj_ = obj;
    }
    static MockI3cMasterSimicsInterfaceImpl *instance_;
    bool acknowledge_called_;
    bool read_response_called_;
    bool daa_response_called_;
    bool ibi_request_called_;
    bool ibi_address_called_;
    ::i3c_ack_t acknowledge_value_;
    uint8 read_response_value_;
    bool read_response_more_;
    uint64 daa_response_id_;
    uint8 daa_response_bcr_;
    uint8 daa_response_dcr_;
    uint8 ibi_address_address_;
    conf_object_t *obj_;
};

MockI3cMasterSimicsInterfaceImpl *
    MockI3cMasterSimicsInterfaceImpl::instance_ = NULL;

class TestEnv
    : public TestGasketEnvironment<
          simics::systemc::tlm2simics::I3cMaster,
          simics::systemc::iface::I3cMasterExtension,
          MockI3cMasterSimicsInterfaceImpl> {
  public:
    TestEnv() : TestGasketEnvironment(I3C_MASTER_INTERFACE) {}
};

BOOST_FIXTURE_TEST_CASE(testAcknowledge, TestEnv) {
    extension_.acknowledge(simics::types::I3C_ack);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.acknowledge_called_, true);
    BOOST_CHECK_EQUAL(interface_.read_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_request_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_address_called_, false);
    BOOST_CHECK_EQUAL(interface_.acknowledge_value_, I3C_ack);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testReadresponse, TestEnv) {
    extension_.read_response(0x55, true);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_response_called_, true);
    BOOST_CHECK_EQUAL(interface_.daa_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_request_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_address_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_response_value_, 0x55);
    BOOST_CHECK_EQUAL(interface_.read_response_more_, true);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testDaaresponse, TestEnv) {
    extension_.daa_response(0x123456789abcdef, 0x55, 0x66);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_response_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_request_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_address_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_response_id_, 0x123456789abcdef);
    BOOST_CHECK_EQUAL(interface_.daa_response_bcr_, 0x55);
    BOOST_CHECK_EQUAL(interface_.daa_response_dcr_, 0x66);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testIbirequest, TestEnv) {
    extension_.ibi_request();
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_request_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_address_called_, false);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_FIXTURE_TEST_CASE(testIbiaddress, TestEnv) {
    extension_.ibi_address(0x55);
    BOOST_CHECK_EQUAL(response_, tlm::TLM_OK_RESPONSE);

    BOOST_CHECK_EQUAL(interface_.acknowledge_called_, false);
    BOOST_CHECK_EQUAL(interface_.read_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.daa_response_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_request_called_, false);
    BOOST_CHECK_EQUAL(interface_.ibi_address_called_, true);
    BOOST_CHECK_EQUAL(interface_.ibi_address_address_, 0x55);
    BOOST_CHECK_EQUAL(interface_.obj_, simulation_.simics_object().object());
}

BOOST_AUTO_TEST_SUITE_END()
