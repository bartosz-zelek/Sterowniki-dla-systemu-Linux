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

#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>

#include <boost/test/unit_test.hpp>
#include <cci/serialization/serialization.hpp>
#include <cci/serialization/base_object.hpp>

#include <unittest/polymorphism.h>

BOOST_AUTO_TEST_SUITE(TestExport)

BOOST_AUTO_TEST_CASE(ExportDerivedA) {
    Base *a = new DerivedA(1, -1);

    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd("poly", a);

    Base *b = NULL;
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd("poly", b);

    BOOST_REQUIRE(dynamic_cast<DerivedA*>(a));
    BOOST_REQUIRE(dynamic_cast<DerivedA*>(b));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA*>(b), *dynamic_cast<DerivedA*>(a));
}

BOOST_AUTO_TEST_CASE(ExportDerivedAB) {
    Base *a = new DerivedA(1, 2);
    Base *b = new DerivedB(3, 4, 5);

    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd("a", a)
             << sc_checkpoint::serialization::create_smd("b", b);

    Base *aa = NULL;
    Base *bb = NULL;
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd("a", aa)
             >> sc_checkpoint::serialization::create_smd("b", bb);

    BOOST_REQUIRE(dynamic_cast<DerivedA*>(aa));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA*>(a),
                      *dynamic_cast<DerivedA*>(aa));

    BOOST_REQUIRE(dynamic_cast<DerivedB*>(bb));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedB*>(b),
                      *dynamic_cast<DerivedB*>(bb));
}

BOOST_AUTO_TEST_SUITE_END()
