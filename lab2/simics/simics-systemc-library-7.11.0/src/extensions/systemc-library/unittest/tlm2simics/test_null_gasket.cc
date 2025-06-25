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

BOOST_AUTO_TEST_SUITE(TestTlm2SimicsNullGasket)

BOOST_AUTO_TEST_CASE(testInstances) {
    {
        simics::systemc::tlm2simics::NullGasket gasket;
        BOOST_CHECK_EQUAL(
            simics::systemc::tlm2simics::NullGasket::instances(), 1);
    }
    BOOST_CHECK_EQUAL(simics::systemc::tlm2simics::NullGasket::instances(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
