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

#include <simics/systemc/trace_monitor.h>
#include <simics/systemc/kernel.h>

#include <systemc>

#include "simcontext_wrapper.h"

BOOST_AUTO_TEST_SUITE(TestEventMonitor)
#if INTC_EXT

class MockTraceCallback : public simics::systemc::EventCallbackInterface {
  public:
    MockTraceCallback() : triggered_count_(0), event_(NULL), channel_(NULL),
                          delta_count(0), process_activation_(0) {}
    void event_callback(const char *event_type,
                        const char *class_type,
                        void *object,
                        const sc_core::sc_time &ts) {
        ++triggered_count_;

        if (strcmp(class_type, "sc_event") == 0)
            event_ = static_cast<sc_core::sc_event *>(object);

        if (strcmp(class_type, "sc_prim_channel") == 0)
            channel_ = static_cast<sc_core::sc_prim_channel *>(object);

        if (strcmp(event_type, INTC_TRIGGER_DELTA_CYCLE_COMPLETED) == 0)
            delta_count = *reinterpret_cast<sc_dt::uint64*>(object);

        if (strcmp(event_type, INTC_TRIGGER_PROCESS_ACTIVATION) == 0)
            ++process_activation_;
    }
    int triggered_count_;
    sc_core::sc_event *event_;
    sc_core::sc_prim_channel *channel_;
    sc_dt::uint64 delta_count;
    int process_activation_;
};

class TestDevice : public sc_core::sc_module {
  public:
    explicit TestDevice(sc_core::sc_module_name = "TestDevice")
        : event_("Event1"), event2_("Event2") {
        sc_core::sc_time delay = sc_core::sc_time::from_value(
            static_cast<sc_dt::uint64>(10));
        event_.notify(delay);
        event2_.notify(delay);
    }

    sc_core::sc_event event_;
    sc_core::sc_event event2_;
};

class TestDeviceWithKernelEvents : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestDeviceWithKernelEvents);
#endif
    explicit TestDeviceWithKernelEvents(sc_core::sc_module_name = "TestDevice")
        : event_("Event1") {
        event_.notify(sc_core::sc_time::from_value(
                          static_cast<sc_dt::uint64>(9)));
        SC_THREAD(loop);
        sensitive << event_;
    }

    void loop() {
        wait(event_);
        {
            sc_core::sc_time delay = sc_core::sc_time::from_value(
                static_cast<sc_dt::uint64>(1));
            mutex_.lock();
            wait(delay);
            mutex_.unlock();
            wait(delay);
        }
    }

    sc_core::sc_event event_;
    sc_core::sc_mutex mutex_;
};

class TestDeviceDynamicEvent : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestDeviceDynamicEvent);
#endif
    explicit TestDeviceDynamicEvent(sc_core::sc_module_name = "TestDevice")
        : event_("Event1"), event2_(NULL) {
        event_.notify(sc_core::sc_time::from_value(
                          static_cast<sc_dt::uint64>(9)));
        SC_THREAD(loop);
        sensitive << event_;
    }

    void loop() {
        wait(event_);
        {
            sc_core::sc_event event2;
            event2.notify(sc_core::sc_time::from_value(
                              static_cast<sc_dt::uint64>(1)));
            wait(event2);
            event2_ = &event2;
        }
    }

    sc_core::sc_event event_;
    sc_core::sc_event *event2_;
};

class TestRetriggerDevice : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestRetriggerDevice);
#endif
    explicit TestRetriggerDevice(sc_core::sc_module_name = "TestDevice")
        : event_("Event1") {
        event_.notify(sc_core::sc_time::from_value(
                          static_cast<sc_dt::uint64>(9)));
        SC_THREAD(loop);
        sensitive << event_;
    }

    void loop() {
        while (true) {
            wait(event_);
            event_.notify(sc_core::sc_time::from_value(
                              static_cast<sc_dt::uint64>(1)));
        }
    }

    sc_core::sc_event event_;
};

class TestNoInitialEventDevice : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestNoInitialEventDevice);
#endif
    explicit TestNoInitialEventDevice(sc_core::sc_module_name = "TestDevice")
        : event_("Event1") {
        SC_THREAD(loop);
        sensitive << event_;
    }
    void loop() {
        while (true) {
            wait(event_);
            event_.notify(sc_core::sc_time::from_value(
                              static_cast<sc_dt::uint64>(1)));
        }
    }
    void postEvent() {
        event_.notify(sc_core::sc_time::from_value(
                          static_cast<sc_dt::uint64>(9)));
    }

    sc_core::sc_event event_;
};

class TestDeviceImmediateEvent : public sc_core::sc_module {
  public:
#ifndef SYSTEMC_3_0_0
    SC_HAS_PROCESS(TestDeviceImmediateEvent);
#endif
    explicit TestDeviceImmediateEvent(sc_core::sc_module_name = "TestDevice")
        : event_("Event1") {
        event_.notify(sc_core::sc_time::from_value(
                          static_cast<sc_dt::uint64>(9)));
        SC_THREAD(loop);
        SC_THREAD(loop2);
        sensitive << event_;
    }

    void loop() {
        wait(event_);
        event2_.notify();
    }
    void loop2() {
        wait(event2_);
    }

    sc_core::sc_event event_;
    sc_core::sc_event event2_;
};

class TestDeviceASyncRequestUpdate : public sc_core::sc_module {
  public:
    class Channel : public sc_core::sc_prim_channel {
      public:
        Channel() : sc_core::sc_prim_channel("Channel") {}
    };
    explicit TestDeviceASyncRequestUpdate(
        sc_core::sc_module_name = "TestDevice") {}

    Channel channel_;
};

class TestEnvironment {
  public:
    TestEnvironment() {}
    void run(int ticks) {
        sc_core::sc_time t = sc_core::sc_time::from_value(
            static_cast<sc_dt::uint64>(ticks));
        sc_core::sc_start(t);
        while (sc_core::sc_pending_activity_at_current_time()) {
            sc_core::sc_start(sc_core::SC_ZERO_TIME);
        }
    }

    int runPendingActivity(int max) {
        for (int i = 0; i< max; ++i) {
            if (sc_core::sc_pending_activity()) {
                sc_start(sc_core::sc_time_to_pending_activity());
            } else {
                return i;
            }
        }
        return max;
    }

    unittest::SimContextWrapper context_;
    simics::systemc::TraceMonitor monitor_;
    MockTraceCallback callback_;
};

BOOST_FIXTURE_TEST_CASE(testSubscribeForEvent, TestEnvironment) {
    TestDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForMultipleEventsAtSameTime,
                        TestEnvironment) {
    TestDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event2_,
                       &callback_, true);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForEventWithRetrigger, TestEnvironment) {
    TestRetriggerDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);

    run(8);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 3);
}

BOOST_FIXTURE_TEST_CASE(testCancelEvent, TestEnvironment) {
    TestDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    device.event_.cancel();
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testCancelAndFireAtSameCycle, TestEnvironment) {
    TestDevice device;
    MockTraceCallback callback2_;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);
    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event2_,
                       &callback2_, true);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 0);

    device.event_.cancel();
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllEvents, TestEnvironment) {
    TestDevice device;
    MockTraceCallback callback2_;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback2_);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllDynamicEventsSubscribeEventNoTrace,
                        TestEnvironment) {
    TestDevice device;
    MockTraceCallback callback2_;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, false);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback2_);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testUnsubscribeForAllDynamicEvents, TestEnvironment) {
    TestDevice device;
    MockTraceCallback callback2_;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, true);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback2_);

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 0);

    monitor_.subscribe(INTC_TRIGGER_EVENT_TRIGGERED, &device.event_,
                       &callback_, false);
    monitor_.unsubscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback2_);
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    BOOST_CHECK_EQUAL(callback2_.triggered_count_, 0);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllDynamicEvents, TestEnvironment) {
    TestDeviceDynamicEvent device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    run(8);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
    BOOST_CHECK_EQUAL(callback_.event_, &device.event_);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);
    BOOST_CHECK_EQUAL(callback_.event_, device.event2_);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllDynamicEventsIgnoresKernelEvents,
                        TestEnvironment) {
    TestDeviceWithKernelEvents device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    run(8);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllDynamicventsWithRetrigger,
                        TestEnvironment) {
    TestRetriggerDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    run(8);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 3);
}

BOOST_FIXTURE_TEST_CASE(testSubscribeForAllDynamicEventsNoInitialEvents,
                        TestEnvironment) {
    TestNoInitialEventDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    run(0);
    device.postEvent();

    run(9);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);

    run(1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(
        testSubscribeForAllDynamicEventsNoInitialEventsWithCatchup,
        TestEnvironment) {
    TestNoInitialEventDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    run(0);
    device.postEvent();

    run(20);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 12);
}

BOOST_FIXTURE_TEST_CASE(testPendingActivities, TestEnvironment) {
    TestDevice device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    BOOST_CHECK_EQUAL(runPendingActivity(2), 1);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testPendingActivitiesWithImmediateEvent,
                        TestEnvironment) {
    TestDeviceImmediateEvent device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_EVENT_TRIGGERED, &callback_);

    BOOST_CHECK_EQUAL(runPendingActivity(3), 2);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 2);
}

BOOST_FIXTURE_TEST_CASE(testASyncRequestUpdate,
                        TestEnvironment) {
    TestDeviceASyncRequestUpdate device;
    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    monitor_.subscribeAllDynamic(INTC_TRIGGER_ASYNC_REQUEST_UPDATE, &callback_);
    BOOST_CHECK_EQUAL(callback_.triggered_count_, 0);

    device.channel_.async_request_update();

    BOOST_CHECK_EQUAL(callback_.triggered_count_, 1);
    BOOST_CHECK_EQUAL(callback_.channel_, &device.channel_);
}

BOOST_FIXTURE_TEST_CASE(testMultipleDeltaCycleComplete, TestEnvironment) {
    monitor_.subscribeAllDynamic(INTC_TRIGGER_DELTA_CYCLE_COMPLETED,
                                 &callback_);
    TestDeviceImmediateEvent device;
    sc_core::sc_start(sc_core::sc_time(10, sc_core::SC_PS));

    BOOST_CHECK_EQUAL(callback_.delta_count, 1);
}

#endif  // INTC_EXT
BOOST_AUTO_TEST_SUITE_END()
