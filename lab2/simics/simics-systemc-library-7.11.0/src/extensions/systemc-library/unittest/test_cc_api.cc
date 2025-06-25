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

#include <simics/conf-object.h>

BOOST_AUTO_TEST_SUITE(TestCCApi)

BOOST_AUTO_TEST_CASE(ConfObjectRefCompare) {
    conf_object_t o1;
    conf_object_t o2;
    simics::ConfObjectRef ref1(&o1);
    simics::ConfObjectRef ref2(&o2);
    simics::ConfObjectRef ref3(&o1);
    simics::ConfObjectRef null_ref(nullptr);

    BOOST_CHECK_EQUAL(ref1, ref3);
    BOOST_CHECK_NE(ref1, ref2);
    BOOST_CHECK_NE(ref1, null_ref);
    BOOST_CHECK_NE(null_ref, ref1);

    conf_object_t *o = &o1;
    BOOST_CHECK_EQUAL(ref1.object(), o);
    BOOST_CHECK_NE(ref2.object(), o);
}

BOOST_AUTO_TEST_CASE(ConfObjectRefConfObjectRefCompare) {
    conf_object_t o1;
    conf_object_t o2;
    simics::ConfObjectRef ref1(&o1);
    simics::ConfObjectRef pref1(&o1);
    simics::ConfObjectRef ref2(&o2);
    BOOST_CHECK_EQUAL(ref1, pref1);
    BOOST_CHECK_EQUAL(pref1, ref1);
    BOOST_CHECK_NE(pref1, ref2);
    BOOST_CHECK_NE(ref2, pref1);
}

BOOST_AUTO_TEST_SUITE_END()
