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

#include <systemc-checkpoint/serialization/vector.h>

#include <unittest/sequence_test_environment.h>

#include <vector>

BOOST_AUTO_TEST_SUITE(TestVector)

BOOST_AUTO_TEST_CASE(VectorInt) {
    saveLoadInt<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorTestUserType) {
    saveLoadUserType<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorNonDefaultConstructor) {
    saveLoadNonDefaultConstructor<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorVectorOfVectors) {
    saveLoadContainerOfContainers<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorPointers) {
    saveLoadPointers<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorResetObjectAddress) {
    saveLoadPointersResetObjectAddress<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorBaseObject) {
    saveLoadBaseObject<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorPolymorphism) {
    saveLoadPolymorphism<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorSize) {
    saveLoadSize<std::vector>();
}

BOOST_AUTO_TEST_CASE(VectorBool) {
    std::vector<bool> c;
    c.push_back(true);
    c.push_back(false);
    c.push_back(true);

    saveLoadCheckEqual(c);
}

BOOST_AUTO_TEST_CASE(VectorBoolSize) {
    std::vector<bool> c;
    c.push_back(false);
    c.push_back(false);
    c.push_back(false);

    boost::property_tree::ptree ptree;
    save(c, ptree);
    load(c, ptree);

    BOOST_CHECK_EQUAL(c.size(), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
