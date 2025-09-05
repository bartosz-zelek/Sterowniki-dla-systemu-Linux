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

#if defined(__GNUC__) && __GNUC__ >= 4 && (__GNUC__ >= 5 || __GNUC_MINOR__ >= 2)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#define BOOST_TEST_MODULE SystemC_Library_Integration

#include <systemc>

#include <boost/test/included/unit_test.hpp>

#include "formatter.h"

struct TestFixture {
    TestFixture() {
        boost::unit_test::results_reporter::set_format(new Formatter);

        // suppress the warnings that we know are safe during unittesting
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_NO_SC_START_ACTIVITY_, sc_core::SC_DO_NOTHING);
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_INSTANCE_EXISTS_, sc_core::SC_DO_NOTHING);
    }
};

BOOST_GLOBAL_FIXTURE(TestFixture);
