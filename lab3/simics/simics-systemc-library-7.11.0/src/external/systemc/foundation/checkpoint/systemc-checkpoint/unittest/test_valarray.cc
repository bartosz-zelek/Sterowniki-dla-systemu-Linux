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

#include <systemc-checkpoint/serialization/valarray.h>
#include <unittest/stl_test_environment.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestValArray)

template <class T>
void checkEqual(const T& got, const T& exp) {
    BOOST_CHECK_EQUAL(got, exp);
}

template <class T>
void checkEqual(const std::valarray<T> &got, const std::valarray<T> &exp) {
    BOOST_REQUIRE_EQUAL(got.size(), exp.size());
    for (std::size_t i = 0; i < got.size(); ++i) {
        checkEqual(got[i], exp[i]);
    }
}

template <class T>
void saveLoadCheckEqual(std::valarray<T> &got,  // NOLINT(runtime/references)
                        const std::valarray<T> &exp) {
    boost::property_tree::ptree ptree;
    save(exp, ptree);
    load(got, ptree);

    checkEqual(got, exp);
}

BOOST_AUTO_TEST_CASE(Empty) {
    std::valarray<int> exp(0);
    std::valarray<int> got(0);
    saveLoadCheckEqual(got, exp);
}

BOOST_AUTO_TEST_CASE(Int) {
    std::valarray<int> exp(3);
    exp[0] = 1;
    exp[1] = 2;
    exp[2] = 3;

    std::valarray<int> got(0, 3);
    saveLoadCheckEqual(got, exp);
}

BOOST_AUTO_TEST_CASE(TwoDimensional) {
    std::valarray<std::valarray<int> > exp(std::valarray<int>(3), 3);
    exp[0][0] = 1;
    exp[0][1] = 2;
    exp[0][2] = 3;

    exp[1][0] = 4;
    exp[1][1] = 5;
    exp[1][2] = 6;

    exp[2][0] = 7;
    exp[2][1] = 8;
    exp[2][2] = 9;

    std::valarray<std::valarray<int> > got(std::valarray<int>(3), 3);
    saveLoadCheckEqual(got, exp);
}

BOOST_AUTO_TEST_CASE(Size) {
    std::valarray<int> v(3);
    v[0] = 0;
    v[1] = 0;
    v[2] = 0;

    boost::property_tree::ptree ptree;
    save(v, ptree);
    load(v, ptree);

    BOOST_REQUIRE_EQUAL(v.size(), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
