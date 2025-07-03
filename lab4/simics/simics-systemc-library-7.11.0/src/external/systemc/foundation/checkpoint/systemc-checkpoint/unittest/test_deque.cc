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

#include <systemc-checkpoint/serialization/deque.h>

#include <unittest/polymorphism.h>
#include <unittest/sequence_test_environment.h>

#include <deque>

BOOST_AUTO_TEST_SUITE(TestDeque)

BOOST_AUTO_TEST_CASE(DequeInt) {
    saveLoadInt<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeTestUserType) {
    saveLoadUserType<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeNonDefaultConstructor) {
    saveLoadNonDefaultConstructor<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeVectorOfVectors) {
    saveLoadContainerOfContainers<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequePointers) {
    saveLoadPointers<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeResetObjectAddress) {
    saveLoadPointersResetObjectAddress<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeBaseObject) {
    saveLoadBaseObject<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequePolymorphism) {
    saveLoadPolymorphism<std::deque>();
}

BOOST_AUTO_TEST_CASE(DequeSize) {
    saveLoadSize<std::deque>();
}

BOOST_AUTO_TEST_SUITE_END()
