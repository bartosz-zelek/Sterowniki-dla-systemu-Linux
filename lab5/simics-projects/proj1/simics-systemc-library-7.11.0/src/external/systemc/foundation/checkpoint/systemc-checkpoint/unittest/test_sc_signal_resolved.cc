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

#include <systemc-checkpoint/serialization/sc_signal_resolved.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScSignalResolved)

SC_MODULE(TestModule) {
    sc_core::sc_signal_resolved signal_;

    SC_CTOR(TestModule) {
        SC_THREAD(thread1);
        SC_THREAD(thread2);
    }
    void thread1() {
        wait(4, sc_core::SC_PS);
        signal_ = sc_dt::SC_LOGIC_1;

        // Reverse of exited threads is not supported yet.
        wait(1, sc_core::SC_NS);
    }
    void thread2() {
        wait(9, sc_core::SC_PS);
        signal_ = sc_dt::SC_LOGIC_0;

        // Reverse of exited threads is not supported yet.
        wait(1, sc_core::SC_NS);
    }
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(signal_);
    }
};

BOOST_FIXTURE_TEST_CASE(TestReverseSignal, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;

    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_1);

    storeWithKernel();

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_X);

    restoreWithKernel();
    BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_1);

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_X);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreSignal, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;

    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    storeWithKernel();

    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restoreWithKernel();
        BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_1);

        sc_start(5, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.signal_.read(), sc_dt::SC_LOGIC_X);
    }
}

BOOST_AUTO_TEST_SUITE_END()
