// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#define BOOST_TEST_MODULE SystemC_Checkpoint_Unittests

#include <boost/test/included/unit_test.hpp>
#include <boost/test/results_collector.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/test/unit_test_log.hpp>

#include <systemc>

class Formatter : public boost::unit_test::output::plain_report_formatter {
  public:
    virtual void do_confirmation_report(boost::unit_test::test_unit const& tu,
                                        std::ostream& ostr) {
        boost::unit_test::test_results const& tr =
            boost::unit_test::results_collector.results(tu.p_id);

        if (tr.passed()) {
            ostr << "No errors detected.\n";
            return;
        }
        plain_report_formatter::do_confirmation_report(tu, ostr);
    }
};

struct TestFixture {
    TestFixture() {
        boost::unit_test::results_reporter::set_format(new Formatter);

        // suppress the warnings that we know are safe during unittesting
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_NO_SC_START_ACTIVITY_, sc_core::SC_DO_NOTHING);
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_INSTANCE_EXISTS_, sc_core::SC_DO_NOTHING);
        sc_core::sc_report_handler::set_actions(
            sc_core::SC_ID_NO_DEFAULT_EVENT_, sc_core::SC_DO_NOTHING);
    }
};

BOOST_GLOBAL_FIXTURE(TestFixture);
