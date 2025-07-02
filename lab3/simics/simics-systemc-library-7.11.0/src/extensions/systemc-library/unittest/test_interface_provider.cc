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

#include <simics/base/types.h>

#include <simics/systemc/interface_provider.h>

#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestInterfaceProvider)

class MockInterface {
  public:
};

class MockInterface2 {
  public:
};

class MockInterface3 {
  public:
};

class TestEnvironment : public Environment {
  public:
    TestEnvironment() : obj_(&conf_), provider_("MockInterface") {
        register_interface("MockInterface", &mock_);
        register_interface("MockInterface2", &mock2_);
        register_interface("MockInterface3", &mock3_);
    }

    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    simics::systemc::InterfaceProvider provider_;
    MockInterface mock_;
    MockInterface2 mock2_;
    MockInterface3 mock3_;
};

BOOST_FIXTURE_TEST_CASE(testResetInterface, TestEnvironment) {
    provider_.set_target(obj_);
    BOOST_CHECK(provider_.get_interface<MockInterface>() == &mock_);
    TestEnvironment environment;
    provider_.set_target(environment.obj_);
    BOOST_CHECK(provider_.has_interface() == true);
    BOOST_CHECK(provider_.get_interface<MockInterface>() == &environment.mock_);
}

BOOST_FIXTURE_TEST_CASE(testSetNullObject, TestEnvironment) {
    simics::ConfObjectRef obj;
    BOOST_CHECK_NO_THROW(provider_.set_target(obj));
}

BOOST_FIXTURE_TEST_CASE(testGetNullObject, TestEnvironment) {
    Stubs stubs;
    simics::ConfObjectRef obj;
    provider_.set_target(obj);
    BOOST_CHECK(provider_.has_interface() == false);
    BOOST_CHECK(provider_.get_interface<MockInterface>() == NULL);
    BOOST_CHECK_EQUAL(0, Stubs::instance_.fatal_error_count_);
}

BOOST_FIXTURE_TEST_CASE(testGetInterfaceName, TestEnvironment) {
    BOOST_CHECK_EQUAL(provider_.get_interface_name(), "MockInterface");
}

BOOST_FIXTURE_TEST_CASE(testOptional, TestEnvironment) {
    BOOST_CHECK_EQUAL(provider_.optional(), false);
    provider_.set_optional(true);
    BOOST_CHECK_EQUAL(provider_.optional(), true);
    provider_.set_optional(false);
    BOOST_CHECK_EQUAL(provider_.optional(), false);
}

BOOST_FIXTURE_TEST_CASE(testInterfaceProviderAddon, TestEnvironment) {
    simics::systemc::InterfaceProviderAddOn addOn1(&provider_,
                                                   "MockInterface2");
    provider_.set_target(obj_);
    BOOST_CHECK(addOn1.has_interface() == true);
    BOOST_CHECK(addOn1.get_interface<MockInterface2>() == &mock2_);

    simics::systemc::InterfaceProviderAddOn addOn2(&provider_,
                                                   "MockInterface3");
    BOOST_CHECK(addOn2.has_interface() == true);
    BOOST_CHECK(addOn2.get_interface<MockInterface3>() == &mock3_);

    addOn1.set_optional(true);
    BOOST_CHECK_EQUAL(provider_.optional(), true);
    BOOST_CHECK_EQUAL(addOn1.optional(), true);
    BOOST_CHECK_EQUAL(addOn2.optional(), true);

    simics::ConfObjectRef obj;
    addOn1.set_target(obj);

    BOOST_CHECK(provider_.has_interface() == false);
    BOOST_CHECK(provider_.get_interface<MockInterface>() == NULL);
    BOOST_CHECK(addOn1.has_interface() == false);
    BOOST_CHECK(addOn1.get_interface<MockInterface2>() == NULL);
    BOOST_CHECK(addOn2.has_interface() == false);
    BOOST_CHECK(addOn2.get_interface<MockInterface3>() == NULL);
}

BOOST_AUTO_TEST_SUITE_END()
