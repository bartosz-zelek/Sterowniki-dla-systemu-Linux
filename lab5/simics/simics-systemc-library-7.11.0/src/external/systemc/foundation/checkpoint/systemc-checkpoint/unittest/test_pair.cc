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
#include <cci/serialization/serialization.hpp>
#include <cci/serialization/string.hpp>

#include <systemc-checkpoint/serialization/pair.h>

#include <unittest/stl_test_environment.h>

#include <string>
#include <utility>

BOOST_AUTO_TEST_SUITE(TestPair)

template <class First, class Second>
void saveAndLoadPair(const std::pair<First, Second> &expected) {
    boost::property_tree::ptree ptree;
    save(expected, ptree);

    std::pair<First, Second> got;
    load(got, ptree);

    BOOST_CHECK_EQUAL(got.first, expected.first);
    BOOST_CHECK_EQUAL(got.second, expected.second);
}

BOOST_AUTO_TEST_CASE(SaveAndExpectPairInt) {
    saveAndLoadPair(std::pair<int, int>(-1, 1));
}

BOOST_AUTO_TEST_CASE(SaveAndExpectPairString) {
    saveAndLoadPair(std::pair<std::string, int>("Hello", 1));
}

BOOST_AUTO_TEST_CASE(SaveAndExpectPairOfPairs) {
    typedef std::pair<int, int> P1;
    typedef std::pair<int, P1> P2;
    typedef std::pair<P2, P1> P3;

    P1 p1(2, 3);
    P2 p2(1, p1);
    P3 p3(p2, p1);

    boost::property_tree::ptree ptree;
    save(p3, ptree);

    P3 got;
    load(got, ptree);

    BOOST_CHECK(got.first == p3.first);
    BOOST_CHECK(got.second == p3.second);
}

BOOST_AUTO_TEST_SUITE_END()
