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

#include <simics/systemc/injection/attr_dict_parser.h>
#include <simics/systemc/injection/inject_gp.h>

#include "environment.h"
#include "stubs.h"

using simics::systemc::injection::InjectGp;
using simics::systemc::injection::AttrDictParser;

BOOST_AUTO_TEST_SUITE(TestInjectGp)

class TestEnvironment : public Environment {
  public:
    TestEnvironment() {
        attr_ = SIM_alloc_attr_dict(2);
    }
    ~TestEnvironment() {
        SIM_attr_free(&attr_);
    }
    void setAttrToValue0(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
    }
    void setAttrToValue1(const char* key, attr_value_t value) {
        SIM_attr_dict_set_item(&attr_, 1, SIM_make_attr_string(key), value);
    }
    void testGp(const char* key, attr_value_t value) {
        setAttrToValue0(key, value);
        AttrDictParser parser(simulation_.simics_object(), &attr_);
        inject_.attrToValue(&parser, &payload_);
        parser.reportInvalidAttrs();
    }
    Stubs stubs_;
    attr_value_t attr_;
    InjectGp<tlm::tlm_generic_payload> inject_;
    tlm::tlm_generic_payload payload_;
};

BOOST_FIXTURE_TEST_CASE(testGpCommand, TestEnvironment) {
    testGp("gp.command", SIM_make_attr_uint64(tlm::TLM_READ_COMMAND));
    BOOST_CHECK_EQUAL(payload_.get_command(), tlm::TLM_READ_COMMAND);

    testGp("gp.command", SIM_make_attr_uint64(tlm::TLM_WRITE_COMMAND));
    BOOST_CHECK_EQUAL(payload_.get_command(), tlm::TLM_WRITE_COMMAND);

    testGp("gp.command", SIM_make_attr_uint64(tlm::TLM_IGNORE_COMMAND));
    BOOST_CHECK_EQUAL(payload_.get_command(), tlm::TLM_IGNORE_COMMAND);

    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);

    testGp("gp.command", SIM_make_attr_uint64(3));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);

    testGp("gp.command", SIM_make_attr_boolean(1));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testGpAddress, TestEnvironment) {
    testGp("gp.address", SIM_make_attr_uint64(1234));
    BOOST_CHECK_EQUAL(payload_.get_address(), 1234);

    testGp("gp.address", SIM_make_attr_boolean(1));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGpDataPtr, TestEnvironment) {
    testGp("gp.data_ptr", SIM_make_attr_data(4, "abc"));

    BOOST_REQUIRE(payload_.get_data_length() == 4);

    BOOST_CHECK_EQUAL(payload_.get_data_ptr()[0], 'a');
    BOOST_CHECK_EQUAL(payload_.get_data_ptr()[1], 'b');
    BOOST_CHECK_EQUAL(payload_.get_data_ptr()[2], 'c');
    BOOST_CHECK_EQUAL(payload_.get_data_ptr()[3], 0);
}

BOOST_FIXTURE_TEST_CASE(testGpResponseStatus, TestEnvironment) {
    testGp("gp.response_status", SIM_make_attr_int64(tlm::TLM_OK_RESPONSE));
    BOOST_CHECK(payload_.get_response_status() == tlm::TLM_OK_RESPONSE);

    testGp("gp.response_status", SIM_make_attr_int64(
        tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE));
    BOOST_CHECK(payload_.get_response_status() ==
        tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);

    testGp("gp.response_status", SIM_make_attr_int64(2));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);

    testGp("gp.response_status", SIM_make_attr_int64(-6));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testGpStreamingWidth, TestEnvironment) {
    testGp("gp.streaming_width", SIM_make_attr_uint64(4132));
    BOOST_CHECK(payload_.get_streaming_width() == 4132);
}

BOOST_FIXTURE_TEST_CASE(testGpByteEnablePtr, TestEnvironment) {
    testGp("gp.byte_enable_ptr", SIM_make_attr_uint64(3412));
    BOOST_CHECK(payload_.get_byte_enable_ptr() ==
        reinterpret_cast<unsigned char*>(3412));
}

BOOST_FIXTURE_TEST_CASE(testGpByteEnableLength, TestEnvironment) {
    testGp("gp.byte_enable_length", SIM_make_attr_uint64(2341));
    BOOST_CHECK_EQUAL(payload_.get_byte_enable_length(), 2341);
}

BOOST_FIXTURE_TEST_CASE(testGpOption, TestEnvironment) {
    testGp("gp.gp_option", SIM_make_attr_uint64(tlm::TLM_MIN_PAYLOAD));
    BOOST_CHECK_EQUAL(payload_.get_gp_option(), tlm::TLM_MIN_PAYLOAD);

    testGp("gp.gp_option", SIM_make_attr_uint64(tlm::TLM_FULL_PAYLOAD));
    BOOST_CHECK_EQUAL(payload_.get_gp_option(), tlm::TLM_FULL_PAYLOAD);

    testGp("gp.gp_option", SIM_make_attr_uint64(
        tlm::TLM_FULL_PAYLOAD_ACCEPTED));
    BOOST_CHECK_EQUAL(payload_.get_gp_option(), tlm::TLM_FULL_PAYLOAD_ACCEPTED);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);

    testGp("gp.gp_option", SIM_make_attr_uint64(3));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);

    testGp("gp.gp_option", SIM_make_attr_boolean(1));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testGpDmiAllowed, TestEnvironment) {
    testGp("gp.dmi_allowed", SIM_make_attr_boolean(1));
    BOOST_CHECK_EQUAL(payload_.is_dmi_allowed(), true);

    testGp("gp.dmi_allowed", SIM_make_attr_boolean(0));
    BOOST_CHECK_EQUAL(payload_.is_dmi_allowed(), false);

    testGp("gp.dmi_allowed", SIM_make_attr_uint64(0));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testGpMultipleAtOnce, TestEnvironment) {
    setAttrToValue1("gp.command", SIM_make_attr_uint64(tlm::TLM_WRITE_COMMAND));
    testGp("gp.address", SIM_make_attr_uint64(1234));
    BOOST_CHECK_EQUAL(payload_.get_command(), tlm::TLM_WRITE_COMMAND);
    BOOST_CHECK_EQUAL(payload_.get_address(), 1234);
}

BOOST_AUTO_TEST_SUITE_END()
