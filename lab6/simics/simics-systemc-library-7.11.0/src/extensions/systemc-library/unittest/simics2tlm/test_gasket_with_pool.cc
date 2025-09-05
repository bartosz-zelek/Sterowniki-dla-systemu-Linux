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


#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/simics2tlm/gasket.h>
#include <simics/systemc/kernel.h>
#include <simics/systemc/kernel_state_modifier.h>

#include <systemc>

#include "environment.h"
#include "stubs.h"

using simics::systemc::iface::Transaction;
using simics::systemc::iface::TransactionExtension;
using simics::systemc::iface::TransactionStatus;
using simics::systemc::iface::TransactionPool;

BOOST_AUTO_TEST_SUITE(TestSimics2TlmGasketWithPool)

class TestDevice : public sc_core::sc_module,
                   public tlm::tlm_fw_transport_if<> {
  public:
    explicit TestDevice(sc_core::sc_module_name = "TestDevice")
        : target_socket_("target_socket"), response_(tlm::TLM_OK_RESPONSE),
          delta_time_(sc_core::sc_time(1, sc_core::SC_NS)) {
            target_socket_.bind(*this);
    }
    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        action(trans, t);
        trans.set_response_status(response_);
    }
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
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
    sc_core::sc_time delta_time_;  // time spent in the device.
};

class TestDeviceWait : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        wait(t);
    }
};

class TestDeviceNoWait : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
    }
};

class TestDeviceThrow : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        SC_REPORT_ERROR("test", "Should throw");
    }
};

class TestDeviceCustomThrow : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        throw 0;
    }
};

class TestDeviceScUserThrow : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_user());
    }
};

class TestDeviceScHaltThrow : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_halt());
    }
};

class TestDeviceKillThrow : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        sc_core::sc_get_current_process_handle().kill();
    }
};

class TestDeviceWithThread : public TestDevice {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestDeviceWithThread);
#endif
    explicit TestDeviceWithThread(sc_core::sc_module_name = "TestDevice")
        : delay_(1) {
        SC_THREAD(loop);
        sensitive << event_;
    }
    void loop() {
        while (true) {
            event_.notify(sc_core::sc_time::from_value(delay_));
            wait();
            thread_action();
        }
    }
    virtual void thread_action() {
    }
    sc_core::sc_event event_;
    sc_dt::uint64 delay_;
};

class TestDeviceThrowInThread : public TestDeviceWithThread {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        // By adding a wait in the b_transport we make sure the thread is
        // run before transaction completes, and any exceptions thrown
        // is caught by our gasket wrapper
        t += delta_time_;
        wait(t);
    }
};

class TestDeviceCustomThrowInThread : public TestDeviceThrowInThread {
  public:
    virtual void thread_action() {
        throw 0;
    }
};

class TestDeviceScUserThrowInThread : public TestDeviceThrowInThread {
  public:
    virtual void thread_action() {
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_user());
    }
};

class TestDeviceScHaltThrowInThread : public TestDeviceThrowInThread {
  public:
    virtual void thread_action() {
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_halt());
    }
};

class TestDeviceKillInThread : public TestDeviceThrowInThread {
  public:
    virtual void thread_action() {
        sc_core::sc_get_current_process_handle().kill();
    }
};

class TestDeviceDmiEnabled: public TestDevice {
  public:
    explicit TestDeviceDmiEnabled(sc_core::sc_module_name = "TestDevice")
        : b_transport_count_(0) {
        delta_time_ = sc_core::SC_ZERO_TIME;
    }
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        t += delta_time_;
        trans.set_address(0xdead);
        ++b_transport_count_;
    }
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans,  // NOLINT
                                    tlm::tlm_dmi &dmi_data) {  // NOLINT
        unsigned char *data = new unsigned char[4];
        dmi_data.init();
        dmi_data.set_dmi_ptr(data);
        dmi_data.set_start_address(trans.get_address());
        dmi_data.set_end_address(trans.get_address() + trans.get_data_length());
        dmi_data.set_granted_access(tlm::tlm_dmi::DMI_ACCESS_READ_WRITE);
        return true;
    }
    int b_transport_count_;
};

template<class Device>
class TestEnvironment  : public Environment {
  public:
    TestEnvironment()
        : gasket_(new simics::systemc::simics2tlm::Gasket<>(
                      "gasket_",
                      simulation_.simics_object())),
          transaction_(pool_.acquire()) {
        simics::systemc::simics2tlm::Gasket<> * gasket =
                static_cast<simics::systemc::simics2tlm::Gasket<> *>(gasket_);
        gasket->init(&simulation_, &simulation_);
        gasket->bind(test_device_.target_socket_);
    }
    ~TestEnvironment() {
        delete gasket_;
    }
    Transaction preparePayloadWithDmiAllowed() {
        Transaction t = pool_.acquire();
        static unsigned char data[4] = {1, 2, 3, 4};
        t->set_dmi_allowed(true);
        t->set_read();
        t->set_data_length(4);
        t->set_data_ptr(data);
        return t;
    }

    simics::systemc::simics2tlm::GasketInterface *gasket_;
    TransactionPool pool_;
    Transaction transaction_;
    Device test_device_;
    Stubs stubs_;
};

template<class Device>
class TestEnvironmentNoSimulation {
  public:
    explicit TestEnvironmentNoSimulation(unittest::MockSimulation *iface)
        :  gasket_(NULL),
           transaction_(pool_.acquire()) {
        simics::systemc::simics2tlm::Gasket<> *gasket
            = new simics::systemc::simics2tlm::Gasket<>("gasket_",
                                                        iface->simics_object());
        gasket->init(iface, iface);
        gasket->bind(test_device_.target_socket_);
        gasket_ = gasket;
    }
    ~TestEnvironmentNoSimulation() {
        delete gasket_;
    }

    simics::systemc::simics2tlm::GasketInterface *gasket_;
    TransactionPool pool_;
    Transaction transaction_;
    Device test_device_;
};

class TestDeviceForwardTlm : public TestDevice {
  public:
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        simics::systemc::Kernel kernel(environment_.simulation_.context());
        BOOST_CHECK_EQUAL(environment_.gasket_->trigger(
                &environment_.transaction_), true);
    }
    TestEnvironment<TestDeviceWait> environment_;
};

class TestDeviceForwardTlmSameEnvironment : public TestDevice {
  public:
    explicit TestDeviceForwardTlmSameEnvironment(
            unittest::MockSimulation *iface) :
        environment_(iface) {
    }
    virtual void action(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t) {  // NOLINT
        BOOST_CHECK_EQUAL(environment_.gasket_->trigger(
                &environment_.transaction_), true);
    }
    TestEnvironmentNoSimulation<TestDeviceWait> environment_;
};

// Specialization where simulation_ is passed to the dut
template<>
class TestEnvironment<TestDeviceForwardTlmSameEnvironment>
    : public Environment {
  public:
    TestEnvironment()
        : gasket_(new simics::systemc::simics2tlm::Gasket<>(
                      "gasket_", simulation_.simics_object())),
          transaction_(pool_.acquire()),
          test_device_(&simulation_) {
        simics::systemc::simics2tlm::Gasket<> * gasket =
                static_cast<simics::systemc::simics2tlm::Gasket<> *>(gasket_);
        gasket->init(&simulation_, &simulation_);
        gasket->bind(test_device_.target_socket_);
    }
    ~TestEnvironment() {
        delete gasket_;
    }
    Transaction preparePayloadWithDmiAllowed() {
        Transaction t = pool_.acquire();
        static unsigned char data[4] = {1, 2, 3, 4};
        t->set_dmi_allowed(true);
        t->set_read();
        t->set_data_length(4);
        t->set_data_ptr(data);
        return t;
    }

    simics::systemc::simics2tlm::GasketInterface *gasket_;
    TransactionPool pool_;
    Transaction transaction_;
    TestDeviceForwardTlmSameEnvironment test_device_;
    Stubs stubs_;
};

BOOST_FIXTURE_TEST_CASE(testTriggerTransaction,
    TestEnvironment<TestDevice>) {
    {
        simics::systemc::KernelStateModifier modifier(&simulation_);
        sc_core::sc_start(sc_core::SC_ZERO_TIME);
    }
    BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), true);

    Stubs::instance_.log_level_ = 1;
    tlm::tlm_response_status errs[] = { tlm::TLM_INCOMPLETE_RESPONSE,
                                        tlm::TLM_GENERIC_ERROR_RESPONSE,
                                        tlm::TLM_ADDRESS_ERROR_RESPONSE,
                                        tlm::TLM_COMMAND_ERROR_RESPONSE,
                                        tlm::TLM_BURST_ERROR_RESPONSE,
                                        tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE };
    for (tlm::tlm_response_status *it = errs;
         it != errs + sizeof errs / sizeof errs[0]; ++it) {
        test_device_.response_ = *it;
        BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), false);
        // TODO(aarno): What is the correct logging behavior to expect?
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_error_count_, 0);
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_info_count_, 0);
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_spec_violation_count_, 0);
        BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_unimplemented_count_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(testWaitInDevice, TestEnvironment<TestDeviceWait>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), true);
}

BOOST_FIXTURE_TEST_CASE(testWaitAfterDeviceCall,
                        TestEnvironment<TestDeviceNoWait>) {
    test_device_.delta_time_ = sc_core::sc_time(10, sc_core::SC_NS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), true);
    BOOST_CHECK_EQUAL(sc_core::sc_time_stamp(), test_device_.delta_time_);
}

BOOST_FIXTURE_TEST_CASE(testThrowInDevice, TestEnvironment<TestDeviceThrow>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testCustomThrowInDevice,
                        TestEnvironment<TestDeviceCustomThrow>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testScUserThrowInDevice,
                        TestEnvironment<TestDeviceScUserThrow>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testScHaltThrowInDevice,
                        TestEnvironment<TestDeviceScHaltThrow>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testKillThrowInDevice,
                        TestEnvironment<TestDeviceKillThrow>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testCustomThrowInDeviceThread,
                        TestEnvironment<TestDeviceCustomThrowInThread>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testScUserThrowInDeviceThread,
                        TestEnvironment<TestDeviceScUserThrowInThread>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testScHaltThrowInDeviceThread,
                        TestEnvironment<TestDeviceScHaltThrowInThread>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testKillInDeviceThread,
                        TestEnvironment<TestDeviceKillInThread>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_NO_THROW(gasket_->trigger(&transaction_));
    BOOST_CHECK_EQUAL(Stubs::instance_.SIM_log_critical_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testInvokeDeviceInSecondSimulation,
    TestEnvironment<TestDeviceForwardTlm>) {
#if INTC_EXT  // multiple context handling is an extended feature
    {
        simics::systemc::Kernel kernel(simulation_.context());
        sc_core::sc_start(sc_core::SC_ZERO_TIME);
    }
    {
        simics::systemc::Kernel kernel(
            test_device_.environment_.simulation_.context());
        sc_core::sc_start(sc_core::SC_ZERO_TIME);
    }
    {
        simics::systemc::Kernel kernel(simulation_.context());
        BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), true);
    }
#endif
}

BOOST_FIXTURE_TEST_CASE(testTriggerTransactionWhenSimulationStopped,
    TestEnvironment<TestDevice>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    sc_core::sc_stop();
    BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), false);
}

BOOST_FIXTURE_TEST_CASE(testReEnterDifferentGasketInSameSimulation,
    TestEnvironment<TestDeviceForwardTlmSameEnvironment>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&transaction_), true);
}

BOOST_FIXTURE_TEST_CASE(testTriggerTransactionTwiceWhenDmiEnabled,
    TestEnvironment<TestDeviceDmiEnabled>) {
    Transaction t = preparePayloadWithDmiAllowed();

    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);

    t = preparePayloadWithDmiAllowed();
    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    // Since DMI is enabled, no b_transport is called in the second access
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testTriggerTransactionTwiceWhenDmiDisabled,
    TestEnvironment<TestDeviceDmiEnabled>) {
    BOOST_CHECK_EQUAL(gasket_->is_dmi_enabled(), true);
    gasket_->set_dmi(false);
    BOOST_CHECK_EQUAL(gasket_->is_dmi_enabled(), false);
    Transaction t = preparePayloadWithDmiAllowed();

    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);

    t = preparePayloadWithDmiAllowed();
    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    // Since DMI is disabled, b_transport is called in the second access
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 2);
}

class MockCallback: public TransactionExtension::Callback {
  public:
    MockCallback()
        : is_async_(true), is_async_called_(false),
          defer_cnt_(0), complete_cnt_(0),
          deferred_transaction_(NULL), completed_transaction_(NULL),
          success_(false) {}
    virtual bool is_async(Transaction *transaction) {
        is_async_called_ = true;
        return is_async_;
    }
    virtual void defer(Transaction *transaction) {
        // Defer must never be called after is_async. The implementation
        // of defer is allowed to change the return value of is_async,
        // with respect to the deferred call.
        BOOST_CHECK_EQUAL(is_async_called_, false);
        ++defer_cnt_;
        deferred_transaction_ = transaction;
    }
    virtual void complete(Transaction *transaction, bool success) {
        ++complete_cnt_;
        completed_transaction_ = transaction;
        success_ = success;
    }
    bool is_async_;
    bool is_async_called_;
    int defer_cnt_;
    int complete_cnt_;
    Transaction *deferred_transaction_;
    Transaction *completed_transaction_;
    bool success_;
};

SC_MODULE(GasketInvokerThread) {
    SC_CTOR(GasketInvokerThread) {
        gasket_ = NULL;
        transaction_ = NULL;
        success_ = false;
        SC_THREAD(action);
    }
    void action() {
        while (true) {
            sc_core::wait(1, sc_core::SC_PS);
            success_ = gasket_->trigger(transaction_);
        }
    }
    simics::systemc::simics2tlm::GasketInterface *gasket_;
    Transaction *transaction_;
    bool success_;
};

BOOST_FIXTURE_TEST_CASE(testCallbackWhenRunning, TestEnvironment<TestDevice>) {
    GasketInvokerThread invoker("invoker");
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    Transaction t = pool_.acquire();
    t.extension()->set_callback(&callback);

    invoker.transaction_ = &t;
    invoker.gasket_ = gasket_;

    sc_core::sc_start(2, sc_core::SC_PS);

    BOOST_CHECK(t->is_response_ok() == true);
    BOOST_CHECK_EQUAL(invoker.success_, true);
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 0);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_, &t);
    BOOST_CHECK_EQUAL(callback.success_, true);
}

BOOST_FIXTURE_TEST_CASE(testCallbackWhenRunningNoSuccess,
                        TestEnvironment<TestDevice>) {
    GasketInvokerThread invoker("invoker");
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    Transaction t = pool_.acquire();
    t.extension()->set_callback(&callback);

    invoker.transaction_ = &t;
    invoker.gasket_ = gasket_;
    test_device_.response_ = tlm::TLM_INCOMPLETE_RESPONSE;

    sc_core::sc_start(2, sc_core::SC_PS);

    BOOST_CHECK(t->is_response_ok() == false);
    BOOST_CHECK_EQUAL(invoker.success_, false);
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 0);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_, &t);
    BOOST_CHECK_EQUAL(callback.success_, false);
}

BOOST_FIXTURE_TEST_CASE(testCallbackNoWaitInGasketThread,
                          TestEnvironment<TestDevice>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    Transaction t = pool_.acquire();
    t.extension()->set_callback(&callback);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK(t->is_response_ok() == true);
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 0);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_->payload(), t.payload());
}

BOOST_FIXTURE_TEST_CASE(testCallbackWaitInGasketThread,
                        TestEnvironment<TestDeviceNoWait>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    Transaction t = pool_.acquire();
    t.extension()->set_callback(&callback);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK(t.extension()->status().active());
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.deferred_transaction_->payload(), t.payload());
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 0);

    sc_core::sc_start(1, sc_core::SC_NS);
    sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK(t->is_response_ok() == true);
    BOOST_CHECK(t.extension()->status().invalid());
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_->payload(), t.payload());
}

BOOST_FIXTURE_TEST_CASE(testCallbackWaitInGasketThreadSync,
                        TestEnvironment<TestDeviceNoWait>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    callback.is_async_ = false;
    Transaction t = pool_.acquire();
    t.extension()->set_callback(&callback);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK(t.extension()->status().invalid());
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_->payload(), t.payload());
}

BOOST_FIXTURE_TEST_CASE(testCallbackWhenDmiEnabled,
                        TestEnvironment<TestDeviceDmiEnabled>) {
    Transaction t = preparePayloadWithDmiAllowed();

    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);

    t = preparePayloadWithDmiAllowed();

    MockCallback callback;
    t.extension()->set_callback(&callback);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    // Since DMI is enabled, no b_transport is called in the second access
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 0);
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_->payload(), t.payload());
    BOOST_CHECK_EQUAL(callback.success_, true);
}

BOOST_FIXTURE_TEST_CASE(testCallbackWhenDmiEnabledASync,
                        TestEnvironment<TestDeviceDmiEnabled>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback;
    Transaction t = preparePayloadWithDmiAllowed();
    test_device_.delta_time_ = sc_core::sc_time(1, sc_core::SC_PS);
    t.extension()->set_callback(&callback);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);

    sc_core::sc_start(2, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);
    BOOST_CHECK(t->is_response_ok() == true);
    BOOST_CHECK_EQUAL(callback.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.deferred_transaction_->payload(), t.payload());
    BOOST_CHECK_EQUAL(callback.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback.completed_transaction_->payload(), t.payload());
    BOOST_CHECK_EQUAL(callback.success_, true);

    MockCallback callback2;
    t = preparePayloadWithDmiAllowed();
    t.extension()->set_callback(&callback2);

    BOOST_CHECK_EQUAL(gasket_->trigger(&t), true);
    // Since DMI is enabled, no b_transport is called in the second access
    BOOST_CHECK_EQUAL(test_device_.b_transport_count_, 1);
    BOOST_CHECK_EQUAL(callback2.defer_cnt_, 0);
    BOOST_CHECK_EQUAL(callback2.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback2.completed_transaction_->payload(), t.payload());
    BOOST_CHECK_EQUAL(callback2.success_, true);
}

BOOST_FIXTURE_TEST_CASE(testReentryCallbackWaitInGasketThread,
                        TestEnvironment<TestDeviceNoWait>) {
    test_device_.delta_time_ = sc_core::sc_time(2, sc_core::SC_NS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    MockCallback callback1;
    Transaction t1 = pool_.acquire();
    t1.extension()->set_callback(&callback1);
    BOOST_CHECK_EQUAL(gasket_->trigger(&t1), true);

    BOOST_CHECK(t1.extension()->status().active());
    BOOST_CHECK_EQUAL(callback1.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.deferred_transaction_->payload(), t1.payload());
    BOOST_CHECK_EQUAL(callback1.complete_cnt_, 0);

    sc_core::sc_start(1, sc_core::SC_NS);
    sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK(t1.extension()->status().active());
    BOOST_CHECK_EQUAL(callback1.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.complete_cnt_, 0);

    MockCallback callback2;
    Transaction t2 = pool_.acquire();
    t2.extension()->set_callback(&callback2);
    BOOST_CHECK_EQUAL(gasket_->trigger(&t2), true);

    BOOST_CHECK(t1.extension()->status().active());
    BOOST_CHECK_EQUAL(callback1.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.complete_cnt_, 0);

    BOOST_CHECK(t2.extension()->status().active());
    BOOST_CHECK_EQUAL(callback2.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback2.deferred_transaction_->payload(), t2.payload());
    BOOST_CHECK_EQUAL(callback2.complete_cnt_, 0);

    sc_core::sc_start(1, sc_core::SC_NS);
    sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK(t1.extension()->status().invalid());
    BOOST_CHECK_EQUAL(callback1.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.completed_transaction_->payload(),
                      t1.payload());

    BOOST_CHECK(t2.extension()->status().active());
    BOOST_CHECK_EQUAL(callback2.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback2.complete_cnt_, 0);

    sc_core::sc_start(1, sc_core::SC_NS);
    sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK(t1.extension()->status().invalid());
    BOOST_CHECK_EQUAL(callback1.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback1.complete_cnt_, 1);

    BOOST_CHECK(t2.extension()->status().invalid());
    BOOST_CHECK_EQUAL(callback2.defer_cnt_, 1);
    BOOST_CHECK_EQUAL(callback2.complete_cnt_, 1);
    BOOST_CHECK_EQUAL(callback2.completed_transaction_->payload(),
                      t2.payload());
}

BOOST_AUTO_TEST_SUITE_END()
