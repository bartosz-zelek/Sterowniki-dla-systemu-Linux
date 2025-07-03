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

#include <systemc-checkpoint/serialization/sc_biguint.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScBigUInt)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    sc_dt::sc_biguint<1> v1_;
    sc_dt::sc_biguint<7> v2_;
    sc_dt::sc_biguint<8> v3_;
    sc_dt::sc_biguint<9> v4_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(v1_);
        ar & SMD(v2_);
        ar & SMD(v3_);
        ar & SMD(v4_);
    }
};

BOOST_FIXTURE_TEST_CASE(TestReverseBigUInt, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;
    TestModule module("TestModule");
    module.v1_ = 1;
    module.v2_ = 64;
    module.v3_ = 128;
    module.v4_ = 256;
    store();
    module.v1_ = 0;
    module.v2_ = 63;
    module.v3_ = 127;
    module.v4_ = 255;
    restore();

    BOOST_CHECK_EQUAL(module.v1_, 1);
    BOOST_CHECK_EQUAL(module.v2_, 64);
    BOOST_CHECK_EQUAL(module.v3_, 128);
    BOOST_CHECK_EQUAL(module.v4_, 256);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreBigUInt, unittest::SerializerEnvironment) {
    sc_checkpoint::serialization::Serializer<TestModule> serializer;
    TestModule module("TestModule");
    module.v1_ = 1;
    module.v2_ = 64;
    module.v3_ = 128;
    module.v4_ = 256;
    store();

    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restore();

        BOOST_CHECK_EQUAL(module.v1_, 1);
        BOOST_CHECK_EQUAL(module.v2_, 64);
        BOOST_CHECK_EQUAL(module.v3_, 128);
        BOOST_CHECK_EQUAL(module.v4_, 256);
    }
}

BOOST_AUTO_TEST_SUITE_END()
