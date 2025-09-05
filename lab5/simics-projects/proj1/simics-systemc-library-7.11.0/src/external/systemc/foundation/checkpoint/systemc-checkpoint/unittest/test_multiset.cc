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

#include <systemc-checkpoint/serialization/set.h>
#include <unittest/set_test_environment.h>

#include <set>

BOOST_AUTO_TEST_SUITE(TestMultiSet)

BOOST_AUTO_TEST_CASE(Int) {
    saveLoadInt<std::multiset>();
}

BOOST_AUTO_TEST_CASE(UserType) {
    saveLoadUserType<std::multiset>();
}

BOOST_AUTO_TEST_CASE(SetOfSets) {
    saveLoadContainerOfContainers<std::multiset>();
}

BOOST_AUTO_TEST_CASE(Pointers) {
    saveLoadPointers<std::multiset>();
}

BOOST_AUTO_TEST_CASE(ResetAddress) {
    saveLoadResetObjectAddress<std::multiset>();
}

BOOST_AUTO_TEST_CASE(BaseObject) {
    saveLoadBaseObject<std::multiset>();
}

BOOST_AUTO_TEST_CASE(Polymorphism) {
    saveLoadPolymorphism<std::multiset>();
}

BOOST_AUTO_TEST_CASE(Size) {
    saveLoadSize<std::multiset>();
}

BOOST_AUTO_TEST_CASE(Order) {
    saveLoadOrder<std::multiset>();
}

BOOST_AUTO_TEST_CASE(MultipleKeys) {
    std::multiset<int> expected;
    expected.insert(1);
    expected.insert(2);
    expected.insert(2);

    boost::property_tree::ptree ptree;
    save(expected, ptree);

    std::multiset<int> got;
    load(got, ptree);
    BOOST_REQUIRE_EQUAL(got.size(), expected.size());

    std::multiset<int>::const_iterator range = got.equal_range(2).first;
    BOOST_CHECK_EQUAL(*(range++), 2);
    BOOST_CHECK_EQUAL(*(range++), 2);
    BOOST_CHECK(range == got.end());
}


BOOST_AUTO_TEST_SUITE_END()
