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

#include <systemc-checkpoint/serialization/sc_clock.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

using sc_core::sc_time;

BOOST_AUTO_TEST_SUITE(TestSerializerScClock)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) : clock_("clock", sc_time(10, sc_core::SC_PS)) {}
    sc_core::sc_clock clock_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(clock_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseClock, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.clock_.read(), true);

    storeWithKernel();

    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.clock_.read(), false);

    reverseWithKernel();

    BOOST_CHECK_EQUAL(module.clock_.read(), true);
    sc_start(1, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(module.clock_.read(), false);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreClock, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    storeWithKernel();

    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restoreWithKernel();

        BOOST_CHECK_EQUAL(module.clock_.read(), true);
        sc_start(1, sc_core::SC_PS);
        BOOST_CHECK_EQUAL(module.clock_.read(), false);
    }
}

BOOST_AUTO_TEST_SUITE_END()
