// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/kernel_state_modifier.h>
#include "environment.h"
#include "mock/mock_simulation.h"
#include "stubs.h"

using simics::systemc::KernelStateModifier;

BOOST_AUTO_TEST_SUITE(TestKernelStateModifier)

BOOST_FIXTURE_TEST_CASE(testNoStateChange, Environment) {
    {
        KernelStateModifier modifier(&simulation_);
        sc_core::sc_start(0, sc_core::SC_PS);
    }
    {
        KernelStateModifier modifier(&simulation_);
    }
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testStateChanged, Environment) {
    {
        KernelStateModifier modifier(&simulation_);
        sc_core::sc_start(0, sc_core::SC_PS);
    }
    sc_core::sc_start(1, sc_core::SC_PS);
    {
        KernelStateModifier modifier(&simulation_);
    }
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_AUTO_TEST_SUITE_END()
