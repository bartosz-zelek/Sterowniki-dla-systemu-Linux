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

#include <unittest/map_test_environment.h>

#include <map>

BOOST_AUTO_TEST_SUITE(TestMultiMap)

BOOST_AUTO_TEST_CASE(MultiMapInt) {
    saveLoadInt<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapTestUserType) {
    saveLoadUserType<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapOfMultiMaps) {
    saveLoadContainerOfContainers<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapPointers) {
    saveLoadPointers<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapResetObjectAddress) {
    saveLoadResetObjectAddress<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapBaseObject) {
    saveLoadBaseObject<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapPolymorphism) {
    saveLoadPolymorphism<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapSize) {
    saveLoadSize<std::multimap>();
}

BOOST_AUTO_TEST_CASE(MultiMapOrder) {
    saveLoadOrder<std::multimap>();
}

// TODO(enilsson): as far as I can tell, the order of entries with equivalent
// keys is not guaranteed prior to C++ 11, in which the order is guaranteed to
// be the order of insertion.
// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(MultiMapEquivalentKeys, 2)
// BOOST_AUTO_TEST_CASE(MultiMapEquivalentKeys) {
//     std::multimap<int, int> expected;
//     expected.insert(std::make_pair(0, 1));
//     expected.insert(std::make_pair(0, 2));
//     expected.insert(std::make_pair(0, 3));

//     boost::property_tree::ptree ptree;
//     save(expected, ptree);

//     std::multimap<int, int> got;
//     load(got, ptree);
//     BOOST_REQUIRE(got.size() == expected.size());

//     std::multimap<int, int>::iterator range = got.equal_range(0).first;
//     BOOST_CHECK_EQUAL(range++->second, 1);
//     BOOST_CHECK_EQUAL(range++->second, 2);
//     BOOST_CHECK_EQUAL(range++->second, 3);
// }

BOOST_AUTO_TEST_SUITE_END()
