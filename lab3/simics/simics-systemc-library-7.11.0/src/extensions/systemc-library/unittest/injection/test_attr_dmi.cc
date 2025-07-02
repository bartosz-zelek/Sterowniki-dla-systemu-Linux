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
#include <simics/systemc/injection/attr_dmi.h>

#include "environment.h"
#include "stubs.h"

using simics::systemc::injection::AttrDictParser;
using simics::systemc::injection::AttrDmi;

BOOST_AUTO_TEST_SUITE(TestAttrDmi)

class TestEnvironment : public Environment {
  public:
    TestEnvironment() : attr_(SIM_alloc_attr_dict(2)), attr_dmi_(&dmi_) {
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
    void testDmi(const char* key, attr_value_t value, bool expected) {
        setAttrToValue0(key, value);
        SIM_attr_dict_set_item(&attr_, 0, SIM_make_attr_string(key), value);
        AttrDictParser p(simulation_.simics_object(), &attr_);
        p.parse(&attr_dmi_);
        BOOST_CHECK_EQUAL(p.reportInvalidAttrs(), expected);
    }
    Stubs stubs_;
    attr_value_t attr_;
    tlm::tlm_dmi dmi_;
    AttrDmi attr_dmi_;
};

BOOST_FIXTURE_TEST_CASE(testDmiPtr, TestEnvironment) {
    testDmi("dmi.dmi_ptr", SIM_make_attr_uint64(5678), true);
    BOOST_CHECK_EQUAL(dmi_.get_dmi_ptr(),
                      reinterpret_cast<unsigned char*>(5678));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_ , 0);

    testDmi("dmi.dmi_ptr", SIM_make_attr_boolean(1), false);
    BOOST_CHECK(Stubs::instance_.SIM_log_error_count_ > 0);
}

BOOST_FIXTURE_TEST_CASE(testDmiStartAddress, TestEnvironment) {
    testDmi("dmi.start_address", SIM_make_attr_uint64(5768), true);
    BOOST_CHECK_EQUAL(dmi_.get_start_address(), 5768);
}

BOOST_FIXTURE_TEST_CASE(testDmiEndAddress, TestEnvironment) {
    testDmi("dmi.end_address", SIM_make_attr_uint64(8765), true);
    BOOST_CHECK_EQUAL(dmi_.get_end_address(), 8765);
}

BOOST_FIXTURE_TEST_CASE(testDmiReadLatency, TestEnvironment) {
    testDmi("dmi.read_latency", SIM_make_attr_uint64(8567), true);
    BOOST_CHECK_EQUAL(dmi_.get_read_latency(),
                      sc_core::sc_time::from_value(8567));
}

BOOST_FIXTURE_TEST_CASE(testDmiWriteLatency, TestEnvironment) {
    testDmi("dmi.write_latency", SIM_make_attr_uint64(7568), true);
    BOOST_CHECK_EQUAL(dmi_.get_write_latency(),
                      sc_core::sc_time::from_value(7568));
}

BOOST_AUTO_TEST_SUITE_END()
