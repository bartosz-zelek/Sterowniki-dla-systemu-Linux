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

#include <simics/systemc/kernel.h>
#include <simics/systemc/report_handler.h>

#include "mock/mock_simulation.h"
#include "stubs.h"

BOOST_AUTO_TEST_SUITE(TestReportHandler)

using std::string;
using simics::systemc::ReportHandler;

class TestEnvironment {
  public:
    TestEnvironment() {
        simics::systemc::ReportHandler::init();
    }
    void check(conf_object_t* obj, const char* msg) {
        BOOST_CHECK_EQUAL(Stubs::instance_.last_log_obj_, obj);
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 1);
        BOOST_CHECK(Stubs::instance_.SIM_log_info_.find(msg) != string::npos);
        // Reset stub call information
        Stubs();
    }
    Stubs stubs_;
};

BOOST_FIXTURE_TEST_CASE(testInit, TestEnvironment) {
    SC_REPORT_INFO("intel/test", "global");
    check(&Stubs::instance_.sim_obj_, "global");
}

BOOST_FIXTURE_TEST_CASE(testSettingReportHandler, TestEnvironment) {
    conf_object_t o1;
    simics::ConfObjectRef r1(&o1);
    {
        ReportHandler h(r1);
        SC_REPORT_INFO("intel/test", "m1");
        check(&o1, "m1");
    }

    SC_REPORT_INFO("intel/test", "global");
    check(&Stubs::instance_.sim_obj_, "global");
}

BOOST_FIXTURE_TEST_CASE(testSettingMultipleReportHandlers, TestEnvironment) {
    conf_object_t o1;
    simics::ConfObjectRef r1(&o1);
    {
        ReportHandler h(r1);
        conf_object_t o2;
        simics::ConfObjectRef r2(&o2);
        {
            ReportHandler h(r2);
            SC_REPORT_INFO("intel/test", "m2");
            check(&o2, "m2");
        }
        SC_REPORT_INFO("intel/test", "m1");
        check(&o1, "m1");
    }

    SC_REPORT_INFO("intel/test", "global");
    check(&Stubs::instance_.sim_obj_, "global");
}

BOOST_FIXTURE_TEST_CASE(testSettingNullObject, TestEnvironment) {
    simics::ConfObjectRef r1;
    {
        ReportHandler h(r1);
        SC_REPORT_INFO("intel/test", "m1");
        BOOST_CHECK_EQUAL(Stubs::instance_.last_log_obj_,
                          &Stubs::instance_.sim_obj_);
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
        const char *msg = "Unable to locate SystemC adapter. Context not set.";
        BOOST_CHECK(Stubs::instance_.SIM_log_error_.find(msg) != string::npos);
    }

    SC_REPORT_INFO("intel/test", "global");
    check(&Stubs::instance_.sim_obj_, "global");
}

BOOST_AUTO_TEST_SUITE_END()
