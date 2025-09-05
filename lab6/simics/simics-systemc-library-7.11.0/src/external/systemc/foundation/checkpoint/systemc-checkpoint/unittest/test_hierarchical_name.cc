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

#include <systemc-checkpoint/hierarchical_name.h>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestHierarchicalName)

BOOST_AUTO_TEST_CASE(TestPush) {
    sc_checkpoint::HierarchicalName name("prefix", ".");
    name.push("a");
    name.push("b");
    name.push("c");

    BOOST_CHECK_EQUAL("prefix.a.b.c", name.name());
}

BOOST_AUTO_TEST_CASE(TestPop) {
    sc_checkpoint::HierarchicalName name("prefix", ".");
    name.push("a");
    name.push("b");
    name.push("c");

    name.pop();
    BOOST_CHECK_EQUAL("prefix.a.b", name.name());
    name.pop();
    BOOST_CHECK_EQUAL("prefix.a", name.name());
    name.pop();
    BOOST_CHECK_EQUAL("prefix", name.name());
}

BOOST_AUTO_TEST_CASE(TestPopEmpty) {
    sc_checkpoint::HierarchicalName name("prefix", ".");
    name.pop();
    BOOST_CHECK_EQUAL("prefix", name.name());
}

BOOST_AUTO_TEST_CASE(TestSuffix) {
    sc_checkpoint::HierarchicalName name("prefix", ".");
    BOOST_CHECK_EQUAL("prefix.suffix", name.concatenate("suffix"));
}

BOOST_AUTO_TEST_SUITE_END()
