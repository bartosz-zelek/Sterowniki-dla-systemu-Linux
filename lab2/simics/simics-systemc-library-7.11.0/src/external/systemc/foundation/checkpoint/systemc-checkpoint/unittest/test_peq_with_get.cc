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
#include <tlm>
#include <tlm_utils/peq_with_get.h>

#include <systemc-checkpoint/serialization/peq_with_get.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

using sc_core::sc_time;

BOOST_AUTO_TEST_SUITE(TestSerializerPeqWithGet)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) : peq_("peq") {}
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> peq_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp1_);
        ar & SMD(gp2_);
        ar & SMD(gp3_);
        ar & SMD(peq_);
    }
    tlm::tlm_generic_payload gp1_;
    tlm::tlm_generic_payload gp2_;
    tlm::tlm_generic_payload gp3_;
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseTime, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_time t1(1, sc_core::SC_PS);
    sc_time t2(2, sc_core::SC_PS);
    sc_time t3(3, sc_core::SC_PS);

    module.peq_.notify(module.gp2_, t2);
    module.peq_.notify(module.gp1_, t1);
    module.peq_.notify(module.gp3_, t3);

    sc_start(2, sc_core::SC_PS);
    storeWithKernel();

    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp1_);
    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp2_);
    BOOST_REQUIRE(module.peq_.get_next_transaction() == NULL);

    sc_start(1, sc_core::SC_PS);

    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp3_);

    restoreWithKernel();

    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp1_);
    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp2_);
    BOOST_REQUIRE(module.peq_.get_next_transaction() == NULL);

    sc_start(1, sc_core::SC_PS);

    BOOST_REQUIRE_EQUAL(module.peq_.get_next_transaction(), &module.gp3_);
}

BOOST_AUTO_TEST_SUITE_END()
