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
#include <simics/systemc/awareness/proxy_attribute_name.h>

#include "environment.h"

using simics::systemc::awareness::ProxyAttributeName;

BOOST_AUTO_TEST_SUITE(TestProxyAttributeName)

/* Simics attribute name grammar
    name ::= [a-zA-Z_] [a-zA-Z0-9_]*
*/

BOOST_AUTO_TEST_CASE(test_no_transform) {
    ProxyAttributeName name("test");

    BOOST_CHECK_EQUAL(name, "test");
    BOOST_CHECK_EQUAL(name.transformed(), false);
}

BOOST_AUTO_TEST_CASE(test_invalid_at_end) {
    ProxyAttributeName name("t0est*");

    BOOST_CHECK_EQUAL(name, "t0est_0x2A_");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_CASE(test_invalid_in_middle) {
    ProxyAttributeName name("t0e*st");

    BOOST_CHECK_EQUAL(name, "t0e_0x2A_st");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_CASE(test_start_with_number) {
    ProxyAttributeName name("0t0est");

    BOOST_CHECK_EQUAL(name, "_0x30_t0est");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_CASE(test_array) {
    ProxyAttributeName name("t0est[0]");

    BOOST_CHECK_EQUAL(name, "t0est_0x5B_0_0x5D_");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_CASE(test_array_in_middle) {
    ProxyAttributeName name("t0e[0]st");

    BOOST_CHECK_EQUAL(name, "t0e_0x5B_0_0x5D_st");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_CASE(test_double_opening_bracket) {
    ProxyAttributeName name("t[[foo]]");

    BOOST_CHECK_EQUAL(name, "t_0x5B5B_foo_0x5D5D_");
    BOOST_CHECK_EQUAL(name.transformed(), true);
}

BOOST_AUTO_TEST_SUITE_END()
