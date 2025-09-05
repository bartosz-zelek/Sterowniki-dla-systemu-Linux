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

#include <simics/systemc/kernel.h>

BOOST_AUTO_TEST_SUITE(TestKernel)

class TestEnvironment {
  public:
    TestEnvironment()
        : context_(new sc_core::sc_simcontext), kernel_(context_) {
    }
    ~TestEnvironment() {
        delete context_;
    }

    sc_core::sc_simcontext *context_;
    simics::systemc::Kernel kernel_;
};

BOOST_FIXTURE_TEST_CASE(testStoreAndRestoreSimulationContext,
                        TestEnvironment) {
    {
        TestEnvironment environment;
        BOOST_CHECK_EQUAL(sc_core::sc_default_global_context,
                          environment.context_);
        BOOST_CHECK_EQUAL(sc_core::sc_curr_simcontext, environment.context_);
    }
    BOOST_CHECK_EQUAL(sc_core::sc_default_global_context, context_);
    BOOST_CHECK_EQUAL(sc_core::sc_curr_simcontext, context_);
}

BOOST_AUTO_TEST_SUITE_END()
