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

#ifndef SYSTEMC_LIBRARY_UNITTEST_FORMATTER_H
#define SYSTEMC_LIBRARY_UNITTEST_FORMATTER_H

#include <boost/test/results_collector.hpp>
#include <boost/test/results_reporter.hpp>
#include <boost/test/unit_test_log.hpp>

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

#endif
