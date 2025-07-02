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

#include <simics/systemc/tlm2simics/null_gasket.h>
#include <simics/systemc/tlm2simics/gasket_owner.h>
#include <simics/systemc/tlm2simics/gasket.h>

#include "environment.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsGasketOwner)

using simics::systemc::tlm2simics::GasketInterface;

BOOST_FIXTURE_TEST_CASE(testSetGasket, Environment) {
    conf_object_t conf;
    simics::ConfObjectRef object(&conf);
    simics::systemc::tlm2simics::GasketOwner owner;
    BOOST_CHECK_EQUAL(simics::systemc::tlm2simics::NullGasket::instances(), 1);

    owner.set_gasket(GasketInterface::Ptr(
        new simics::systemc::tlm2simics::Gasket<>("gasket", object)));
    BOOST_CHECK_EQUAL(simics::systemc::tlm2simics::NullGasket::instances(), 0);
}

BOOST_AUTO_TEST_CASE(testSetGasketToNULL) {
    Stubs stubs;
    simics::systemc::tlm2simics::GasketOwner owner;

    BOOST_CHECK_THROW(owner.set_gasket(NULL), std::exception);
    BOOST_CHECK_EQUAL(1, Stubs::instance_.fatal_error_count_);
}

BOOST_AUTO_TEST_SUITE_END()
