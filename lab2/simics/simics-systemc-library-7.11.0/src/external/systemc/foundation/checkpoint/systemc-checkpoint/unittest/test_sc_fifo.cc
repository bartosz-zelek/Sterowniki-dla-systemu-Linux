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

#include <systemc-checkpoint/serialization/sc_fifo.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

BOOST_AUTO_TEST_SUITE(TestSerializerScFifo)

SC_MODULE(TestModule) {
    sc_core::sc_fifo<int> fifo_;
    int count_;

    SC_CTOR(TestModule) : fifo_(3), count_(1) {
        SC_THREAD(thread);
    }

    void thread() {
        while (true) {
            fifo_.write(count_);
            ++count_;
        }
    }

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(fifo_);
        ar & SMD(count_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseFifo, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    storeWithKernel();

    BOOST_CHECK_EQUAL(module.fifo_.read(), 1);
    BOOST_CHECK_EQUAL(module.fifo_.read(), 2);
    BOOST_CHECK_EQUAL(module.fifo_.read(), 3);

    reverseWithKernel();

    BOOST_CHECK_EQUAL(module.fifo_.read(), 1);

    sc_start(5, sc_core::SC_PS);

    BOOST_CHECK_EQUAL(module.fifo_.read(), 2);
    BOOST_CHECK_EQUAL(module.fifo_.read(), 3);
    BOOST_CHECK_EQUAL(module.fifo_.read(), 4);

    BOOST_CHECK_EQUAL(module.fifo_.num_available(), 0);
}

BOOST_FIXTURE_TEST_CASE(TestRestoreFifo, unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    sc_start(5, sc_core::SC_PS);

    storeWithKernel();

    {
        unittest::Simulation s;
        TestModule module("TestModule");

        restoreWithKernel();

        BOOST_CHECK_EQUAL(module.fifo_.read(), 1);

        sc_start(5, sc_core::SC_PS);

        BOOST_CHECK_EQUAL(module.fifo_.read(), 2);
        BOOST_CHECK_EQUAL(module.fifo_.read(), 3);
        BOOST_CHECK_EQUAL(module.fifo_.read(), 4);

        BOOST_CHECK_EQUAL(module.fifo_.num_available(), 0);
    }
}

BOOST_AUTO_TEST_SUITE_END()
