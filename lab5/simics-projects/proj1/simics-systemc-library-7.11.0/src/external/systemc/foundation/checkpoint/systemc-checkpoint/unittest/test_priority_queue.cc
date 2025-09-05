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

#include <systemc-checkpoint/serialization/queue.h>
#include <unittest/stl_test_environment.h>

#include <deque>
#include <queue>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestPriorityQueue)

template <class T, class Container>
void popCheckEqual(std::priority_queue<T, Container> *q, T exp) {
    BOOST_CHECK_EQUAL(q->top(), exp);
    q->pop();
}

template <class T, class Container, class Cmp>
void checkEqual(std::priority_queue<T, Container, Cmp> got,
                std::priority_queue<T, Container, Cmp> exp) {
    typedef sc_checkpoint::ExposeContainer<
        std::priority_queue<T, Container, Cmp> > ExposeContainer;
    Container gotC = static_cast<ExposeContainer &>(got).container();
    Container expC = static_cast<ExposeContainer &>(exp).container();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        expC.begin(), expC.end(), gotC.begin(), gotC.end());
}

template <class T, class Container, class Cmp>
void saveLoadCheckEqual(const std::priority_queue<T, Container, Cmp> &exp) {
    boost::property_tree::ptree ptree;
    save(exp, ptree);

    std::priority_queue<T, Container, Cmp> got;
    load(got, ptree);

    checkEqual(got, exp);
}

// The underlying containers are extensively tested, so it's sufficient to
// perform only some smoke testing to make sure that all container types are
// supported
BOOST_AUTO_TEST_CASE(AdaptingDeque) {
    std::priority_queue<int, std::deque<int> > q;
    q.push(1);
    q.push(3);
    q.push(2);

    saveLoadCheckEqual(q);

    popCheckEqual(&q, 3);
    popCheckEqual(&q, 2);
    popCheckEqual(&q, 1);
}

BOOST_AUTO_TEST_CASE(AdaptingVector) {
    std::priority_queue<int, std::vector<int> > q;
    q.push(1);
    q.push(3);
    q.push(2);

    saveLoadCheckEqual(q);

    popCheckEqual(&q, 3);
    popCheckEqual(&q, 2);
    popCheckEqual(&q, 1);
}

BOOST_AUTO_TEST_SUITE_END()
