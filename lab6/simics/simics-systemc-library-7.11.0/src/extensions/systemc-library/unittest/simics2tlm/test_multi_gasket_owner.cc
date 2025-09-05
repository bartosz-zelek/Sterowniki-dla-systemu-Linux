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

#include <simics/systemc/simics2tlm/null_gasket.h>
#include <simics/systemc/simics2tlm/multi_gasket_owner.h>
#include <simics/systemc/simics2tlm/gasket.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::simics2tlm::Gasket;

BOOST_AUTO_TEST_SUITE(TestSimics2TlmMultiGasketOwner)

using simics::systemc::simics2tlm::GasketInterface;

class TestEnvironment {
  public:
    TestEnvironment() {
        gasket_ = GasketInterface::Ptr(
                new Gasket<>("gasket", simulation_.simics_object()));
    }
    unittest::MockSimulation simulation_;
    GasketInterface::Ptr gasket_;
};

class MockInterface {
  public:
    virtual ~MockInterface() {}
};

class MockInterfaceNotImplemented {
  public:
    virtual ~MockInterfaceNotImplemented() {}
};

class MockOwner : public simics::systemc::simics2tlm::MultiGasketOwner,
                  public MockInterface {
  public:
    MockOwner() {
        set_type();
    }
};

BOOST_FIXTURE_TEST_CASE(testAddGasket, TestEnvironment) {
    simics::systemc::simics2tlm::MultiGasketOwner owner;
    BOOST_CHECK_EQUAL(simics::systemc::simics2tlm::NullGasket::instances(), 1);
    owner.addGasket(1, gasket_);
    BOOST_CHECK_EQUAL(simics::systemc::simics2tlm::NullGasket::instances(), 0);
    BOOST_CHECK_EQUAL(owner.findGasket(1), gasket_);
}

BOOST_FIXTURE_TEST_CASE(testSetTypeOfGasket, TestEnvironment) {
    MockOwner owner;
    owner.addGasket(1, gasket_);
    BOOST_CHECK(owner == *owner.findGasket(1)->type());
}

BOOST_FIXTURE_TEST_CASE(testGetInterface, TestEnvironment) {
    TestEnvironment t2;
    MockOwner owner;
    owner.addGasket(1, gasket_);
    owner.addGasket(2, t2.gasket_);
    BOOST_CHECK(gasket_->type()->get_interface<MockInterface>() == &owner);
    BOOST_CHECK(gasket_->type()->get_interface<MockInterfaceNotImplemented>()
        == NULL);
    BOOST_CHECK(t2.gasket_->type()->get_interface<MockInterface>() == &owner);
    BOOST_CHECK(t2.gasket_->type()->get_interface<MockInterfaceNotImplemented>()
        == NULL);
}

BOOST_FIXTURE_TEST_CASE(testGetKeys, TestEnvironment) {
    TestEnvironment t2;
    MockOwner owner;
    owner.addGasket(1, gasket_);
    owner.addGasket(2, t2.gasket_);
    std::set<int> s = owner.keys();
    BOOST_CHECK(s.size() == 2);
    BOOST_CHECK(s.find(1) != s.end());
    BOOST_CHECK(s.find(2) != s.end());
}

BOOST_AUTO_TEST_SUITE_END()
