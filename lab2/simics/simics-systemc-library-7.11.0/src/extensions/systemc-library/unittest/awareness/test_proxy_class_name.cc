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
#include <simics/systemc/awareness/proxy_class_name.h>

#include "environment.h"

using simics::systemc::awareness::ProxyClassName;

BOOST_AUTO_TEST_SUITE(TestProxyClassName)

/* Simics Class name grammar
   name ::= id ( '.' name )*
   id ::= [a-zA-Z_] [a-zA-Z0-9_-]*
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
    void init(const char *name) {
        assert(sc_object_ == NULL);
        std::string obj1(name);
        std::size_t pos = obj1.find(".");
        if (pos != std::string::npos) {
            std::string obj2 = obj1.substr(pos + 1);
            obj1 = obj1.substr(0, pos);
            sc_object_ = new TestObject(obj1.c_str(), obj2.c_str());
            proxy_class_name = ProxyClassName(sc_object_->object_2_->name());
        } else {
            sc_object_ = new TestObject(name, NULL);
            proxy_class_name = ProxyClassName(sc_object_->name());
        }
    }
    virtual ~TestEnvironment() {
        delete sc_object_;
    }

    TestObject *sc_object_;
    ProxyClassName proxy_class_name;
};

BOOST_FIXTURE_TEST_CASE(test_no_transform, TestEnvironment) {
    init("_-9.test");

    BOOST_CHECK_EQUAL(proxy_class_name, "_-9.test");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), false);
}

BOOST_FIXTURE_TEST_CASE(test_star_in_middle, TestEnvironment) {
    init("t0e*st");

    BOOST_CHECK_EQUAL(proxy_class_name, "t0e_0x2A_st");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_closing_bracket_middle, TestEnvironment) {
    init("t-e]st");

    BOOST_CHECK_EQUAL(proxy_class_name, "t-e_0x5D_st");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_opening_bracket_middle, TestEnvironment) {
    init("t0e[st");

    BOOST_CHECK_EQUAL(proxy_class_name, "t0e_0x5B_st");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_closing_bracket_end, TestEnvironment) {
    init("t-est[");

    BOOST_CHECK_EQUAL(proxy_class_name, "t-est_0x5B_");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_opening_bracket_end, TestEnvironment) {
    init("t-est]");

    BOOST_CHECK_EQUAL(proxy_class_name, "t-est_0x5D_");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_start_with_number, TestEnvironment) {
    init("0t0est");

    BOOST_CHECK_EQUAL(proxy_class_name, "_0x30_t0est");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_start_with_dash, TestEnvironment) {
    init("--0est");

    BOOST_CHECK_EQUAL(proxy_class_name, "_0x2D_-0est");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_array_end, TestEnvironment) {
    init("t0est[A]");

    BOOST_CHECK_EQUAL(proxy_class_name, "t0est_0x5B_A_0x5D_");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_FIXTURE_TEST_CASE(test_array_in_middle, TestEnvironment) {
    init("t-e[0]st");

    BOOST_CHECK_EQUAL(proxy_class_name, "t-e_0x5B_0_0x5D_st");
    BOOST_CHECK_EQUAL(proxy_class_name.transformed(), true);
}

BOOST_AUTO_TEST_SUITE_END()
