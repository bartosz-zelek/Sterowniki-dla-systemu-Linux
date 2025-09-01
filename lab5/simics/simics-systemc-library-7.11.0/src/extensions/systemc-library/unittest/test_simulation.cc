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
#include <boost/core/noncopyable.hpp>


#include <systemc>
#include <simics/systemc/device.h>
#include <simics/systemc/internal.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <simics/systemc/simulation.h>
#include <simics/systemc/simics2tlm/gasket.h>

#include <vector>

#include "mock/mock_trace_monitor.h"
#include "stubs.h"
#include "environment.h"

BOOST_AUTO_TEST_SUITE(TestSimulation)

#if INTC_EXT  // Simulation class cannot be tested without support for multiple
              // contexts, which is disabled in the non-INTC_EXT version of
              // Simulation class in order to produce a more user-friendly
              // error message

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

class MockScThread : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(MockScThread);
#endif
    explicit MockScThread(sc_core::sc_module_name = "MockThread")
        : count_(0),
          run_once_(false), delay_(1),
          active_at_current_time_(true) {
        SC_THREAD(loop);
        sensitive << event_;
    }

    void triggerEvent(const sc_dt::uint64& delay) {
        event_.notify(sc_core::sc_time::from_value(delay));
    }
    void loop() {
        while (true) {
            event_.notify(
                sc_core::sc_time::from_value(
                    static_cast<sc_dt::uint64>(
                        active_at_current_time_ ? 0 : delay_)));
            active_at_current_time_ = false;
            wait();
            ++count_;
            action();
            if (run_once_)
                break;
        }
    }
    virtual void action() {
    }
    sc_core::sc_event event_;
    int count_;
    bool run_once_;
    sc_dt::uint64 delay_;
    bool active_at_current_time_;
};

class MockScThreadUserException : public MockScThread {
  public:
    void action() /* override */ {
        throw 0;
    }
};

class MockScThreadScHaltException : public MockScThread {
  public:
    void action() /* override */ {
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_halt());
    }
};

class MockScThreadKill : public MockScThread {
  public:
    void action() /* override */ {
        sc_core::sc_get_current_process_handle().kill();
    }
};

class MockScThreadPause : public MockScThread {
  public:
    void action() /* override */ {
        sc_core::sc_pause();
    }
};

class MockScThreadStart : public MockScThread {
  public:
    void action() /* override */ {
        sc_core::sc_start();
    }
};

class MockScThreadRunDeltaPhase : public MockScThread {
  public:
    void action() /* override */ {
        run_once_ = true;
        triggerEvent(0);
        simulation_->runDeltaPhase(0);
    }
    simics::systemc::Simulation *simulation_;
};

class TestModule : public sc_core::sc_module {
  public:
    explicit TestModule(sc_core::sc_module_name = "TestModule") : value_(0) {
    }
    void set_value(int value) {
        value_ = value;
    }
    int get_value() const {
        return value_;
    }

    ~TestModule() {
    }
    int value_;
};

class TestDevice : public sc_core::sc_module,
                   public tlm::tlm_fw_transport_if<> {
  public:
    explicit TestDevice(sc_core::sc_module_name = "TestDevice")
        : target_socket_("target_socket"), response_(tlm::TLM_OK_RESPONSE) {
            target_socket_.bind(*this);
    }
    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        action(trans);
        trans.set_response_status(response_);
    }
    virtual void action(tlm::tlm_generic_payload &trans) {  // NOLINT
    }
    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload &trans,  // NOLINT
                                       tlm::tlm_phase &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return tlm::TLM_COMPLETED;
    }
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans,  // NOLINT
                                    tlm::tlm_dmi &dmi_data) {  // NOLINT
        return true;
    }
    unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {  // NOLINT
        return 0;
    }

    tlm::tlm_target_socket<> target_socket_;
    tlm::tlm_response_status response_;
};

// To get access to Simulation::finalize() and instances()
class TestSimulation : public simics::systemc::Simulation {
  public:
    explicit TestSimulation(simics::ConfObjectRef o)
        : Simulation(o, &internal_), internal_(this) {
    }
    virtual ~TestSimulation() = default;
    using Simulation::finalize;
    using Simulation::instances;
    simics::systemc::Internal internal_;
};

struct event_handler_interface_t {
    bool (*handle_event)(conf_object_t *NOTNULL obj);

    event_handler_interface_t() {
        handle_event = &handle_event_impl;
        callHandleEventCount_ = 0;
    }
    static bool handle_event_impl(conf_object_t *obj) {
        ++callHandleEventCount_;
        return true;
    }
    static int callHandleEventCount_;
};

int event_handler_interface_t::callHandleEventCount_ = 0;

class TestEnvironmentBase :
    private boost::noncopyable, public EnvironmentBase  {
  public:
    TestEnvironmentBase()
        : obj_(&conf_), simulation_(obj_) {
        conf_.instance_data = &simulation_;
        sc_core::sc_report_handler::set_handler(MockReportHandler);
        MockReportHandlerClear();
        trace_monitor_ = new unittest::MockTraceMonitor;
        register_interface("event_handler", &event_handler_);
        Stubs::instance_.SIM_object_descendant_ = &conf_;
        Stubs::instance_.SIM_port_object_parent_ = &conf_;
    }
    virtual ~TestEnvironmentBase() {
        sc_core::sc_report_handler::set_handler(NULL);
        delete trace_monitor_;
    }
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    TestSimulation simulation_;
    Stubs stubs_;
    simics::systemc::TraceMonitorInterface *trace_monitor_;
    event_handler_interface_t event_handler_;
};

template<class T>
class TestEnvironment : public TestEnvironmentBase {
  public:
    TestEnvironment() {
        simulation_.finalize();
        simulation_.set_delta(NULL, NULL, 10);
    }

    T sc_thread_;
};

class ContextSanity {
  public:
    ContextSanity() {
        context_ = sc_core::sc_get_curr_simcontext();
    }
    ~ContextSanity() {
        if (context_ != sc_core::sc_get_curr_simcontext()) {
            std::cerr << "Error: leaving with a different context "
                      << "than what we started with" << std::endl;
            exit(1);
        }
    }

  private:
    sc_core::sc_simcontext *context_;
};

BOOST_GLOBAL_FIXTURE(ContextSanity);

BOOST_FIXTURE_TEST_CASE(testNewSimulation, TestEnvironment<MockScThread>) {
    simulation_.run();

    BOOST_CHECK_EQUAL(sc_thread_.count_, 11);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_break_simulation_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testBreakSimulation,
                        TestEnvironment<MockScThreadPause>) {
    sc_thread_.active_at_current_time_ = false;
    sc_thread_.delay_ = 5;
    simulation_.run();

    BOOST_CHECK_EQUAL(sc_thread_.count_, 1);
    BOOST_CHECK_EQUAL(simulation_.get_delta(NULL), 5);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_break_simulation_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testBreakSimulationInDeltaPhase,
                        TestEnvironment<MockScThreadPause>) {
    simulation_.runDeltaPhase(0);

    BOOST_CHECK_EQUAL(sc_thread_.count_, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_break_simulation_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testNoRemaingingDeltaEventsAfterRun,
                        TestEnvironment<MockScThread>) {
    sc_thread_.active_at_current_time_ = false;
    sc_thread_.delay_ = 5;
    simulation_.set_delta(NULL, NULL, 5);
    simulation_.run();

    BOOST_CHECK_EQUAL(sc_thread_.count_, 1);
    BOOST_CHECK_EQUAL(simulation_.get_delta(NULL), 0);
}

BOOST_FIXTURE_TEST_CASE(testSetDeltaWhenSystemCNotRunning,
                        TestEnvironment<MockScThread>) {
    sc_start(1, sc_core::SC_PS);

    simulation_.set_delta(NULL, NULL, 1);

    // set_delta is invoked when we call handleEvent.
    // We are checking for the call.
    BOOST_CHECK_EQUAL(event_handler_interface_t::callHandleEventCount_, 0);

    // Gasket invokes runSimulation when SystemC is not running.
    simulation_.runSimulation(sc_core::sc_max_time());

    // set_delta would be called from the co-engine.
    // We can run the simulation with the newly set delta value.
    BOOST_CHECK_EQUAL(event_handler_interface_t::callHandleEventCount_, 1);
}

BOOST_FIXTURE_TEST_CASE(testRunAfterScStop,
                        TestEnvironment<MockScThread>) {
    simulation_.run();
    sc_core::sc_stop();
    MockReportHandlerClear();

    simulation_.set_delta(NULL, NULL, 10);
    BOOST_CHECK_NO_THROW(simulation_.run());
    BOOST_CHECK_EQUAL(report_throw_counter, 0);
    BOOST_CHECK_EQUAL(report_log_counter, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
    BOOST_CHECK_EQUAL(simulation_.get_delta(NULL), 10);
}

BOOST_FIXTURE_TEST_CASE(testRunDeltaPhaseAfterScStop,
                        TestEnvironment<MockScThread>) {
    simulation_.run();
    sc_core::sc_stop();
    MockReportHandlerClear();

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(report_throw_counter, 0);
    BOOST_CHECK_EQUAL(report_log_counter, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

class MockScThreadLoopForever : public sc_core::sc_module {
  public:
    typedef MockScThreadLoopForever SC_CURRENT_USER_MODULE;
    MockScThreadLoopForever() : sc_core::sc_module(
            sc_core::sc_module_name("MockScThreadLoopForever")), count_(0) {
        SC_THREAD(loop);
        sensitive << event_;
        dont_initialize();
    }
    void loop() {
        while (true) {
            event_.notify(0, sc_core::SC_PS);
            wait();
            ++count_;
        }
    }
    sc_core::sc_event event_;
    int count_;
};

BOOST_FIXTURE_TEST_CASE(testRunDeltaPhaseBreak,
                        TestEnvironment<MockScThreadLoopForever>) {
    simulation_.run();
    {
        simics::systemc::KernelStateModifier modifier(&simulation_);
        sc_thread_.event_.notify();
    }

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(sc_thread_.count_, 10000000);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testScStartAfterScStop,
                        TestEnvironment<MockScThread>) {
    simulation_.run();
    sc_core::sc_stop();

    BOOST_CHECK_THROW(sc_core::sc_start(sc_core::SC_ZERO_TIME),
                      sc_core::sc_report);
}

BOOST_FIXTURE_TEST_CASE(testScStartTwice,
                        TestEnvironment<MockScThreadStart>) {
    sc_thread_.triggerEvent(1);
    BOOST_CHECK_NO_THROW(simulation_.run());
    BOOST_CHECK_EQUAL(report_throw_counter, 1);
    BOOST_CHECK_EQUAL(report_log_counter, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testUserExceptionInDeltaPhase,
                        TestEnvironment<MockScThreadUserException>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(report_throw_counter, 1);
    BOOST_CHECK_EQUAL(report_log_counter, 1);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testScHaltExceptionInDeltaPhase,
                        TestEnvironment<MockScThreadScHaltException>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(report_throw_counter, 0);
    BOOST_CHECK_EQUAL(report_log_counter, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testKillInDeltaPhase,
                        TestEnvironment<MockScThreadKill>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(report_throw_counter, 0);
    BOOST_CHECK_EQUAL(report_log_counter, 0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testReEnterRunDeltaPhase,
                        TestEnvironment<MockScThreadRunDeltaPhase>) {
    sc_thread_.simulation_ = &simulation_;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_NO_THROW(simulation_.runDeltaPhase(0));
    BOOST_CHECK_EQUAL(sc_thread_.count_, 1);
}

SC_MODULE(DeltaThread) {
  public:
    typedef DeltaThread SC_CURRENT_USER_MODULE;
    DeltaThread()
        : sc_core::sc_module(sc_core::sc_module_name("DeltaThread")),
          started(false) {
        SC_THREAD(thread);
    }
    void thread() {
        started = true;
    }
    bool started;
};

BOOST_FIXTURE_TEST_CASE(initWithoutDeltaPhase, TestEnvironmentBase) {
    DeltaThread sc_thread_;
    // elaboration phase until we call init
    BOOST_CHECK_EQUAL(simulation_.context()->get_status(),
                      sc_core::SC_ELABORATION);
    BOOST_CHECK_EQUAL(sc_thread_.started, false);
    simulation_.finalize();
    // simulation (paused) phase, but no delta cycles have run yet
    BOOST_CHECK_EQUAL(simulation_.context()->get_status(),
                      sc_core::SC_PAUSED);
    BOOST_CHECK_EQUAL(sc_thread_.started, false);
    // run delta cycle, runs thread
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(sc_thread_.started, true);
}

SC_MODULE(MockScThreadReset) {
  public:
    typedef MockScThreadReset SC_CURRENT_USER_MODULE;
    MockScThreadReset()
        : sc_core::sc_module(sc_core::sc_module_name("MockThreadReset")) {
        SC_THREAD(model_thread);
    }
    void model_thread() {
        while (true) {
            sc_core::wait(1, sc_core::SC_PS);
            simics2tlmGasketCallSendTransaction();
        }
    }
    void simics2tlmGasketCallSendTransaction() {
        try {
            // transaction handling in model
            sc_core::sc_get_current_process_handle().reset();
        } catch(...) {
            sc_core::sc_stop();
        }
    }
};

BOOST_FIXTURE_TEST_CASE(testScStartNotLeftInScRunning,
                        TestEnvironment<MockScThreadReset>) {
    simulation_.run();
    BOOST_CHECK_EQUAL(sc_core::sc_get_status(), sc_core::SC_STOPPED);
}

BOOST_FIXTURE_TEST_CASE(testScStartNotLeftInScRunningInDeltaPhase,
                        TestEnvironment<MockScThreadReset>) {
    simulation_.set_delta(NULL, NULL, 1);
    simulation_.run();
    simulation_.runDeltaPhase(0);
    BOOST_CHECK_EQUAL(sc_core::sc_get_status(), sc_core::SC_STOPPED);
}

BOOST_FIXTURE_TEST_CASE(testKernelStateCheckFromRun,
                        TestEnvironment<MockScThread>) {
    sc_thread_.triggerEvent(10);
    simulation_.run();
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testKernelStateCheckFromRunSimulation,
                        TestEnvironment<MockScThread>) {
    sc_thread_.triggerEvent(10);
    simulation_.runSimulation(sc_core::sc_time::from_value(10));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testKernelStateCheckFromRunDeltaPhase,
                        TestEnvironment<MockScThread>) {
    sc_thread_.triggerEvent(10);
    simulation_.runDeltaPhase(0);
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testInstances, TestEnvironmentBase) {
    std::vector<conf_object_t *> instances = simulation_.instances();
    BOOST_CHECK_EQUAL(instances.size(), 2);
    BOOST_CHECK_EQUAL(instances[0], &conf_);

    {
        TestEnvironmentBase base2;
        instances = simulation_.instances();
        BOOST_CHECK_EQUAL(instances.size(), 4);
        BOOST_CHECK_EQUAL(instances[0], &conf_);
        BOOST_CHECK_EQUAL(instances[2], &base2.conf_);
    }

    instances = simulation_.instances();
    BOOST_CHECK_EQUAL(instances.size(), 2);
    BOOST_CHECK_EQUAL(instances[0], &conf_);
}

#endif  // INTC_EXT

BOOST_AUTO_TEST_SUITE_END()
