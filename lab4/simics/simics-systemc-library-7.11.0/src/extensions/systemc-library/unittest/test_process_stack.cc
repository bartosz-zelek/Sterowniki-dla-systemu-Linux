// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <boost/test/unit_test.hpp>

#include <simics/systemc/process_stack.h>

#include <systemc>

#include "simcontext_wrapper.h"
#include "mock/mock_simulation.h"

BOOST_AUTO_TEST_SUITE(TestProcessStack)

template <typename T>
class TestEnvironment {
  public:
    TestEnvironment()
        : pstack(&simulation), calls(0) {
        for (int i = 0; i < 10; ++i) {
            device[i].pstack_ = &pstack;
            device[i].calls_ = &calls;
            device[i].enter_ = &enter;
            device[i].exit_ = &exit;
        }
        sc_core::sc_start(sc_core::SC_ZERO_TIME);
    }

    unittest::MockSimulation simulation;
    T device[10];
    simics::systemc::ProcessStack pstack;
    int calls;
    std::list<std::string> enter;
    std::list<std::string> exit;
};

class FooInterface {
  public:
    ~FooInterface() {}
    virtual void foo() = 0;
};

class TestDevice : public sc_core::sc_module,
                   public FooInterface {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestDevice);
#endif
    explicit TestDevice(sc_core::sc_module_name = "TestDevice")
        : event1_("Event1"), event2_("Event2"),
          pstack_(NULL), calls_(NULL), use_stack_(true) {
        SC_THREAD(t1);
        SC_THREAD(t2);
        event1_.notify(sc_core::sc_time(1, sc_core::SC_PS));
    }

    // t1 schedules t2 and calls foo() that waits longer than t2 event notify
    // The idea is to grow the stack and then wait for it to unwind correctly
    void t1() {
        while (1) {
            wait(event1_);
            enter_->push_back(std::string(name()) + "t1");
            sc_core::sc_time delay(sc_core::sc_time(5, sc_core::SC_PS));
            event2_.notify(delay);
            pstack_->push();
            foo();  // doesn't matter if we call our foo or other device's foo
            pstack_->pop();
            exit_->push_back(std::string(name()) + "t1");
        }
    }

    // t2 schedules t1 and calls foo() that waits longer than t1 event notify
    // The idea is to grow the stack and then wait for it to unwind correctly
    void t2() {
        while (1) {
            wait(event2_);
            enter_->push_back(std::string(name()) + "t2");
            sc_core::sc_time delay(sc_core::sc_time(5, sc_core::SC_PS));
            event1_.notify(delay);
            pstack_->push();
            foo();
            pstack_->pop();
            exit_->push_back(std::string(name()) + "t2");
        }
    }

    void foo() {
        ++(*calls_);
        wait(sc_core::sc_time(10, sc_core::SC_PS));
        if (use_stack_) {
            pstack_->waitForCurrentProcessOnTop();
        }
        --(*calls_);
    }

    sc_core::sc_event event1_;
    sc_core::sc_event event2_;
    simics::systemc::ProcessStack *pstack_;
    int *calls_;
    std::list<std::string> *enter_;
    std::list<std::string> *exit_;
    bool use_stack_;
};

BOOST_FIXTURE_TEST_CASE(withoutStack, TestEnvironment<TestDevice>) {
    for (int i = 0; i < 10; ++i) {
        device[i].use_stack_ = false;
    }

    BOOST_CHECK_EQUAL(calls, 0);
    // By running 12 ps, without a proper call stack, all t1 threads's call to
    // foo() would have returned while the t2 threads would not (yet). This
    // would break LIFO order which is bad if foo is part of a Simics interface
    // where LIFO is assumed.  However, with a proper process stack, we can run
    // until the threads have returned in the correct order (see next test)
    sc_core::sc_start(sc_core::sc_time(12, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(calls, 10);
    // Running an additional 5 ps will return all t2's foo calls too
    sc_core::sc_start(sc_core::sc_time(5, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(calls, 0);
    // Since the return order is already proven wrong, we don't test it here
}

BOOST_FIXTURE_TEST_CASE(withStack, TestEnvironment<TestDevice>) {
    BOOST_CHECK_EQUAL(calls, 0);
    // With process stack enabled, no calls should return after 12 ps as we
    // require all t2 threads to return first
    sc_core::sc_start(sc_core::sc_time(12, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(calls, 20);
    pstack.runUntilStackEmpty();
    BOOST_CHECK_EQUAL(calls, 0);
    BOOST_CHECK_EQUAL(sc_core::sc_time_stamp(),
                      sc_core::sc_time(16, sc_core::SC_PS));
    // Test the LIFO order among the threads too
    exit.reverse();
    BOOST_CHECK_EQUAL_COLLECTIONS(enter.begin(), enter.end(),
                                  exit.begin(), exit.end());
}

BOOST_AUTO_TEST_SUITE_END()
