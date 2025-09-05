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

#include <systemc>

#include <systemc-checkpoint/all_sc_objects.h>

#include <unittest/simulation.h>

#include <string>

BOOST_AUTO_TEST_SUITE(TestAllScObjects)

SC_MODULE(TestModuleBottom) {
    SC_CTOR(TestModuleBottom) {}
};

SC_MODULE(TestModuleMiddle) {
    SC_CTOR(TestModuleMiddle) : bottom_("TestModuleBottom") {}
    TestModuleBottom bottom_;
};

SC_MODULE(TestModuleTop) {
    SC_CTOR(TestModuleTop) : middle_("TestModuleMiddle") {}
    TestModuleMiddle middle_;
};

BOOST_AUTO_TEST_CASE(TestAllObjectsExtracted) {
    unittest::Simulation simulation_;

    TestModuleTop module("TestModuleTop");
    sc_checkpoint::AllScObjects objects;
    BOOST_REQUIRE_EQUAL(objects.size(), static_cast<std::size_t>(3));
    BOOST_CHECK_EQUAL(objects[0]->basename(), std::string("TestModuleTop"));
    BOOST_CHECK_EQUAL(objects[1]->basename(), std::string("TestModuleMiddle"));
    BOOST_CHECK_EQUAL(objects[2]->basename(), std::string("TestModuleBottom"));
}

bool filter(const sc_core::sc_object *object) {
    if (std::string("TestModuleMiddle") == object ->basename())
        return false;

    return true;
}

BOOST_AUTO_TEST_CASE(TestAllObjectsFiltered) {
    unittest::Simulation simulation_;

    TestModuleTop module("TestModuleTop");
    sc_checkpoint::AllScObjects objects(filter);
    BOOST_REQUIRE_EQUAL(objects.size(), static_cast<std::size_t>(2));
    BOOST_CHECK_EQUAL(objects[0]->basename(), std::string("TestModuleTop"));
    BOOST_CHECK_EQUAL(objects[1]->basename(), std::string("TestModuleBottom"));
}

BOOST_AUTO_TEST_SUITE_END()
