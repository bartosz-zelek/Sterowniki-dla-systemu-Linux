// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>


#include <simics/systemc/thread_pool.h>
#include <simics/systemc/kernel.h>

#include <systemc>

#include "mock/mock_simulation.h"

using simics::systemc::iface::SimulationInterface;
using simics::systemc::Thread;
using simics::systemc::ThreadCallbackInterface;
using simics::systemc::ThreadInterface;
using simics::systemc::ThreadPool;

BOOST_AUTO_TEST_SUITE(TestThreadPool)

template<class T>
class TestEnvironment {
  public:
    unittest::MockSimulation simulation_;
    T thread_;
};

class NoWait : public ThreadCallbackInterface {
  public:
    explicit NoWait(SimulationInterface *simulation)
        : runs_(0), finished_(false), blocks_(0), exception_(false),
          simulation_(simulation) {}
    virtual void run(ThreadInterface *call) {
        finished_ = false;
        ++runs_;
    }
    virtual void block(ThreadInterface *call) {
        ++blocks_;
    }
    virtual void finish(ThreadInterface *call) {
        finished_ = true;
    }
    virtual void exception(ThreadInterface *call) {
        exception_ = true;
    }
    virtual SimulationInterface *simulation(ThreadInterface *call) {
        return simulation_;
    }
    int runs_;
    bool finished_;
    int blocks_;
    bool exception_;
    SimulationInterface *simulation_;
};

class Wait : public NoWait {
  public:
    explicit Wait(SimulationInterface *simulation) : NoWait(simulation) {}
    virtual void run(ThreadInterface *call) {
        finished_ = false;
        sc_core::wait(1, sc_core::SC_PS);
        ++runs_;
    }
};

BOOST_FIXTURE_TEST_CASE(testNoWait, TestEnvironment<Thread>) {
    NoWait cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    thread_.spawn();

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 0);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    // Test reuse
    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 0);
    BOOST_CHECK_EQUAL(cb.runs_, 2);
    BOOST_CHECK_EQUAL(cb.exception_, false);
}

BOOST_FIXTURE_TEST_CASE(testNoWaitPool, TestEnvironment<ThreadPool<> >) {
    NoWait cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 0);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    // Test reuse
    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 0);
    BOOST_CHECK_EQUAL(cb.runs_, 2);
    BOOST_CHECK_EQUAL(cb.exception_, false);
}

BOOST_FIXTURE_TEST_CASE(testWait, TestEnvironment<Thread>) {
    Wait cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    thread_.spawn();

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.finished_, false);
    BOOST_CHECK_EQUAL(cb.blocks_, 1);
    BOOST_CHECK_EQUAL(cb.runs_, 0);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 1);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    // Test reuse
    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.finished_, false);
    BOOST_CHECK_EQUAL(cb.blocks_, 2);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 2);
    BOOST_CHECK_EQUAL(cb.runs_, 2);
    BOOST_CHECK_EQUAL(cb.exception_, false);
}

BOOST_FIXTURE_TEST_CASE(testWaitPool, TestEnvironment<ThreadPool<> >) {
    Wait cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.finished_, false);
    BOOST_CHECK_EQUAL(cb.blocks_, 1);
    BOOST_CHECK_EQUAL(cb.runs_, 0);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 1);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    // Test reuse
    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.finished_, false);
    BOOST_CHECK_EQUAL(cb.blocks_, 2);
    BOOST_CHECK_EQUAL(cb.runs_, 1);
    BOOST_CHECK_EQUAL(cb.exception_, false);

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.finished_, true);
    BOOST_CHECK_EQUAL(cb.blocks_, 2);
    BOOST_CHECK_EQUAL(cb.runs_, 2);
    BOOST_CHECK_EQUAL(cb.exception_, false);
}

class ThreadWithData : public Thread {
  public:
    ThreadWithData() {
        that_ = this;
    }
    static ThreadWithData *that_;
};

ThreadWithData *ThreadWithData::that_ = NULL;

class ThreadDataCallback : public ThreadCallbackInterface {
  public:
    explicit ThreadDataCallback(SimulationInterface *simulation)
        : data_run_(NULL), data_block_(NULL), data_finish_(NULL),
          data_exception_(NULL), data_simulation_(NULL),
          simulation_(simulation) {}
    virtual void run(ThreadInterface *call) {
        data_run_ = dynamic_cast<ThreadWithData *>(call);
    }
    virtual void block(ThreadInterface *call) {
        data_block_ = dynamic_cast<ThreadWithData *>(call);
    }
    virtual void finish(ThreadInterface *call) {
        data_finish_ = dynamic_cast<ThreadWithData *>(call);
    }
    virtual void exception(ThreadInterface *call) {
        data_exception_ = dynamic_cast<ThreadWithData *>(call);
    }
    virtual SimulationInterface *simulation(ThreadInterface *call) {
        data_simulation_ = dynamic_cast<ThreadWithData *>(call);
        return simulation_;
    }
    ThreadWithData *data_run_;
    ThreadWithData *data_block_;
    ThreadWithData *data_finish_;
    ThreadWithData *data_exception_;
    ThreadWithData *data_simulation_;
    SimulationInterface *simulation_;
};

BOOST_FIXTURE_TEST_CASE(testThreadDataNoWait,
                        TestEnvironment<ThreadPool<ThreadWithData> >) {
    ThreadDataCallback cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.data_run_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_block_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_finish_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_simulation_, ThreadWithData::that_);
}

class ThreadDataCallbackWait : public ThreadDataCallback {
  public:
    explicit ThreadDataCallbackWait(SimulationInterface *simulation)
        : ThreadDataCallback(simulation) {
    }
    virtual void run(ThreadInterface *call) {
        ThreadDataCallback::run(call);
        sc_core::wait(1, sc_core::SC_PS);
    }
};

BOOST_FIXTURE_TEST_CASE(testThreadDataWait,
                        TestEnvironment<ThreadPool<ThreadWithData> >) {
    ThreadDataCallbackWait cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.data_run_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_block_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_finish_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.data_finish_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));
}

class ThreadDataCallbackException : public ThreadDataCallback {
  public:
    explicit ThreadDataCallbackException(SimulationInterface *simulation)
        : ThreadDataCallback(simulation) {
    }
    virtual void run(ThreadInterface *call) {
        ThreadDataCallback::run(call);
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_user());
    }
};

BOOST_FIXTURE_TEST_CASE(testThreadDataException,
                        TestEnvironment<ThreadPool<ThreadWithData> >) {
    ThreadDataCallbackException cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb),
                      ThreadInterface::CALL_RETURN_TERMINATED);
    BOOST_CHECK_EQUAL(cb.data_run_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_block_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_finish_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_exception_, ThreadWithData::that_);
}

class ThreadDataCallbackWaitException : public ThreadDataCallback {
  public:
    explicit ThreadDataCallbackWaitException(SimulationInterface *simulation)
        : ThreadDataCallback(simulation) {
    }
    virtual void run(ThreadInterface *call) {
        ThreadDataCallback::run(call);
        sc_core::wait(1, sc_core::SC_PS);
        sc_core::sc_get_current_process_handle().throw_it(sc_core::sc_user());
    }
};

BOOST_FIXTURE_TEST_CASE(testThreadDataExceptionWait,
                        TestEnvironment<ThreadPool<ThreadWithData> >) {
    ThreadDataCallbackWaitException cb(&simulation_);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_WAITING);
    BOOST_CHECK_EQUAL(cb.data_run_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_block_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_finish_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));

    sc_core::sc_start(1, sc_core::SC_PS);
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(cb.data_finish_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_exception_, ThreadWithData::that_);
}

class ThreadDataCallbackExceptionAfterFinished : public ThreadDataCallback {
  public:
    explicit ThreadDataCallbackExceptionAfterFinished(
            SimulationInterface *simulation)
        : ThreadDataCallback(simulation) {
    }
    virtual void run(ThreadInterface *call) {
        ThreadDataCallback::run(call);
        handle_ = sc_core::sc_get_current_process_handle();
    }
    sc_core::sc_process_handle handle_;
};

SC_MODULE(ExceptionThread) {
    SC_CTOR(ExceptionThread) {
        SC_THREAD(action);
        sensitive << event_;
        dont_initialize();
    }
    void triggerEvent(const sc_dt::uint64& delay) {
        event_.notify(sc_core::sc_time::from_value(delay));
    }
    void action() {
        handle_.throw_it(sc_core::sc_user());
    }
    sc_core::sc_event event_;
    sc_core::sc_process_handle handle_;
};

BOOST_FIXTURE_TEST_CASE(testThreadDataExceptionAfterFinished,
                        TestEnvironment<ThreadPool<ThreadWithData> >) {
    ThreadDataCallbackExceptionAfterFinished cb(&simulation_);
    ExceptionThread exception_thread("thread");
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(thread_.call(&cb), ThreadInterface::CALL_RETURN_FINISHED);
    BOOST_CHECK_EQUAL(cb.data_run_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_block_, static_cast<ThreadWithData *>(NULL));
    BOOST_CHECK_EQUAL(cb.data_finish_, ThreadWithData::that_);
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));

    exception_thread.handle_ = cb.handle_;
    exception_thread.triggerEvent(0);
    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(cb.data_exception_, static_cast<ThreadWithData *>(NULL));
}

class ThreadWithName : public Thread {
  public:
    virtual const char *thread_name() {
        return "ThreadWithName";
    }
};

BOOST_FIXTURE_TEST_CASE(testThreadName, TestEnvironment<ThreadWithName>) {
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    thread_.spawn();

    BOOST_CHECK(sc_core::sc_find_object("ThreadWithName") != 0);
}

BOOST_AUTO_TEST_SUITE_END()
