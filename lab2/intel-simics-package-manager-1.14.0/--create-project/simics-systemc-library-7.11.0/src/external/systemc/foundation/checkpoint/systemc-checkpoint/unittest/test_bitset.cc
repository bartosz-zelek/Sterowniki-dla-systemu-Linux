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

#include <systemc-checkpoint/serialization/bitset.h>
#include <unittest/stl_test_environment.h>

#include <bitset>
#include <string>

BOOST_AUTO_TEST_SUITE(TestBitset)

template <std::size_t N>
void saveLoadCheckEqual(const std::string expected) {
    boost::property_tree::ptree ptree;
    save(std::bitset<N>(expected), ptree);

    std::bitset<N> b;
    load(b, ptree);

    BOOST_CHECK_EQUAL(b.to_string(), expected);
}

BOOST_AUTO_TEST_CASE(Zero) {
    saveLoadCheckEqual<0>("");
}

BOOST_AUTO_TEST_CASE(One) {
    saveLoadCheckEqual<1>("1");
}

BOOST_AUTO_TEST_CASE(Eight) {
    saveLoadCheckEqual<8>("01010101");
}

BOOST_AUTO_TEST_CASE(ThirtyTwo) {
    saveLoadCheckEqual<32>(std::bitset<32>(0xFFFFFFFF).to_string());
}

BOOST_AUTO_TEST_SUITE_END()
