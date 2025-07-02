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
#include <simics/systemc/awareness/proxy_name.h>

#include "environment.h"

using simics::systemc::awareness::ProxyName;

BOOST_AUTO_TEST_SUITE(TestProxyName)

/* Simics Object name grammar
   name ::= part ( '.' part )*
   part ::= id ( '[' [0-9]+ ']' )?
   id   ::= [a-zA-Z_] [a-zA_Z0-9_]*
*/

// From the IEEE spec 5.17
// It shall be an error if a string name includes the period character (.)
// or any white-space characters.

class TestObject : public sc_core::sc_module {
  public:
    explicit TestObject(sc_core::sc_module_name name, const char* name2)
        : sc_core::sc_module(name), object_2_(NULL) {
        if (name2)
            object_2_ = new TestObject(name2, NULL);
    }
    virtual ~TestObject() {
        delete object_2_;
    }

    TestObject *object_2_;
};

class TestEnvironment : public Environment {
  public:
    TestEnvironment() : sc_object_(NULL) {}
    void init(const char *name, const char *name2 = NULL) {
        assert(sc_object_ == NULL);
        sc_object_ = new TestObject(name, name2);
        if (name2)
            proxy_name_ = ProxyName(sc_object_->object_2_->name());
        else
            proxy_name_ = ProxyName(sc_object_->name());
    }
    virtual ~TestEnvironment() {
        delete sc_object_;
    }

    TestObject *sc_object_;
    ProxyName proxy_name_;
};

BOOST_FIXTURE_TEST_CASE(test_no_transform, TestEnvironment) {
    init("test[0]");

    BOOST_CHECK_EQUAL(proxy_name_, "test[0]");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), false);
}

BOOST_FIXTURE_TEST_CASE(test_no_transform_port, TestEnvironment) {
    init("t0est[0]", "t1est[1]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est[0].t1est[1]");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), false);
}

BOOST_FIXTURE_TEST_CASE(test_error_prefix, TestEnvironment) {
    ProxyName::set_error_prefix_name("renamed.");
    init("t0est*");
    ProxyName::set_error_prefix_name(NULL);

    BOOST_CHECK_EQUAL(proxy_name_, "renamed.t0est_0x2A_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_error_prefix_port, TestEnvironment) {
    ProxyName::set_error_prefix_name("renamed.");
    init("t0est*", "t1est");
    ProxyName::set_error_prefix_name(NULL);

    BOOST_CHECK_EQUAL(proxy_name_, "renamed.t0est_0x2A_.t1est");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_invalid_in_middle, TestEnvironment) {
    init("t0e*st");

    BOOST_CHECK_EQUAL(proxy_name_, "t0e_0x2A_st");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_no_closing_bracket, TestEnvironment) {
    init("t0est[");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5B_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_no_closing_bracket_port, TestEnvironment) {
    init("t0est[", "t1est[");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5B_.t1est_0x5B_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_no_closing_bracket_with_number, TestEnvironment) {
    init("t0est[1");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5B_1");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_no_opening_bracket, TestEnvironment) {
    init("t0est]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5D_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_no_opening_bracket_with_number, TestEnvironment) {
    init("t0est1]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est1_0x5D_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_start_with_number, TestEnvironment) {
    init("0t0est[0]");

    BOOST_CHECK_EQUAL(proxy_name_, "_0x30_t0est[0]");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_start_with_number_port, TestEnvironment) {
    init("t0est[0]", "1t1est[1]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est[0]._0x31_t1est[1]");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_array, TestEnvironment) {
    init("t0est[A]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5B_A_0x5D_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_array_with_port, TestEnvironment) {
    init("t0est[A]", "t1est[B]");

    BOOST_CHECK_EQUAL(proxy_name_, "t0est_0x5B_A_0x5D_.t1est_0x5B_B_0x5D_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_array_in_middle, TestEnvironment) {
    init("t0e[0]st");

    BOOST_CHECK_EQUAL(proxy_name_, "t0e_0x5B_0_0x5D_st");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_double_opening_bracket, TestEnvironment) {
    init("t[[foo]]");

    BOOST_CHECK_EQUAL(proxy_name_, "t_0x5B5B_foo_0x5D5D_");
    BOOST_CHECK_EQUAL(proxy_name_.transformed(), true);
}

BOOST_AUTO_TEST_SUITE_END()
