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

#include <boost/test/unit_test.hpp>

#include <systemc>

#include <systemc-checkpoint/serialization/sc_mutex.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScMutex)

SC_MODULE(TestModule) {
    sc_core::sc_mutex mutex_;
    bool locked_by_thread_1;
    bool locked_by_thread_2;
    int state_thread_1;
    int state_thread_2;

    SC_CTOR(TestModule)
    : locked_by_thread_1(false), locked_by_thread_2(false),
      state_thread_1(0), state_thread_2(0) {
        SC_THREAD(thread1);
        sensitive << mutex_;

        SC_THREAD(thread2);
        sensitive << mutex_;
    }

    void thread1() {
        while (true) {
            switch (state_thread_1) {
                case 0: {
                    mutex_.lock();
                    locked_by_thread_1 = true;
                    state_thread_1 = 1;
                }
                break;
                case 1: {
                    wait(5, sc_core::SC_PS);
                    state_thread_1 = 2;
                }
                break;
                case 2: {
                    mutex_.unlock();
                    locked_by_thread_1 = false;
                    state_thread_1 = 3;
                }
                break;
                case 3: {
                    wait(1, sc_core::SC_PS);
                    state_thread_1 = 0;
                }
                break;
            }
        }
    }
    void thread2() {
        while (true) {
            switch (state_thread_2) {
                case 0: {
                    mutex_.lock();
                    locked_by_thread_2 = true;
                    state_thread_2 = 1;
                }
                break;
                case 1: {
                    wait(5, sc_core::SC_PS);
                    state_thread_2 = 2;
                }
                break;
                case 2: {
                    mutex_.unlock();
                    locked_by_thread_2 = false;
                    state_thread_2 = 3;
                }
                break;
                case 3: {
                    wait(1, sc_core::SC_PS);
                    state_thread_2 = 0;
                }
                break;
            }
        }
    }

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(mutex_);
        ar & SMD(locked_by_thread_1);
        ar & SMD(locked_by_thread_2);
        ar & SMD(state_thread_1);
        ar & SMD(state_thread_2);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseMutex, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.locked_by_thread_1, true);
    BOOST_CHECK_EQUAL(module.locked_by_thread_2, false);

    storeWithKernel();

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.locked_by_thread_1, false);
    BOOST_CHECK_EQUAL(module.locked_by_thread_2, true);

    reverseWithKernel();

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.locked_by_thread_1, false);
    BOOST_CHECK_EQUAL(module.locked_by_thread_2, true);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreMutex, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    storeWithKernel();

    {
        unittest::Simulation s;
        TestModule module("TestModule");

        restoreWithKernel();

        sc_start(5, sc_core::SC_PS);

        BOOST_CHECK_EQUAL(module.locked_by_thread_1, false);
        BOOST_CHECK_EQUAL(module.locked_by_thread_2, true);
    }
}

BOOST_AUTO_TEST_SUITE_END()
