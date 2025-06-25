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

#include <systemc-checkpoint/serialization/map.h>
#include <unittest/map_test_environment.h>

#include <map>

BOOST_AUTO_TEST_SUITE(TestMap)

BOOST_AUTO_TEST_CASE(MapInt) {
    saveLoadInt<std::map>();
}

BOOST_AUTO_TEST_CASE(MapTestUserType) {
    saveLoadUserType<std::map>();
}

BOOST_AUTO_TEST_CASE(MapOfMaps) {
    saveLoadContainerOfContainers<std::map>();
}

BOOST_AUTO_TEST_CASE(MapPointers) {
    saveLoadPointers<std::map>();
}

BOOST_AUTO_TEST_CASE(MapResetObjectAddress) {
    saveLoadResetObjectAddress<std::map>();
}

BOOST_AUTO_TEST_CASE(MapBaseObject) {
    saveLoadBaseObject<std::map>();
}

BOOST_AUTO_TEST_CASE(MapPolymorphism) {
    saveLoadPolymorphism<std::map>();
}

BOOST_AUTO_TEST_CASE(MapSize) {
    saveLoadSize<std::map>();
}

BOOST_AUTO_TEST_CASE(MapOrder) {
    saveLoadOrder<std::map>();
}

// TODO(enilsson): It appears that this particular use-case is unsupported in
// 1.58 and earlier because of a bug in the construction of pairs
// (https://svn.boost.org/trac/boost/ticket/11343). Because we, in part,
// provide our own implementation of containers we might be able to resolve
// this on our own, but since it's unsupported in the Boost version that we're
// using we should be able to postpone it.
// BOOST_AUTO_TEST_CASE(MapNonDefaultConstructor) {
//     std::map<int, UserTypeNonDefaultConstructor> m;

//     m[1] = UserTypeNonDefaultConstructor(-1);
//     m[2] = UserTypeNonDefaultConstructor(-2);
//     m[3] = UserTypeNonDefaultConstructor(-3);

//     saveAndLoadMap(m);
// }

BOOST_AUTO_TEST_SUITE_END()
