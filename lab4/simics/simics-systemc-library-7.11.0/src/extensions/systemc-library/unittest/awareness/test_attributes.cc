// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/awareness/attributes.h>

#include "mock/awareness/mock_attribute.h"

using simics::systemc::awareness::Attributes;

BOOST_AUTO_TEST_SUITE(TestAttributes)

BOOST_AUTO_TEST_CASE(testInitAttributes) {
    int key = Attributes::defineAttribute<
        unittest::awareness::MockAttribute>()->key();

    {
        Attributes attributes1;
        Attributes attributes2;

        attributes1.init();
        attributes2.init();

        unittest::awareness::MockAttribute *attribute1
            = attributes1.attribute<unittest::awareness::MockAttribute>(key);

        unittest::awareness::MockAttribute *attribute2
            = attributes2.attribute<unittest::awareness::MockAttribute>(key);

        BOOST_CHECK_EQUAL(attribute1->init_, 1);
        BOOST_CHECK_EQUAL(attribute2->init_, 2);

        BOOST_CHECK_NE(attribute1, attribute2);
    }

    BOOST_CHECK_EQUAL(unittest::awareness::MockAttribute::deleted_, 2);
}

BOOST_AUTO_TEST_SUITE_END()
