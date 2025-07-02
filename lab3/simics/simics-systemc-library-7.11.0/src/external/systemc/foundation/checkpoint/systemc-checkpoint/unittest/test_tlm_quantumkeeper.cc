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
#include <tlm_utils/tlm_quantumkeeper.h>

#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/tlm_quantumkeeper.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

using sc_core::sc_time;

BOOST_AUTO_TEST_SUITE(TestSerializerTlmQuantumKeeper)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    tlm_utils::tlm_quantumkeeper quantumkeeper_;

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(quantumkeeper_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseQuantumKeeperLocalTime,
                        unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_core::sc_time t1(1, sc_core::SC_PS);
    sc_core::sc_time t2(2, sc_core::SC_PS);

    module.quantumkeeper_.set(t1);
    store();

    module.quantumkeeper_.set(t2);
    BOOST_CHECK_EQUAL(module.quantumkeeper_.get_local_time(), t2);

    restore();
    BOOST_CHECK_EQUAL(module.quantumkeeper_.get_local_time(), t1);
}

BOOST_FIXTURE_TEST_CASE(TestReverseQuantumKeeperNextSyncPoint,
                        unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_core::sc_time t(1, sc_core::SC_PS);
    tlm_utils::tlm_quantumkeeper::set_global_quantum(t);

    sc_start(5, sc_core::SC_PS);

    module.quantumkeeper_.reset();
    BOOST_CHECK_EQUAL(module.quantumkeeper_.need_sync(), false);

    storeWithKernel();

    sc_start(5, sc_core::SC_PS);
    module.quantumkeeper_.reset();
    restoreWithKernel();
    BOOST_CHECK_EQUAL(module.quantumkeeper_.need_sync(), false);

    module.quantumkeeper_.inc(t);
    BOOST_CHECK_EQUAL(module.quantumkeeper_.need_sync(), true);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreQuantumKeeperNextSyncPoint,
                        unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_core::sc_time t(1, sc_core::SC_PS);
    tlm_utils::tlm_quantumkeeper::set_global_quantum(t);

    sc_start(5, sc_core::SC_PS);
    module.quantumkeeper_.reset();

    storeWithKernel();
    {
        unittest::Simulation s;
        TestModule module("TestModule");
        sc_core::sc_time t(1, sc_core::SC_PS);
        tlm_utils::tlm_quantumkeeper::set_global_quantum(t);

        restoreWithKernel();

        BOOST_CHECK_EQUAL(module.quantumkeeper_.need_sync(), false);

        module.quantumkeeper_.inc(t);
        BOOST_CHECK_EQUAL(module.quantumkeeper_.need_sync(), true);
    }
}

BOOST_AUTO_TEST_SUITE_END()
