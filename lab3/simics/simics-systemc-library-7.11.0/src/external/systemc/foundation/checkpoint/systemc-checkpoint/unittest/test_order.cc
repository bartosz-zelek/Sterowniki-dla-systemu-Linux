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
#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/smd.h>

BOOST_AUTO_TEST_SUITE(TestOrder)

BOOST_AUTO_TEST_CASE(OrderIndependence) {
    boost::property_tree::ptree ptree;

    {
        int a = 1;
        int b = 2;
        int c = 3;

        sc_checkpoint::OStateTree otree(&ptree);
        sc_checkpoint::OArchive oarchive(&otree);
        oarchive << SMD(a)
                 << SMD(b)
                 << SMD(c);
    }

    int a = 0;
    int b = 0;
    int c = 0;

    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> SMD(c)
             >> SMD(b)
             >> SMD(a);

    BOOST_CHECK_EQUAL(a, 1);
    BOOST_CHECK_EQUAL(b, 2);
    BOOST_CHECK_EQUAL(c, 3);
}

BOOST_AUTO_TEST_SUITE_END()
