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
#include <unittest/adapter_test_environment.h>

#include <deque>
#include <list>
#include <queue>

BOOST_AUTO_TEST_SUITE(TestQueue)

template <class T, class Container>
void popCheckEqual(std::queue<T, Container> *q, T exp) {
    BOOST_CHECK_EQUAL(q->front(), exp);
    q->pop();
}

// The underlying containers are extensively tested, so it's sufficient to
// perform only some smoke testing to make sure that all container types are
// supported
BOOST_AUTO_TEST_CASE(AdaptingDeque) {
    std::queue<int, std::deque<int> > q;
    q.push(1);
    q.push(2);
    q.push(3);

    saveLoadCheckEqual(q);

    popCheckEqual(&q, 1);
    popCheckEqual(&q, 2);
    popCheckEqual(&q, 3);
}

BOOST_AUTO_TEST_CASE(AdaptingList) {
    std::queue<int, std::list<int> > q;
    q.push(1);
    q.push(2);
    q.push(3);

    saveLoadCheckEqual(q);

    popCheckEqual(&q, 1);
    popCheckEqual(&q, 2);
    popCheckEqual(&q, 3);
}

BOOST_AUTO_TEST_SUITE_END()
