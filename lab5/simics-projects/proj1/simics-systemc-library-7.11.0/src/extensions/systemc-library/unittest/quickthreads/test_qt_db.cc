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

#include <systemc>

#include <simics/systemc/kernel.h>
#include <qt_db.h>

BOOST_AUTO_TEST_SUITE(TestQtDb)

#if INTC_EXT

SC_MODULE(ThreadModule) {
    SC_CTOR(ThreadModule) {
        SC_THREAD(T1);
        SC_THREAD(T2);
    }
    void T1() {
        wait(1, sc_core::SC_PS);
    }
    void T2() {
        wait(2, sc_core::SC_PS);
    }
    void spawnT1() {
        sc_core::sc_spawn(sc_bind(&ThreadModule::T1, this), "T1");
    }
};

class TestEnvironment {
  public:
    TestEnvironment()
        : context_(new sc_core::sc_simcontext), kernel_(context_) {
    }
    ~TestEnvironment() {
        delete context_;
    }
    void startTimeTest() {
        t0_ = clock();
    }
    void endTimeTest(float limit) {
        clock_t t1 = clock();
        float diff = static_cast<float>(t1 - t0_);
        diff /= CLOCKS_PER_SEC;
        BOOST_CHECK_LT(diff, limit);
    }

    sc_core::sc_simcontext *context_;
    simics::systemc::Kernel kernel_;
    clock_t t0_;
};

BOOST_AUTO_TEST_CASE(TestNumberOfThreads) {
    {
        TestEnvironment env;
        ThreadModule m("ThreadModule");

        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 0);

        sc_core::sc_start(1, sc_core::SC_PS);
        // One for the main thread running sc_start and two for T1 and T2
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 3);

        sc_core::sc_start(1, sc_core::SC_PS);
        // T1 has ended
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 2);

        sc_core::sc_start(1, sc_core::SC_PS);
        // T2 has ended
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

        sc_core::sc_start(1, sc_core::SC_PS);
        // main remains
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

        // Spawn dynamic thread T1
        m.spawnT1();
        sc_core::sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 2);

        sc_core::sc_start(1, sc_core::SC_PS);
        // T1 has ended
        BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);
    }
    // Package deleted and main removed as well
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 0);
}

BOOST_FIXTURE_TEST_CASE(TestThreadName, TestEnvironment) {
    ThreadModule m("ThreadModule");

    BOOST_CHECK(qtdb_name(NULL, 0) == NULL);

    sc_core::sc_start(1, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 1), "sc_main - Running on LWP");

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 2), "ThreadModule.T1");

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 3), "ThreadModule.T2");

    BOOST_CHECK(qtdb_name(NULL, 4) == NULL);

    sc_core::sc_start(1, sc_core::SC_PS);
    // T1 has ended
    BOOST_CHECK(qtdb_name(NULL, 2) == NULL);

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 1), "sc_main - Running on LWP");
    BOOST_CHECK_EQUAL(qtdb_name(NULL, 3), "ThreadModule.T2");
    BOOST_CHECK(qtdb_name(NULL, 4) == NULL);

    sc_core::sc_start(1, sc_core::SC_PS);
    // T2 has ended
    BOOST_CHECK(qtdb_name(NULL, 3) == NULL);

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 1), "sc_main - Running on LWP");
    BOOST_CHECK(qtdb_name(NULL, 2) == NULL);
    BOOST_CHECK(qtdb_name(NULL, 4) == NULL);

    // Spawn dynamic thread T1
    m.spawnT1();
    sc_core::sc_start(1, sc_core::SC_PS);

    // T1 has been created
    BOOST_CHECK_EQUAL(qtdb_name(NULL, 2), "T1");

    BOOST_CHECK_EQUAL(qtdb_name(NULL, 1), "sc_main - Running on LWP");
    BOOST_CHECK(qtdb_name(NULL, 3) == NULL);
    BOOST_CHECK(qtdb_name(NULL, 4) == NULL);
}

BOOST_FIXTURE_TEST_CASE(TestThreadsId, TestEnvironment) {
    ThreadModule m("ThreadModule");

    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), 1);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 1), 2);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 2), 3);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 3), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), 1);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 1), 3);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 2), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), 1);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 1), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), 1);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 1), -1);

    m.spawnT1();
    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 0), 1);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 1), 2);
    BOOST_CHECK_EQUAL(qtdb_tid(NULL, 2), -1);
}

BOOST_FIXTURE_TEST_CASE(TestThreadsIdExists, TestEnvironment) {
    ThreadModule m("ThreadModule");

    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 0), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 0), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 1), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 2), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 3), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 4), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 0), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 1), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 2), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 3), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 4), -1);

    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 0), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 1), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 2), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 3), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 4), -1);

    m.spawnT1();
    sc_core::sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 0), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 1), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 2), 1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 3), -1);
    BOOST_CHECK_EQUAL(qtdb_tid_exists(NULL, 4), -1);
}

BOOST_FIXTURE_TEST_CASE(TestRegisterValueFor64BitRegisters, TestEnvironment) {
    ThreadModule m("ThreadModule");
    uint64_t value = 0;
    BOOST_CHECK(!qtdb_register_value64(NULL, 0, 0, &value));

    sc_core::sc_start(3, sc_core::SC_PS);

    BOOST_CHECK(qtdb_register_value64(NULL, 1, 0, &value));
    BOOST_CHECK(qtdb_register_value64(NULL, 1, 16, &value));
    BOOST_CHECK(qtdb_register_value64(NULL, 1, 17, &value));
    BOOST_CHECK(!qtdb_register_value64(NULL, 1, 18, &value));

    // Unknown thread ID, thread has ended
    BOOST_CHECK(!qtdb_register_value64(NULL, 2, 0, &value));

    // Stack set to 0 for thread run on LWP
    BOOST_CHECK_EQUAL(qtdb_name(NULL, 1), "sc_main - Running on LWP");
    BOOST_CHECK(qtdb_register_value64(NULL, 1, 7, &value));
}

BOOST_FIXTURE_TEST_CASE(TestRegisterValueFor32BitRegisters, TestEnvironment) {
    ThreadModule m("ThreadModule");
    uint32_t value = 0;
    BOOST_CHECK(!qtdb_register_value32(NULL, 0, 18, &value));

    sc_core::sc_start(3, sc_core::SC_PS);

    BOOST_CHECK(!qtdb_register_value32(NULL, 1, 17, &value));
    BOOST_CHECK(qtdb_register_value32(NULL, 1, 18, &value));
    BOOST_CHECK(qtdb_register_value32(NULL, 1, 23, &value));
    BOOST_CHECK(!qtdb_register_value32(NULL, 1, 24, &value));

    // Unknown thread ID, thread has ended
    BOOST_CHECK(!qtdb_register_value32(NULL, 2, 18, &value));
}

BOOST_FIXTURE_TEST_CASE(TestPerformanceThreadCreate, TestEnvironment) {
    ThreadModule m("ThreadModule");
    sc_core::sc_start(3, sc_core::SC_PS);
    // Only Main thread left
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    // Spawn dynamic threads
    static const int threads = 10000;
    for (int i = 0; i < threads; ++i)
        m.spawnT1();

    // Verify that they have not been added before time measure started
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    startTimeTest();

    sc_core::sc_start(1, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), threads + 1);

    endTimeTest(5.0f);
}

BOOST_FIXTURE_TEST_CASE(TestPerformanceThreadDelete, TestEnvironment) {
    ThreadModule m("ThreadModule");
    sc_core::sc_start(3, sc_core::SC_PS);
    // Only Main thread left
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    // Spawn dynamic threads
    static const int threads = 10000;
    for (int i = 0; i < threads; ++i)
        m.spawnT1();

    sc_core::sc_start(1, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), threads + 1);

    startTimeTest();

    sc_core::sc_start(1, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    endTimeTest(1.0f);
}

BOOST_FIXTURE_TEST_CASE(TestPerformanceForTTest, TestEnvironment) {
    ThreadModule m("ThreadModule");
    sc_core::sc_start(3, sc_core::SC_PS);
    // Only Main thread left
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    // Spawn dynamic threads
    static const int threads = 0x200;
    for (int i = 0; i < threads; ++i)
        m.spawnT1();

    // Verify that they have not been added before time measure started
    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), 1);

    startTimeTest();

    sc_core::sc_start(1, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(qtdb_number_of_threads(NULL), threads + 1);

    endTimeTest(1.0f);
}

#endif  // INTC_EXT

BOOST_AUTO_TEST_SUITE_END()
