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

#include <boost/property_tree/ptree.hpp>
#include <cci/serialization/serialization.hpp>
#include <boost/test/unit_test.hpp>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>

#include <sstream>

class NonTracked {
  public:
    NonTracked() : i_(0) {}
    explicit NonTracked(int i) : i_(i) {}

    template<class Archive>
    void serialize(Archive &ar, const unsigned) {
        ar & SMD(i_);
    }

    int i_;
};

class Tracked {
  public:
    Tracked() : i_(0) {}
    explicit Tracked(int i) : i_(i) {}

    template<class Archive>
    void serialize(Archive &ar, const unsigned) {
        ar & SMD(i_);
    }

    int i_;
};

BOOST_AUTO_TEST_SUITE(TestBoostTracking)

// If no pointers to a type T are serialized, then, by default, tracking is
// disabled for that type. If so, the same instance may be serialized and
// subsequently deserialized multiple times.
BOOST_AUTO_TEST_CASE(SerializeInstanceMultipleTimesNonTracked) {
    boost::property_tree::ptree ptree;
    {
        NonTracked a(4711);

        sc_checkpoint::OStateTree ost(&ptree);
        sc_checkpoint::OArchive oarchive(&ost);
        oarchive << sc_checkpoint::serialization::create_smd("a1", a)
                 << sc_checkpoint::serialization::create_smd("a2", a);
    }

    NonTracked a;
    NonTracked b;

    sc_checkpoint::IStateTree ist(&ptree);
    sc_checkpoint::IArchive iarchive(&ist);
    iarchive >> sc_checkpoint::serialization::create_smd("a1", a)
             >> sc_checkpoint::serialization::create_smd("a2", b);

    BOOST_CHECK_EQUAL(a.i_, 4711);
    BOOST_CHECK_EQUAL(b.i_, 4711);
}

// If tracking is enabled for a type T, then the same instance may not be
// serialized and subsequently deserialized multiple times. The serialization
// framework keeps track of object instances and does not proceed with
// serialization in case an object has already been serialized.
BOOST_AUTO_TEST_CASE(SerializeInstanceMultipleTimesTracked) {
    boost::property_tree::ptree ptree;
    {
        Tracked *p = NULL;  // does not matter where it points to
        Tracked a(4711);
        int next = 0x4711;

        sc_checkpoint::OStateTree ost(&ptree);
        sc_checkpoint::OArchive oarchive(&ost);
        oarchive << SMD(p)
                 << sc_checkpoint::serialization::create_smd("a1", a)
                 << sc_checkpoint::serialization::create_smd("a2", a)
                 << SMD(next);
    }

    Tracked *p = NULL;
    Tracked a, b;
    int next = 0;

    sc_checkpoint::IStateTree ist(&ptree);
    sc_checkpoint::IArchive iarchive(&ist);
    iarchive >> SMD(p)
             >> sc_checkpoint::serialization::create_smd("a1", a)
             >> sc_checkpoint::serialization::create_smd("a2", b)
             >> SMD(next);

    BOOST_CHECK_EQUAL(a.i_, 4711);
    BOOST_CHECK_EQUAL(b.i_, 0);  // not serialized
    BOOST_CHECK_EQUAL(next, 0x4711);  // following values serialized correctly
}

class CountingConstructor {
  public:
    CountingConstructor() : i_(0) {
        ++count_;
    }
    explicit CountingConstructor(int i) : i_(i) {
        ++count_;
    }

    template<class Archive>
    void serialize(Archive &ar, const unsigned) {  // NOLINT(runtime/references)
        ar & SMD(i_);
    }

    int i_;
    static int count_;
};

int CountingConstructor::count_ = 0;

BOOST_AUTO_TEST_CASE(SerializeInstanceMultipleTimesPointer) {
    boost::property_tree::ptree ptree;
    {
        CountingConstructor *p = new CountingConstructor(4711);

        sc_checkpoint::OStateTree ost(&ptree);
        sc_checkpoint::OArchive oarchive(&ost);
        oarchive << sc_checkpoint::serialization::create_smd("p1", p)
                 << sc_checkpoint::serialization::create_smd("p2", p);

        delete p;
    }

    CountingConstructor *p1 = NULL;
    CountingConstructor *p2 = NULL;

    BOOST_REQUIRE_EQUAL(CountingConstructor::count_, 1);
    sc_checkpoint::IStateTree ist(&ptree);
    sc_checkpoint::IArchive iarchive(&ist);
    iarchive >> sc_checkpoint::serialization::create_smd("p1", p1)
             >> sc_checkpoint::serialization::create_smd("p2", p2);
    BOOST_REQUIRE_EQUAL(CountingConstructor::count_, 2);  // instantiated once

    BOOST_REQUIRE_EQUAL(p1, p2);
    BOOST_CHECK_EQUAL(p1->i_, 4711);

    delete p1;
}

BOOST_AUTO_TEST_SUITE_END()
