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

#include <systemc-checkpoint/serialization/stack.h>
#include <unittest/adapter_test_environment.h>

#include <deque>
#include <list>
#include <stack>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestStack)

template <class T, class Container>
void popCheckEqual(std::stack<T, Container> *s, T exp) {
    BOOST_CHECK_EQUAL(s->top(), exp);
    s->pop();
}

// The underlying containers are extensively tested, so it's sufficient to
// perform only some smoke testing to make sure that all container types are
// supported
BOOST_AUTO_TEST_CASE(AdaptingDeque) {
    std::stack<int, std::deque<int> > s;
    s.push(1);
    s.push(2);
    s.push(3);

    saveLoadCheckEqual(s);

    popCheckEqual(&s, 3);
    popCheckEqual(&s, 2);
    popCheckEqual(&s, 1);
}

BOOST_AUTO_TEST_CASE(AdaptingList) {
    std::stack<int, std::list<int> > s;
    s.push(1);
    s.push(2);
    s.push(3);

    saveLoadCheckEqual(s);

    popCheckEqual(&s, 3);
    popCheckEqual(&s, 2);
    popCheckEqual(&s, 1);
}

BOOST_AUTO_TEST_CASE(AdaptingVector) {
    std::stack<int, std::vector<int> > s;
    s.push(1);
    s.push(2);
    s.push(3);

    saveLoadCheckEqual(s);

    popCheckEqual(&s, 3);
    popCheckEqual(&s, 2);
    popCheckEqual(&s, 1);
}

BOOST_AUTO_TEST_SUITE_END()
