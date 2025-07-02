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

#include <systemc-checkpoint/serialization/list.h>

#include <unittest/sequence_test_environment.h>

#include <list>

BOOST_AUTO_TEST_SUITE(TestList)

BOOST_AUTO_TEST_CASE(ListInt) {
    saveLoadInt<std::list>();
}

BOOST_AUTO_TEST_CASE(ListTestUserType) {
    saveLoadUserType<std::list>();
}

BOOST_AUTO_TEST_CASE(ListNonDefaultConstructor) {
    saveLoadNonDefaultConstructor<std::list>();
}

BOOST_AUTO_TEST_CASE(ListVectorOfVectors) {
    saveLoadContainerOfContainers<std::list>();
}

BOOST_AUTO_TEST_CASE(ListPointers) {
    saveLoadPointers<std::list>();
}

BOOST_AUTO_TEST_CASE(ListResetObjectAddress) {
    saveLoadPointersResetObjectAddress<std::list>();
}

BOOST_AUTO_TEST_CASE(ListBaseObject) {
    saveLoadBaseObject<std::list>();
}

BOOST_AUTO_TEST_CASE(ListPolymorphism) {
    saveLoadPolymorphism<std::list>();
}

BOOST_AUTO_TEST_CASE(ListSize) {
    saveLoadSize<std::list>();
}

BOOST_AUTO_TEST_SUITE_END()
