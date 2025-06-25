// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/types/any_type.h>
#include <vector>

using simics::types::AnyType;

BOOST_AUTO_TEST_SUITE(TestTypesAnyType)

BOOST_AUTO_TEST_CASE(TestGetValue) {
    AnyType t1;
    int v = 5;
    t1 = v;

    BOOST_CHECK_EQUAL(t1.value<int>(), 5);
}

BOOST_AUTO_TEST_CASE(TestCopyConstructor) {
    AnyType t1;
    int v = 5;
    t1 = v;
    AnyType t2(t1);

    BOOST_CHECK_EQUAL(t1.value<int>(), 5);
    BOOST_CHECK_EQUAL(t2.value<int>(), 5);
}

BOOST_AUTO_TEST_CASE(TestAssignment) {
    AnyType t1;
    int v = 5;
    t1 = v;
    AnyType t2 = t1;

    BOOST_CHECK_EQUAL(t1.value<int>(), 5);
    BOOST_CHECK_EQUAL(t2.value<int>(), 5);
}

BOOST_AUTO_TEST_SUITE_END()
