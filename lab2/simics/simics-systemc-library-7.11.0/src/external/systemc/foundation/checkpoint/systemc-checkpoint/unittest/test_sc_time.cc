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

#include <systemc-checkpoint/serialization/sc_time.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScTime)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    sc_core::sc_time time_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(time_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseTime, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    module.time_ = sc_core::sc_time::from_value(1000);

    store();

    module.time_ *= 2;
    BOOST_CHECK_EQUAL(module.time_.value(),
                      static_cast<sc_core::sc_time::value_type>(2000));

    restore();
    BOOST_CHECK_EQUAL(module.time_.value(),
                      static_cast<sc_core::sc_time::value_type>(1000));
}

BOOST_AUTO_TEST_SUITE_END()
