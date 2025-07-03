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

#include <simics/systemc/simics2tlm/null_gasket.h>
#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/gasket.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::simics2tlm::Gasket;
using simics::systemc::simics2tlm::GasketInterface;

BOOST_AUTO_TEST_SUITE(TestSimics2TlmGasketOwner)

class MockGasket : public simics::systemc::simics2tlm::Gasket<> {
  public:
    MockGasket(sc_core::sc_module_name name,
               const simics::ConfObjectRef &obj) : Gasket<>(name, obj) {
        ++instance_counter_;
    }
    virtual ~MockGasket() {
        --instance_counter_;
    }
    static int instance_counter_;
};

int MockGasket::instance_counter_ = 0;

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

class MockOwner : public simics::systemc::simics2tlm::GasketOwner,
                  public MockInterface {
  public:
    MockOwner() {
        set_type();
    }
};

BOOST_FIXTURE_TEST_CASE(testSetGasket, TestEnvironment) {
    simics::systemc::simics2tlm::GasketOwner owner;
    BOOST_CHECK_EQUAL(simics::systemc::simics2tlm::NullGasket::instances(), 1);

    owner.set_gasket(gasket_);
    BOOST_CHECK_EQUAL(simics::systemc::simics2tlm::NullGasket::instances(), 0);
}

BOOST_AUTO_TEST_CASE(testSetGasketToNULL) {
    Stubs stubs;
    simics::systemc::simics2tlm::GasketOwner owner;

    BOOST_CHECK_THROW(owner.set_gasket(NULL), std::exception);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.fatal_error_count_);
}

BOOST_FIXTURE_TEST_CASE(testSetTypeOfGasket, TestEnvironment) {
    MockOwner owner;
    owner.set_gasket(gasket_);

    BOOST_CHECK(owner == *owner.gasket()->type());
}

BOOST_FIXTURE_TEST_CASE(testGetInterface, TestEnvironment) {
    MockOwner owner;
    owner.set_gasket(gasket_);
    BOOST_CHECK(gasket_->type()->get_interface<MockInterface>() == &owner);
    BOOST_CHECK(gasket_->type()->get_interface<MockInterfaceNotImplemented>()
        == NULL);
}

BOOST_FIXTURE_TEST_CASE(testMultipleSetGasket, TestEnvironment) {
    {
        GasketInterface::Ptr mock_gasket_ = GasketInterface::Ptr(
            new MockGasket("gasket", simulation_.simics_object()));
        {
            simics::systemc::simics2tlm::GasketOwner owner;
            owner.set_gasket(mock_gasket_);
            {
                simics::systemc::simics2tlm::GasketOwner owner;
                owner.set_gasket(mock_gasket_);
                BOOST_CHECK_EQUAL(MockGasket::instance_counter_, 1);
            }
            BOOST_CHECK_EQUAL(MockGasket::instance_counter_, 1);
        }
        BOOST_CHECK_EQUAL(MockGasket::instance_counter_, 1);
    }
    BOOST_CHECK_EQUAL(MockGasket::instance_counter_, 0);
}

BOOST_AUTO_TEST_SUITE_END()
