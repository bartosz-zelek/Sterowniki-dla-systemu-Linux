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

#include <systemc-checkpoint/serialization/sc_ppq.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

using sc_core::sc_time;

BOOST_AUTO_TEST_SUITE(TestSerializerPpq)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) : queue_(3, compare) {}
    sc_core::sc_ppq<sc_time *> queue_;

    static int compare(const void *p1, const void *p2) {
        const sc_time* t1 = static_cast<const sc_time*>(p1);
        const sc_time* t2 = static_cast<const sc_time*>(p2);

        if (*t1 < *t2)
            return 1;

        if (*t1 > *t2)
            return -1;

        return 0;
    }

    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(queue_);
    }
};

sc_checkpoint::serialization::Serializer<TestModule> serializer_;

BOOST_FIXTURE_TEST_CASE(TestReverseTime, unittest::SerializerEnvironment) {
    TestModule module("TestModule");

    module.queue_.insert(new sc_time(1, sc_core::SC_PS));
    module.queue_.insert(new sc_time(3, sc_core::SC_PS));
    module.queue_.insert(new sc_time(2, sc_core::SC_PS));

    store();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(1, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(2, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(3, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_CHECK_EQUAL(module.queue_.empty(), true);

    restore();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(1, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(2, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_REQUIRE_EQUAL(module.queue_.empty(), false);
    BOOST_CHECK_EQUAL(*module.queue_.top(), sc_time(3, sc_core::SC_PS));
    delete module.queue_.extract_top();

    BOOST_CHECK_EQUAL(module.queue_.empty(), true);
}

BOOST_AUTO_TEST_SUITE_END()
