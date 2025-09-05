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

#include <systemc-checkpoint/serialization/sc_signal.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScSignal)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    sc_core::sc_signal<int> signal_int_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(signal_int_);
    }
};

BOOST_FIXTURE_TEST_CASE(TestReverseSignal, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;

    TestModule module("TestModule");
    module.signal_int_ = 1;
    sc_start(5, sc_core::SC_PS);
    module.signal_int_ = 2;

    store();

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.signal_int_.read(), 2);

    restore();

    BOOST_CHECK_EQUAL(module.signal_int_.read(), 1);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreSignal, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;

    TestModule module("TestModule");
    module.signal_int_ = 1;
    sc_start(5, sc_core::SC_PS);
    module.signal_int_ = 2;

    store();

    int current_value_1 = module.signal_int_.read();

    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restore();
        int current_value_2 = module.signal_int_.read();

        BOOST_CHECK_EQUAL(current_value_1, current_value_2);
    }
}

BOOST_AUTO_TEST_SUITE_END()
