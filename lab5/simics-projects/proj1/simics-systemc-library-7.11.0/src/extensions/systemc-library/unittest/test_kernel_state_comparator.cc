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

#include <simics/systemc/kernel_state_comparator.h>
#include "simcontext_wrapper.h"

using simics::systemc::KernelStateComparator;

BOOST_AUTO_TEST_SUITE(TestKernelStateComparator)

SC_MODULE(TestModule) {
 public:
    typedef TestModule SC_CURRENT_USER_MODULE;
    TestModule()
        : sc_core::sc_module(sc_core::sc_module_name("TestModule")) {
        SC_THREAD(thread1);
        sensitive << event1_;
        SC_THREAD(thread2);
        sensitive << event2_;

        event1_.notify(10, sc_core::SC_PS);
        event2_.notify(20, sc_core::SC_PS);
    }
    void thread1() {
        while (true)
            sc_core::wait(event1_);
    }
    void thread2() {
        while (true)
            sc_core::wait(event2_);
    }
    sc_core::sc_event event1_;
    sc_core::sc_event event2_;
    sc_core::sc_signal<int> signal_;
};

SC_MODULE(TestModuleDeltaCycle) {
 public:
    typedef TestModuleDeltaCycle SC_CURRENT_USER_MODULE;
    TestModuleDeltaCycle()
        : sc_core::sc_module(sc_core::sc_module_name("TestModuleDeltaCycle")) {
        SC_THREAD(thread1);
        sensitive << event1_;
    }
    void thread1() {
        while (true) {
            event1_.notify(0, sc_core::SC_PS);
            sc_core::wait(event1_);
        }
    }

    sc_core::sc_event event1_;
};

class TestChannel : public sc_core::sc_prim_channel {
  public:
    TestChannel() : sc_core::sc_prim_channel("TestChannel"), update_cnt_(0) {
    }
    virtual void update(void) {
        ++update_cnt_;
    }

    int update_cnt_;
};

template <class T>
class TestEnvironment {
  public:
    TestEnvironment() {
        sc_core::sc_start(sc_core::SC_ZERO_TIME);
        comparator_ = KernelStateComparator();
    }

    unittest::SimContextWrapper context_;
    KernelStateComparator comparator_;
    T module_;
};

BOOST_FIXTURE_TEST_CASE(testNoStateChange, TestEnvironment<TestModule>) {
    KernelStateComparator c;

    BOOST_CHECK(comparator_ == c);
}

BOOST_FIXTURE_TEST_CASE(testEventNotifyImmediate, TestEnvironment<TestModule>) {
    module_.event1_.notify();
    KernelStateComparator c;

    BOOST_CHECK(comparator_ != c);
}

BOOST_FIXTURE_TEST_CASE(testEventNotifyTimed, TestEnvironment<TestModule>) {
    module_.event2_.notify(15, sc_core::SC_PS);
    KernelStateComparator c;

    BOOST_CHECK(comparator_ != c);
}

BOOST_FIXTURE_TEST_CASE(testTimeAdvanced, TestEnvironment<TestModule>) {
    sc_core::sc_start(1, sc_core::SC_PS);
    KernelStateComparator c;

    BOOST_CHECK(comparator_ != c);
}

BOOST_FIXTURE_TEST_CASE(testPrimitiveChannel, TestEnvironment<TestModule>) {
    module_.signal_ = 1;
    KernelStateComparator c;

    BOOST_CHECK(comparator_ != c);
}

BOOST_FIXTURE_TEST_CASE(testDeltaEvent, TestEnvironment<TestModuleDeltaCycle>) {
    sc_core::sc_start(0, sc_core::SC_PS);
    KernelStateComparator c;

    BOOST_CHECK(comparator_ != c);
}

BOOST_FIXTURE_TEST_CASE(testASyncRequestUpdate,
                        TestEnvironment<TestChannel>) {
    module_.async_request_update();
    KernelStateComparator c;

    BOOST_CHECK(comparator_ == c);
}

BOOST_AUTO_TEST_SUITE_END()
