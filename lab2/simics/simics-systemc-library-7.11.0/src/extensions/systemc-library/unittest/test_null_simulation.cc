// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>


#include <systemc>

#include <simics/systemc/null_simulation.h>
#include <simics/systemc/context.h>
#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestNullSimulation)

#if INTC_EXT  // Simulation class cannot be tested without support for multiple
              // contexts, which is disabled in the non-INTC_EXT version of
              // Simulation class in order to produce a more user-friendly
              // error message

BOOST_AUTO_TEST_CASE(testContextIsStopped) {
    unittest::SimContextWrapper wrapper;

    BOOST_CHECK_EQUAL(sc_core::sc_get_status(), sc_core::SC_ELABORATION);
    {
        simics::systemc::NullSimulation null;
        simics::systemc::Context c(&null);
        BOOST_CHECK_EQUAL(sc_core::sc_get_status(), sc_core::SC_STOPPED);
    }

    BOOST_CHECK_EQUAL(sc_core::sc_get_status(), sc_core::SC_ELABORATION);
}

#endif  // INTC_EXT

BOOST_AUTO_TEST_SUITE_END()
