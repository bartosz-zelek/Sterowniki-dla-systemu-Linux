// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/test/unit_test.hpp>

#include <systemc>

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/oarchive.h>

#include <unittest/simulation.h>

#include <cstdlib>

#include <string>

static void* delete_address_filter = NULL;
static int delete_object_count = 0;

// Calling free with anything but a null-pointer or a pointer that's been
// returned by malloc, calloc, or realloc, is undefined behavior. Thus, if we
// override delete we must also override new
void *operator new(std::size_t size) {
    void *addr = std::malloc(size);
    if (!addr)
        throw std::bad_alloc();

    return addr;
}

void operator delete(void *p) {
    if (delete_address_filter == p)
        ++delete_object_count;

    std::free(p);
}

BOOST_AUTO_TEST_SUITE(TestKernel)

using boost::property_tree::ptree;

using sc_core::sc_time;
using sc_core::sc_time_stamp;
using sc_core::sc_start;

class Env {
  public:
    void store(sc_checkpoint::Kernel *kernel) {
        ptree_.clear();
        sc_checkpoint::OStateTree ot(&ptree_);
        sc_checkpoint::OArchive oa(&ot);
        kernel->save(oa, 0);
    }
    void store() {
        store(&kernel_);
    }
    void restore(sc_checkpoint::Kernel *kernel) {
        // Perform init in accordance with Adapter implementation.
        sc_start(sc_core::SC_ZERO_TIME);

        sc_checkpoint::IStateTree it(&ptree_);
        sc_checkpoint::IArchive ia(&it);
        kernel->load(ia, 0);
    }
    void reverse(sc_checkpoint::Kernel *kernel) {
        sc_checkpoint::IStateTree it(&ptree_);
        sc_checkpoint::IArchive ia(&it);
        kernel->set_reverse_execution(true);
        kernel->load(ia, 0);
    }
    void restore() {
        restore(&kernel_);
    }
    void restoreStandaloneCase(sc_checkpoint::Kernel *kernel) {
        // No initial sc_start is performed.
        sc_checkpoint::IStateTree it(&ptree_);
        sc_checkpoint::IArchive ia(&it);
        kernel->load(ia, 0);
    }
    void reverse() {
        reverse(&kernel_);
    }
    bool compare(ptree *tree0, ptree *tree1) {
        std::stringstream ss0;
        boost::property_tree::json_parser::write_json(ss0, *tree0);
        std::stringstream ss1;
        boost::property_tree::json_parser::write_json(ss1, *tree1);
        return ss0.str() == ss1.str();
    }

    unittest::Simulation simulation_;
    sc_checkpoint::Kernel kernel_;
    boost::property_tree::ptree ptree_;
};

SC_MODULE(EventModule) {
    SC_CTOR(EventModule) : runs_(0) {
        SC_METHOD(process);
        sensitive << event_;
        dont_initialize();
    }
    void process() {
        ++runs_;
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(NextTriggerModule) {
    SC_CTOR(NextTriggerModule) : runs_(0), skip_next_trigger_(false) {
        SC_METHOD(process);
    }
    void process() {
        ++runs_;
        if (!skip_next_trigger_)
            next_trigger(event_);
    }

    sc_core::sc_event event_;
    int runs_;
    bool skip_next_trigger_;
};

SC_MODULE(KernelEventModule) {
    SC_CTOR(KernelEventModule)
        : event_((std::string(SC_KERNEL_EVENT_PREFIX)+"_event").c_str()),
          runs_(0) {
        SC_METHOD(process);
        sensitive << event_;
        dont_initialize();
    }
    void process() {
        ++runs_;
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(KernelEventWithNameCollisionModule) {
    SC_CTOR(KernelEventWithNameCollisionModule)
        : event_1_((std::string(SC_KERNEL_EVENT_PREFIX)+"_event_1").c_str()),
          event_2_((std::string(SC_KERNEL_EVENT_PREFIX)+"_event").c_str()),
          event_3_((std::string(SC_KERNEL_EVENT_PREFIX)+"_event").c_str()),
          runs_(0) {
        SC_METHOD(process);
        sensitive << event_1_;
        sensitive << event_2_;
        sensitive << event_3_;
        dont_initialize();
    }
    void process() {
        ++runs_;
    }

    sc_core::sc_event event_1_;
    sc_core::sc_event event_2_;
    sc_core::sc_event event_3_;
    int runs_;
};

SC_MODULE(SignalModule) {
    SC_CTOR(SignalModule) : runs_(0) {
        SC_METHOD(process);
        sensitive << signal_;
        dont_initialize();
    }
    void process() {
        ++runs_;
    }

    sc_core::sc_signal<int> signal_;
    int runs_;
};

SC_MODULE(ThreadModuleWithExit) {
    SC_CTOR(ThreadModuleWithExit) : runs_(0) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        ++runs_;
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(ThreadModuleWithKill) {
    SC_CTOR(ThreadModuleWithKill) : runs_(0) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        ++runs_;
        wait(event_);
        sc_core::sc_get_current_process_handle().kill();
        wait(event_);
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(ThreadModuleWithExitAndInit) {
    SC_CTOR(ThreadModuleWithExitAndInit) : runs_(0) {
        SC_THREAD(thread);
    }
    void thread() {
        ++runs_;
    }

    int runs_;
};

SC_MODULE(MethodModuleWithKill) {
    SC_CTOR(MethodModuleWithKill) : runs_(0) {
        SC_METHOD(method);
        sensitive << event_;
        dont_initialize();
    }
    void method() {
        ++runs_;
        sc_core::sc_get_current_process_handle().kill();
    }

    sc_core::sc_event event_;
    int runs_;
};

class Scope {
  public:
    Scope() {
        ++scope_entered_;
    }
    ~Scope() {
        ++scope_left_;
    }
    static void reset() {
        scope_entered_ = 0;
        scope_left_ = 0;
    }
    static int scope_entered_;
    static int scope_left_;
};

int Scope::scope_entered_ = 0;
int Scope::scope_left_ = 0;

SC_MODULE(ThreadModuleWithoutExit) {
    SC_CTOR(ThreadModuleWithoutExit) : runs_(0) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        Scope scope;
        while (true) {
            ++runs_;
            sc_core::wait(event_);
        }
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(ThreadModuleWithoutExitAndInit) {
    SC_CTOR(ThreadModuleWithoutExitAndInit) : runs_(0) {
        SC_THREAD(thread);
        sensitive << event_;
    }
    void thread() {
        while (true) {
            ++runs_;
            sc_core::wait(event_);
        }
    }

    sc_core::sc_event event_;
    int runs_;
};

SC_MODULE(ThreadModulePostEvent) {
    SC_CTOR(ThreadModulePostEvent) : event_(NULL), runs_(0) {
        SC_THREAD(thread);
    }
    void thread() {
        while (true) {
            ++runs_;
            if (event_)
                event_->notify();

            sc_core::wait(5, sc_core::SC_PS);
        }
    }
    sc_core::sc_event *event_;
    int runs_;
};

SC_MODULE(ThreadModuleTimed) {
    SC_CTOR(ThreadModuleTimed) : runs_(0) {
        SC_THREAD(thread);
    }
    void thread() {
        while (true) {
            sc_core::wait(5, sc_core::SC_PS);
            ++runs_;
        }
    }

    int runs_;
};

SC_MODULE(ThreadModuleTimedWithoutInit) {
    SC_CTOR(ThreadModuleTimedWithoutInit)
        : runs_(0), delay_(5), callback_(NULL) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        while (true) {
            sc_core::wait(delay_, sc_core::SC_PS);
            ++runs_;
            if (callback_)
                (*callback_)(this);
        }
    }

    class Callback {
      public:
        virtual ~Callback() {}
        virtual void operator()(ThreadModuleTimedWithoutInit *module) = 0;
    };
    sc_core::sc_event event_;
    int runs_;
    int delay_;
    Callback *callback_;
};

SC_MODULE(ThreadModuleTwoStates) {
    enum {
        STATE_WAIT_FOR_EVENT,
        STATE_WAIT_FOR_TIME,
        STATE_DONE
    };
    SC_CTOR(ThreadModuleTwoStates) : runs_(0), state_(STATE_WAIT_FOR_EVENT) {
        SC_THREAD(thread);
    }
    void thread() {
        while (true) {
            switch (state_) {
                case STATE_WAIT_FOR_EVENT: {
                    sc_core::wait(event_);
                    state_ = STATE_WAIT_FOR_TIME;
                }
                break;
                case STATE_WAIT_FOR_TIME: {
                    sc_core::wait(5, sc_core::SC_PS);
                    state_ = STATE_DONE;
                }
                break;
                case STATE_DONE: {
                    sc_core::wait();
                }
                break;
            }
            ++runs_;
        }
    }

    sc_core::sc_event event_;
    int runs_;
    int state_;
};

SC_MODULE(ThreadModuleExceptionHandling) {
    SC_CTOR(ThreadModuleExceptionHandling) : cleanup_done_(false) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        Scope scope;
        while (true) {
            try {
                sc_core::wait(event_);
            } catch(const sc_core::sc_unwind_exception &e) {
                cleanup_done_ = true;
                throw;
            }
        }
    }

    bool cleanup_done_;
    sc_core::sc_event event_;
};

SC_MODULE(DynamicEventModule) {
    SC_CTOR(DynamicEventModule) : runs_(0), dynamic_event_(NULL) {
        SC_THREAD(thread);
        sensitive << event_;
        dont_initialize();
    }
    void thread() {
        ++runs_;
        sc_core::sc_event event;
        dynamic_event_ = &event;
        sc_core::wait(event);
        ++runs_;
    }

    sc_core::sc_event event_;
    int runs_;
    sc_core::sc_event *dynamic_event_;
};

SC_MODULE(ThreadModuleWithThreadRunInDeltaCycle) {
    SC_CTOR(ThreadModuleWithThreadRunInDeltaCycle)
            : runs_(0), runs_in_thread1_(0), runs_in_thread2_(0),
              runs_in_process_(0) {
        SC_THREAD(thread);

        SC_THREAD(thread1);
        sensitive << event1_;
        dont_initialize();

        SC_THREAD(thread2);
        sensitive << event2_;
        dont_initialize();

        SC_METHOD(process);
        sensitive << event3_;
        dont_initialize();
    }
    void thread() {
        while (true) {
            sc_core::wait(5, sc_core::SC_PS);
            ++runs_;
            event1_.notify();
            event2_.notify(sc_core::SC_ZERO_TIME);
            event3_.notify(sc_core::SC_ZERO_TIME);
        }
    }
    void thread1() {
        while (true) {
            wait(sc_core::SC_ZERO_TIME);
            ++runs_in_thread1_;
            wait();
        }
    }
    void thread2() {
        while (true) {
            ++runs_in_thread2_;
            wait();
        }
    }
    void process() {
        ++runs_in_process_;
    }

    int runs_;
    int runs_in_thread1_;
    int runs_in_thread2_;
    int runs_in_process_;
    sc_core::sc_event event1_;
    sc_core::sc_event event2_;
    sc_core::sc_event event3_;
};

BOOST_FIXTURE_TEST_CASE(TestRestoreNotStarted, Env) {
    store();
    {
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        restore(&k);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreNotStartedWithThread, Env) {
    ThreadModuleWithoutExit m1("ThreadModuleWithoutExit");
    store();
    BOOST_CHECK_EQUAL(m1.runs_, 0);
    {
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        ThreadModuleWithoutExit m2("ThreadModuleWithoutExit");
        restore(&k);
        BOOST_CHECK_EQUAL(m2.runs_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreNotStartedWithThreadAndInit, Env) {
    ThreadModuleWithoutExitAndInit m1("ThreadModuleWithoutExitAndInit");
    store();
    BOOST_CHECK_EQUAL(m1.runs_, 0);
    {
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        ThreadModuleWithoutExitAndInit m2("ThreadModuleWithoutExitAndInit");
        restore(&k);
        BOOST_CHECK_EQUAL(m2.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreStandalone, Env) {
    ThreadModuleTimed m("ThreadModuleTimed");
    store();
    {
        unittest::Simulation s;
        ThreadModuleTimed m("ThreadModuleTimed");
        sc_checkpoint::Kernel k;
        restoreStandaloneCase(&k);
        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(0));
        BOOST_CHECK_EQUAL(m.runs_, 0);

        sc_start(6, sc_core::SC_PS);

        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(6));
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreAfterScInit, Env) {
    sc_start(sc_core::SC_ZERO_TIME);
    store();
    {
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        restore(&k);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    }
}

BOOST_FIXTURE_TEST_CASE(TestNoDiffInPtrees, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    store();

    ptree ptree0 = ptree_;
    store();

    BOOST_CHECK(compare(&ptree0, &ptree_));
}

BOOST_FIXTURE_TEST_CASE(TestRestoreTime, Env) {
    sc_start(5, sc_core::SC_PS);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(10));
    {
        // This line creates a new SystemC Kernel context. Initialized with
        // the default settings. It is inside this new kernel the restore of
        // the content of the former one is tested.
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(10));
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseTime, Env) {
    sc_start(5, sc_core::SC_PS);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(10));
    reverse();
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(10));
}

BOOST_FIXTURE_TEST_CASE(TestRestoreExternalTimeInformation, Env) {
    kernel_.set_time_information(10, 1000);
    store();
    {
        unittest::Simulation s;
        sc_checkpoint::Kernel k;
        restore(&k);
        int64_t high = 0;
        uint64_t low = 0;
        k.time_information(&high, &low);
        BOOST_CHECK_EQUAL(high, 10);
        BOOST_CHECK_EQUAL(low, static_cast<unsigned> (1000) );
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    {
        unittest::Simulation s;
        EventModule m("EventModule");
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreCancelTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    {
        unittest::Simulation s;
        EventModule m("EventModule");
        sc_checkpoint::Kernel k;
        restore(&k);
        m.event_.cancel();
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreNextTrigger, Env) {
    NextTriggerModule m("NextTriggerModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.skip_next_trigger_ = true;
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    {
        unittest::Simulation s;
        NextTriggerModule m("NextTriggerModule");
        m.skip_next_trigger_ = true;
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 2);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreCancelNextTrigger, Env) {
    NextTriggerModule m("NextTriggerModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.skip_next_trigger_ = true;
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    {
        unittest::Simulation s;
        NextTriggerModule m("NextTriggerModule");
        m.skip_next_trigger_ = true;
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        m.event_.cancel();
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseZeroTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(0, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseTimedKernelEvent, Env) {
    KernelEventModule m("KernelEventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseTimedKernelEventWithNameCollision, Env) {
    KernelEventWithNameCollisionModule m("KernelEventWithNameCollision");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_1_.notify(sc_time(1, sc_core::SC_PS));
    m.event_2_.notify(sc_time(2, sc_core::SC_PS));
    m.event_3_.notify(sc_time(3, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(2, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 3);
    reverse();
    sc_start(2, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 4);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 5);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 6);
}

BOOST_FIXTURE_TEST_CASE(TestReverseTimedEventWhenCancelled, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    m.event_.cancel();
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestReverseClearTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    store();
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestReverseCancelTimedEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    m.event_.cancel();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestReverseCancelTimedEventBeforeStore, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.cancel();
    store();
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestReverseCancelTimedEventWhenCancelled, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    m.event_.cancel();
    reverse();
    m.event_.cancel();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestReverseCancelTimedEventWhenSavedEventIsNotTriggered,
                        Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify(sc_time(1, sc_core::SC_PS));
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    // The sc_event_timed is stored and still pending.
    reverse();
    // The previous pending events are cleared.
    // In this case, the same event is added during rebuild.
    m.event_.cancel();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreImmediateEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    {
        unittest::Simulation s;
        EventModule m("EventModule");
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseImmediateEvent, Env) {
    EventModule m("EventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseExitedThread, Env) {
    ThreadModuleWithExit m("ThreadModuleWithExit");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify();
    store();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_AUTO_TEST_CASE(TestReverseExitedThreadIsDeleted) {
    {
        Env e;
        ThreadModuleWithExit m("ThreadModuleWithExit");
        // Object lookup needs to be done before the thread exits.
        // sc_find_object returns NULL for an exited thread.
        delete_address_filter = sc_core::sc_find_object(
            "ThreadModuleWithExit.thread");
        sc_start(5, sc_core::SC_PS);
        m.event_.notify();
        e.store();
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        e.reverse();
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 2);

        BOOST_CHECK_EQUAL(delete_object_count, 0);
    }
    BOOST_CHECK_EQUAL(delete_object_count, 1);
}

BOOST_FIXTURE_TEST_CASE(TestReverseKillSelfThread, Env) {
    ThreadModuleWithKill m("ThreadModuleWithKill");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify();
    store();
    BOOST_CHECK_EQUAL(m.runs_, 0);

    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);

    for (int i = 2; i <= 10; ++i) {
        reverse();
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, i);
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseExitedThreadWithInit, Env) {
    ThreadModuleWithExitAndInit m("ThreadModuleWithExitAndInit");
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseExitedThreadWithInitAtZeroTime, Env) {
    ThreadModuleWithExitAndInit m("ThreadModuleWithExitAndInit");
    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    // This thread has exited before is has been stored
    // It shall not be run again.
    store();
    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestReversePreserveThreadExecutionOrder, Env) {
    class Callbacks : public ThreadModuleTimedWithoutInit::Callback {
      public:
        Callbacks() : last_called_(NULL) {}
        virtual void operator()(ThreadModuleTimedWithoutInit *module) {
            last_called_ = module;
        }
        sc_core::sc_object *last_called_;
    } callbacks;

    ThreadModuleTimedWithoutInit m("ThreadModuleTimedWithoutInit");
    ThreadModuleTimedWithoutInit m2("ThreadModuleTimedWithoutInit2");

    m.callback_ = &callbacks;
    m2.callback_ = &callbacks;

    sc_start(0, sc_core::SC_PS);

    // Activation event at 5
    m2.event_.notify(2, sc_core::SC_PS);
    m2.delay_ = 3;

    // Activation event at 5
    m.event_.notify(1, sc_core::SC_PS);
    m.delay_ = 4;

    // Thread of module m registers timed event for wake up
    sc_start(2, sc_core::SC_PS);

    // Thread of module m2 registers timed event for wake up
    sc_start(0, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(m.runs_, 0);
    BOOST_CHECK_EQUAL(m2.runs_, 0);

    // Save timed events for thread activation at 5 for both threads.
    store();

    sc_start(4, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    BOOST_CHECK_EQUAL(m2.runs_, 1);

    // Thread of module m is run before thread of module m2.
    BOOST_CHECK_EQUAL(callbacks.last_called_, &m2);

    // Thread stack of module m registers timed event @ 6ps (2 + 4)
    // Thread stack of module m2 registers timed event @ 5ps (2 + 3)
    reverse();

    sc_start(4, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    BOOST_CHECK_EQUAL(m2.runs_, 2);

    // Thread of module m is run before thread of module m2.
    BOOST_CHECK_EQUAL(callbacks.last_called_, &m2);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreExitedThread, Env) {
    ThreadModuleWithExit m("ThreadModuleWithExit");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    store();
    {
        unittest::Simulation s;
        ThreadModuleWithExit m("ThreadModuleWithExit");
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreExitedThreadAtZeroTime, Env) {
    ThreadModuleWithExit m("ThreadModuleWithExit");
    sc_start(0, sc_core::SC_PS);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    {
        unittest::Simulation s;
        ThreadModuleWithExit m("ThreadModuleWithExit");
        sc_checkpoint::Kernel k;
        restore(&k);

        // Thread has not been run before the checkpoint was saved.
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(0, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreCleansUpThreadForStackInit, Env) {
    ThreadModuleWithExit m("ThreadModuleWithExit");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify();
    sc_start(5, sc_core::SC_PS);
    sc_checkpoint::AllScObjects objs;
    store();
    {
        unittest::Simulation s;
        ThreadModuleWithExit m("ThreadModuleWithExit");
        sc_checkpoint::Kernel k;
        restore(&k);
        sc_checkpoint::AllScObjects restored_objs;

        BOOST_CHECK_EQUAL(objs.size(), restored_objs.size());
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreKeepsThreadResetOrderAsWhenThreadsCreated,
                        Env) {
    // The order of module construction determines which module thread is run
    // first.
    ThreadModulePostEvent m2("ThreadModulePostEvent");
    ThreadModuleWithoutExitAndInit m1("ThreadModuleWithoutExitAndInit");
    m2.event_ = &m1.event_;

    sc_start(0, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(m1.runs_, 1);
    BOOST_CHECK_EQUAL(m2.runs_, 1);

    store();
    {
        unittest::Simulation s;
        ThreadModulePostEvent m2("ThreadModulePostEvent");
        ThreadModuleWithoutExitAndInit m1("ThreadModuleWithoutExitAndInit");
        m2.event_ = &m1.event_;
        sc_checkpoint::Kernel k;
        restore(&k);

        BOOST_CHECK_EQUAL(m1.runs_, 2);
        BOOST_CHECK_EQUAL(m2.runs_, 2);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreMethod, Env) {
    MethodModuleWithKill m("MethodModuleWithKill");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    {
        unittest::Simulation s;
        MethodModuleWithKill m("MethodModuleWithKill");
        sc_checkpoint::Kernel k;
        restore(&k);

        // Method did not run before checkpoint was stored.
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseMethod, Env) {
    MethodModuleWithKill m("MethodModuleWithKill");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);

    reverse();

    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreThread, Env) {
    ThreadModuleWithoutExit m("ThreadModuleWithoutExit");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    {
        unittest::Simulation s;
        ThreadModuleWithoutExit m("ThreadModuleWithoutExit");
        sc_checkpoint::Kernel k;
        restore(&k);

        // Thread did not run before checkpoint was stored.
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestReverseThread, Env) {
    ThreadModuleWithoutExit m("ThreadModuleWithoutExit");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);

    Scope::reset();
    reverse();

    // Thread did not run before checkpoint was stored.
    // Check that the stack has been unwind.
    BOOST_CHECK_EQUAL(Scope::scope_left_, 1);
    // Check that the stack has not been built up.
    BOOST_CHECK_EQUAL(Scope::scope_entered_, 0);

    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(5, sc_core::SC_PS);

    // Check that the stack has been unwind.
    BOOST_CHECK_EQUAL(Scope::scope_left_, 1);
    // Check that the stack has been built up.
    BOOST_CHECK_EQUAL(Scope::scope_entered_, 1);

    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseThreadNotInitialized, Env) {
    ThreadModuleWithoutExit m("ThreadModuleWithoutExit");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    m.event_.notify();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);

    Scope::reset();
    reverse();

    // Check that the stack has been unwind.
    BOOST_CHECK_EQUAL(Scope::scope_left_, 1);
    // Check that the stack has been not been built up.
    BOOST_CHECK_EQUAL(Scope::scope_entered_, 0);

    BOOST_CHECK_EQUAL(m.runs_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestReverseThreadNotAtZeroTime, Env) {
    ThreadModuleTimed m("ThreadModuleTimed");
    sc_start(3, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    sc_start(2, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    reverse();
    sc_start(2, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseThreadAtZeroTime, Env) {
    ThreadModuleWithoutExitAndInit m("ThreadModuleWithoutExitAndInit");
    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    store();
    reverse();
    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestReverseThreadTwoStates, Env) {
    ThreadModuleTwoStates m("ThreadModuleTwoStates");
    sc_start(0, sc_core::SC_PS);
    m.event_.notify();
    store();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_WAIT_FOR_EVENT);

    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_WAIT_FOR_TIME);

    m.state_ = ThreadModuleTwoStates::STATE_WAIT_FOR_EVENT;
    reverse();
    BOOST_CHECK_EQUAL(m.runs_, 1);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_WAIT_FOR_EVENT);

    sc_start(0, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_WAIT_FOR_TIME);

    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_WAIT_FOR_TIME);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 3);
    BOOST_CHECK_EQUAL(m.state_ , ThreadModuleTwoStates::STATE_DONE);
}

BOOST_FIXTURE_TEST_CASE(TestReverseThreadWithExceptionHandling, Env) {
    ThreadModuleExceptionHandling m("ThreadModuleExceptionHandling");
    sc_start(5, sc_core::SC_PS);
    m.event_.notify();
    sc_start(5, sc_core::SC_PS);
    store();

    BOOST_CHECK_EQUAL(m.cleanup_done_, false);

    Scope::reset();
    reverse();

    BOOST_CHECK_EQUAL(Scope::scope_entered_, 1);
    BOOST_CHECK_EQUAL(Scope::scope_left_, 1);
    BOOST_CHECK_EQUAL(m.cleanup_done_, true);
}

BOOST_FIXTURE_TEST_CASE(TestReverseChannelUpdate, Env) {
    SignalModule m("SignalModule");
    sc_start(5, sc_core::SC_PS);
    m.signal_ = 1;
    BOOST_CHECK_EQUAL(m.runs_, 0);
    // Store the old value. That is done by the Serializer outside this test.
    int &value = const_cast<int &>(m.signal_.read());
    int old_value = value;
    store();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    // Restore the old value. That is done by the Serializer outside this test.
    value = old_value;
    reverse();
    BOOST_CHECK_EQUAL(m.runs_, 1);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 2);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreMultipleChannelUpdate, Env) {
    SignalModule m1("SignalModule1");
    SignalModule m2("SignalModule2");
    sc_start(5, sc_core::SC_PS);
    m1.signal_ = 1;
    m2.signal_ = 10;
    store();
    {
        unittest::Simulation s;
        SignalModule m1("SignalModule1");
        SignalModule m2("SignalModule2");
        sc_checkpoint::Kernel k;
        restore(&k);
        // Restore the old value.
        const_cast<int &>(m1.signal_.read()) = 1;
        const_cast<int &>(m2.signal_.read()) = 10;

        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m1.runs_, 1);
        BOOST_CHECK_EQUAL(m2.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreThreadRunInDeltaCycle, Env) {
    ThreadModuleWithThreadRunInDeltaCycle
        m("ThreadModuleWithThreadRunInDeltaCycle");
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    // Verify that a store has no effects on subsequent ones.
    store();
    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(m.runs_, 0);
    BOOST_CHECK_EQUAL(m.runs_in_thread1_, 0);
    BOOST_CHECK_EQUAL(m.runs_in_thread2_, 0);
    BOOST_CHECK_EQUAL(m.runs_in_process_, 0);

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(m.runs_, 1);
    BOOST_CHECK_EQUAL(m.runs_in_thread1_, 0);
    BOOST_CHECK_EQUAL(m.runs_in_thread2_, 0);
    BOOST_CHECK_EQUAL(m.runs_in_process_, 0);

    store();

    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    BOOST_CHECK_EQUAL(m.runs_, 1);
    BOOST_CHECK_EQUAL(m.runs_in_thread1_, 1);
    BOOST_CHECK_EQUAL(m.runs_in_thread2_, 1);
    BOOST_CHECK_EQUAL(m.runs_in_process_, 1);
    {
        unittest::Simulation s;
        ThreadModuleWithThreadRunInDeltaCycle
            m("ThreadModuleWithThreadRunInDeltaCycle");
        sc_checkpoint::Kernel k;
        restore(&k);

        BOOST_CHECK_EQUAL(m.runs_, 0);
        BOOST_CHECK_EQUAL(m.runs_in_thread1_, 0);
        BOOST_CHECK_EQUAL(m.runs_in_thread2_, 0);
        BOOST_CHECK_EQUAL(m.runs_in_process_, 0);

        sc_core::sc_start(sc_core::SC_ZERO_TIME);

        BOOST_CHECK_EQUAL(m.runs_, 0);
        BOOST_CHECK_EQUAL(m.runs_in_thread1_, 1);
        BOOST_CHECK_EQUAL(m.runs_in_thread2_, 1);
        BOOST_CHECK_EQUAL(m.runs_in_process_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreDeltaCount, Env) {
    ThreadModuleWithThreadRunInDeltaCycle
        m("ThreadModuleWithThreadRunInDeltaCycle");
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    sc_start(1000, sc_core::SC_PS);

    BOOST_CHECK(sc_core::sc_delta_count() == 399);

    store();

    sc_core::sc_start(sc_core::SC_ZERO_TIME);
    BOOST_CHECK(sc_core::sc_delta_count() == 400);
    {
        unittest::Simulation s;
        ThreadModuleWithThreadRunInDeltaCycle
            m("ThreadModuleWithThreadRunInDeltaCycle");
        sc_checkpoint::Kernel k;
        restore(&k);

        BOOST_CHECK(sc_core::sc_delta_count() == 399);

        sc_core::sc_start(sc_core::SC_ZERO_TIME);

        BOOST_CHECK(sc_core::sc_delta_count() == 400);
    }
}

/*
 * Currently commented out. New solution necessary for checkpointing
 * of dynamic events.
BOOST_FIXTURE_TEST_CASE(TestRestoreDynamicTimedEvent, Env) {
    DynamicEventModule m("DynamicEventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.dynamic_event_->notify(sc_time(1, sc_core::SC_PS));
    store();
    {
        unittest::Simulation s;
        DynamicEventModule m("DynamicEventModule");
        sc_checkpoint::Kernel k;
        restore(&k);
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreDynamicTimedEventNotRunBeforeStore, Env) {
    DynamicEventModule m("DynamicEventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    store();
    {
        unittest::Simulation s;
        DynamicEventModule m("DynamicEventModule");
        sc_checkpoint::Kernel k;
        restore(&k);

        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 0);
        m.event_.notify();
        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        m.dynamic_event_->notify(sc_time(1, sc_core::SC_PS));
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 2);
    }
}

BOOST_FIXTURE_TEST_CASE(TestRestoreDynamicImmediateEvent, Env) {
    DynamicEventModule m("DynamicEventModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 0);
    m.event_.notify();
    BOOST_CHECK_EQUAL(m.runs_, 0);
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(m.runs_, 1);
    m.dynamic_event_->notify();
    store();
    {
        unittest::Simulation s;
        DynamicEventModule m("DynamicEventModule");
        sc_checkpoint::Kernel k;
        restore(&k);

        BOOST_CHECK_EQUAL(m.runs_, 0);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(m.runs_, 1);
    }
}
*/
BOOST_AUTO_TEST_SUITE_END()
