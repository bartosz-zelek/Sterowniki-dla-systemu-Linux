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

#include <simics/systemc/reference_counter.h>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestReferenceCounter)

using simics::systemc::ReferenceCounter;

class TestClass {
  public:
};

BOOST_AUTO_TEST_CASE(testConstructorAndDestructor) {
    TestClass instance;
    ReferenceCounter<TestClass> counter1(&instance);

    BOOST_CHECK_EQUAL(1, counter1.references());

    {
        ReferenceCounter<TestClass> counter2(&instance);
        BOOST_CHECK_EQUAL(2, counter1.references());
        BOOST_CHECK_EQUAL(2, counter2.references());
    }

    BOOST_CHECK_EQUAL(1, counter1.references());
}

BOOST_AUTO_TEST_CASE(testCopyConstructor) {
    TestClass instance;
    ReferenceCounter<TestClass> counter(&instance);

    {
        std::vector<ReferenceCounter<TestClass> > references {counter};
        BOOST_CHECK_EQUAL(2, counter.references());
    }

    BOOST_CHECK_EQUAL(1, counter.references());
}

BOOST_AUTO_TEST_CASE(testAssignmentOperator) {
    TestClass instance1;
    TestClass instance2;

    ReferenceCounter<TestClass> counter1(&instance1);
    ReferenceCounter<TestClass> counter2(&instance2);
    ReferenceCounter<TestClass> counter3(&instance2);

    BOOST_CHECK_EQUAL(1, counter1.references());
    BOOST_CHECK_EQUAL(2, counter2.references());
    BOOST_CHECK_EQUAL(2, counter3.references());
    BOOST_CHECK_EQUAL(&instance1, counter1.operator->());
    BOOST_CHECK_EQUAL(&instance2, counter2.operator->());
    BOOST_CHECK_EQUAL(&instance2, counter3.operator->());

    counter2 = counter1;

    BOOST_CHECK_EQUAL(2, counter1.references());
    BOOST_CHECK_EQUAL(2, counter2.references());
    BOOST_CHECK_EQUAL(1, counter3.references());
    BOOST_CHECK_EQUAL(&instance1, counter1.operator->());
    BOOST_CHECK_EQUAL(&instance1, counter2.operator->());
    BOOST_CHECK_EQUAL(&instance2, counter3.operator->());

    counter1 = counter1;

    BOOST_CHECK_EQUAL(2, counter1.references());
    BOOST_CHECK_EQUAL(&instance1, counter1.operator->());
}

BOOST_AUTO_TEST_CASE(testArrowOperator) {
    TestClass instance;
    ReferenceCounter<TestClass> counter(&instance);

    BOOST_CHECK_EQUAL(&instance, counter.operator->());
}

BOOST_AUTO_TEST_CASE(testConversion) {
    TestClass instance;
    ReferenceCounter<TestClass> counter(&instance);
    std::vector<ReferenceCounter<TestClass> > references {counter};

    TestClass *instance_ptr = references[0];
    BOOST_CHECK_EQUAL(instance_ptr, &instance);
}

BOOST_AUTO_TEST_SUITE_END()
