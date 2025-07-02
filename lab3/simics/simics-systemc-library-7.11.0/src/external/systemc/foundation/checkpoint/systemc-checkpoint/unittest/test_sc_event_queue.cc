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

#include <systemc-checkpoint/serialization/sc_event_queue.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScEventQueue)

SC_MODULE(TestModule) {
    sc_core::sc_event_queue queue_;
    int runs_;

    SC_CTOR(TestModule) : runs_(0) {
        SC_METHOD(process);
        dont_initialize();
        sensitive << queue_;
    }

    void init() {
        queue_.notify(5, sc_core::SC_PS);
        queue_.notify(6, sc_core::SC_PS);
        queue_.notify(8, sc_core::SC_PS);
    }

    void process() {
        ++runs_;
    }

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(queue_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseQueue, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);
    module.init();
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 0);

    storeWithKernel();

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 1);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 2);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 2);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 3);

    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 3);

    reverseWithKernel();

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 4);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 5);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 5);

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 6);

    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.runs_, 6);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreQueue, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);
    module.init();
    sc_start(5, sc_core::SC_PS);
    storeWithKernel();

    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restoreWithKernel();

        sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.runs_, 1);

        sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.runs_, 2);

        sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.runs_, 2);

        sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.runs_, 3);

        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.runs_, 3);
    }
}

BOOST_AUTO_TEST_SUITE_END()
