// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>
#include <systemc>

#include <cci_configuration>
#include <cci_utils/broker.h>

#include <simics/systemc/awareness/attributes.h>
#include <simics/systemc/awareness/cci_attribute.h>

#include "environment.h"
#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestCciAttribute)

using cci::cci_param;
using cci::CCI_ABSOLUTE_NAME;
using cci::CCI_IMMUTABLE_PARAM;
using simics::systemc::awareness::CciAttribute;
using simics::systemc::awareness::Attributes;
using std::string;


class TestEnv : public Environment {
  public:
    TestEnv()
        : broker_("GLOBAL_BROKER"),
          originator_(cci::cci_originator("originator")),
          key_(-1),
          attribute_(NULL) {
        cci::cci_register_broker(&broker_);
    }
    void init() {
        key_ = CciAttribute::define("parameter name");
        attributes_.init();
        attribute_ = attributes_.attribute<CciAttribute>(key_);
    }
    ~TestEnv() {
        cci::cci_unregister_brokers();
    }

    unittest::SimContextWrapper wrapper_;
    cci_utils::broker broker_;
    cci::cci_originator originator_;
    Attributes attributes_;
    int key_;
    CciAttribute *attribute_;
};

BOOST_FIXTURE_TEST_CASE(testAttributeGet, TestEnv) {
    cci_param<string> p("parameter name", "value", "test parameter",
                        CCI_ABSOLUTE_NAME, originator_);
    init();
    attr_value_t attr = attribute_->get();

    BOOST_CHECK_EQUAL(SIM_attr_string(attr), "value");
}

BOOST_FIXTURE_TEST_CASE(testAttributeSet, TestEnv) {
    cci_param<string> p("parameter name", "value", "test parameter",
                        CCI_ABSOLUTE_NAME, originator_);
    init();
    attr_value_t attr = SIM_make_attr_string("new_value");

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Ok);
    BOOST_CHECK_EQUAL(p.get_value(), "new_value");
}

BOOST_FIXTURE_TEST_CASE(testAttributeGetInteger, TestEnv) {
    cci_param<int> p("parameter name", 1, "test parameter",
                     CCI_ABSOLUTE_NAME, originator_);
    init();
    attr_value_t attr = attribute_->get();

    BOOST_REQUIRE(SIM_attr_is_integer(attr));
    BOOST_CHECK_EQUAL(SIM_attr_integer(attr), 1);
}

BOOST_FIXTURE_TEST_CASE(testAttributeSetInteger, TestEnv) {
    cci_param<int> p("parameter name", 0, "test parameter", CCI_ABSOLUTE_NAME,
                     originator_);
    init();
    attr_value_t attr = SIM_make_attr_int64(1);

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Ok);
    BOOST_CHECK_EQUAL(p.get_value(), 1);
}

BOOST_FIXTURE_TEST_CASE(testAttributeSetLockedParameter, TestEnv) {
    cci_param<int> p("parameter name", 0, "test parameter", CCI_ABSOLUTE_NAME,
                     originator_);
    p.lock();
    init();
    attr_value_t attr = SIM_make_attr_int64(1);

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Not_Writable);
    BOOST_CHECK_EQUAL(p.get_value(), 0);
}

BOOST_FIXTURE_TEST_CASE(testAttributeSetImmutableParameter, TestEnv) {
    cci_param<int, CCI_IMMUTABLE_PARAM> p("parameter name", 0, "test parameter",
                                          CCI_ABSOLUTE_NAME, originator_);
    init();
    attr_value_t attr = SIM_make_attr_int64(1);

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Not_Writable);
    BOOST_CHECK_EQUAL(p.get_value(), 0);
}

BOOST_FIXTURE_TEST_CASE(testAttributeSetIllegalValue, TestEnv) {
    cci_param<string> p("parameter name", "value", "test parameter",
                        CCI_ABSOLUTE_NAME, originator_);
    init();
    attr_value_t attr = SIM_make_attr_int64(1);

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Illegal_Value);
    BOOST_CHECK_EQUAL(p.get_value(), "value");
}

BOOST_FIXTURE_TEST_CASE(testAttributeParameterDeleted, TestEnv) {
    {
        cci_param<int> p("parameter name", 0, "test parameter",
                         CCI_ABSOLUTE_NAME, originator_);
        init();
    }
    attr_value_t attr = SIM_make_attr_int64(1);

    BOOST_CHECK_EQUAL(attribute_->set(&attr), Sim_Set_Object_Not_Found);
}

BOOST_AUTO_TEST_SUITE_END()
