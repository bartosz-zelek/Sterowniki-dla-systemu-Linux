// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#if INTC_EXT

#include <boost/test/unit_test.hpp>

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/late_binder.h>

#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestLateBinder)

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") {
    }

    // sockets
    tlm_utils::simple_initiator_socket<TestModule> initiator;
    tlm_utils::simple_target_socket<TestModule> target;
    tlm_utils::simple_initiator_socket<TestModule, 64> initiator_64;
    tlm_utils::simple_target_socket<TestModule, 64> target_64;

    // signals
    sc_core::sc_in<bool> in_bool;
    sc_core::sc_in<int> in_int;
    sc_core::sc_in<double> in_double;
    sc_core::sc_in<sc_dt::uint64> in_uint64;
    sc_core::sc_in<sc_dt::sc_logic> in_logic;

    sc_core::sc_out<bool> out_bool;
    sc_core::sc_out<int> out_int;
    sc_core::sc_out<double> out_double;
    sc_core::sc_out<sc_dt::uint64> out_uint64;
    sc_core::sc_out<sc_dt::sc_logic> out_logic;

    // Actually this should be the same as out
    sc_core::sc_inout<bool> inout_bool;
    sc_core::sc_inout<int> inout_int;
    sc_core::sc_inout<double> inout_double;
    sc_core::sc_inout<sc_dt::uint64> inout_uint64;
    sc_core::sc_inout<sc_dt::sc_logic> inout_logic;
};

// begin copy from test_simulation.cc
static int report_display_counter = 0;
static int report_log_counter = 0;
static int report_stop_counter = 0;
static int report_interrupt_counter = 0;
static int report_abort_counter = 0;
static int report_throw_counter = 0;

static void MockReportHandlerClear() {
    report_display_counter = 0;
    report_log_counter = 0;
    report_stop_counter = 0;
    report_interrupt_counter = 0;
    report_abort_counter = 0;
    report_throw_counter = 0;
}
static void MockReportHandler(const sc_core::sc_report &report,
                              const sc_core::sc_actions &actions) {
    if (sc_core::SC_DISPLAY & actions) {
        ++report_display_counter;
    }

    if (sc_core::SC_LOG & actions) {
        ++report_log_counter;
    }

    if (sc_core::SC_STOP & actions) {
        ++report_stop_counter;
        sc_core::sc_stop();
    }

    if (sc_core::SC_INTERRUPT & actions) {
        ++report_interrupt_counter;
    }

    if (sc_core::SC_ABORT & actions) {
        ++report_abort_counter;
    }

    if (sc_core::SC_THROW & actions) {
        ++report_throw_counter;
        throw report;
    }
}
// end copy

class TestEnvironment {
  public:
    TestEnvironment() {
        sc_core::sc_report_handler::set_handler(MockReportHandler);
        MockReportHandlerClear();
    }
    ~TestEnvironment() {
        sc_core::sc_report_handler::set_handler(NULL);
    }

    unittest::SimContextWrapper context_;
};

BOOST_FIXTURE_TEST_CASE(testLateBinder, TestEnvironment) {
    TestModule tb;
    simics::systemc::LateBinder lb;
    sc_core::sc_get_curr_simcontext()->set_late_binder(&lb);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    // unbound ports are expected to be reported by the kernel,
    // test 4 socket + 3 signals * 5 types are reported
    BOOST_CHECK_EQUAL(report_log_counter, 4 + 3 * 5);
    MockReportHandlerClear();

    // Test UnconnectedSockets
    sc_core::sc_time t;
    tlm::tlm_generic_payload gp;
    tlm::tlm_phase phase;
    tlm::tlm_dmi dmi_data;
    tb.initiator->nb_transport_fw(gp, phase, t);
    tb.initiator->b_transport(gp, t);
    tb.initiator->b_transport(gp, t);
    tb.initiator->get_direct_mem_ptr(gp, dmi_data);
    tb.initiator->transport_dbg(gp);
    tb.target->nb_transport_bw(gp, phase, t);
    tb.target->invalidate_direct_mem_ptr(0, 0);
    BOOST_CHECK_EQUAL(report_log_counter, 7);
    MockReportHandlerClear();

    // Test UnconnectedSignal
    tb.out_bool.read();
    tb.out_bool.default_event();
    tb.out_bool.value_changed_event();
    tb.out_bool.event();
    tb.out_bool.posedge_event();
    tb.out_bool.negedge_event();
    tb.out_bool.posedge();
    tb.out_bool.negedge();
    tb.out_bool.write(true);
    BOOST_CHECK_EQUAL(report_log_counter, 9);
    MockReportHandlerClear();
}

BOOST_FIXTURE_TEST_CASE(testNoLateBinder, TestEnvironment) {
    TestModule tb;
    // expect "Error: (E109) complete binding failed: port not bound" exception
    BOOST_CHECK_THROW(sc_core::sc_start(sc_core::SC_ZERO_TIME),
                      std::exception);
}

BOOST_AUTO_TEST_SUITE_END()
#endif  // INTC_EXT
