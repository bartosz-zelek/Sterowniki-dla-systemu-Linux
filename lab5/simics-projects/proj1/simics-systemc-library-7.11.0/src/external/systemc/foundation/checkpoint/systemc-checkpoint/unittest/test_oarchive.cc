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

#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/archive_test_environment.h>

#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cci/serialization/serialization.hpp>
#include <cci/serialization/string.hpp>

#include <string>
#include <sstream>
#include <iostream>

void compare_ptrees(
    boost::property_tree::ptree expected, boost::property_tree::ptree got) {
    bool equal = expected == got;
    BOOST_CHECK(equal);
    if (!equal) {
        std::ostringstream ss_expected;
        std::ostringstream ss_got;

        boost::property_tree::write_json(ss_expected, expected);
        boost::property_tree::write_json(ss_got, got);

        std::cout << "Property trees do not match! Expected:" << std::endl
                  << ss_expected.str() << std::endl << "Got:" << std::endl
                  << ss_got.str() << std::endl;
    }
}

template <typename T>
void saveAndExpectState(const T expected) {
    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree oTree(&ptree);
    sc_checkpoint::OArchive oArchive(&oTree);

    oArchive << SMD(expected);
    compare_ptrees(expected.expectedState("expected"), ptree);
}

BOOST_AUTO_TEST_SUITE(TestOArchive)

BOOST_AUTO_TEST_CASE(SaveAndExpectStateInteger) {
    saveAndExpectState(SerializableWithSingleMember<int>(42));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectStateDouble) {
    saveAndExpectState(SerializableWithSingleMember<double>(0.05));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectStateBool) {
    saveAndExpectState(SerializableWithSingleMember<bool>(false));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectStateString) {
    saveAndExpectState(SerializableWithSingleMember<std::string>("Hello"));
}

BOOST_AUTO_TEST_CASE(SaveAndExpectIntegerPointer) {
    saveAndExpectState(SerializableWithPointers<int>(42));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectDoublePointer) {
    saveAndExpectState(SerializableWithPointers<double>(0.05));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectBoolPointer) {
    saveAndExpectState(SerializableWithPointers<bool>(false));
}
BOOST_AUTO_TEST_CASE(SaveAndExpectStringPointer) {
    saveAndExpectState(SerializableWithPointers<std::string>("Hello"));
}

BOOST_AUTO_TEST_CASE(SaveAndExpectBaseClass) {
    saveAndExpectState(SerializableDerivedClass("Hello", "World"));
}

BOOST_AUTO_TEST_CASE(SaveAndExpectNonDefaultConstructorPointer) {
    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree oTree(&ptree);
    sc_checkpoint::OArchive oArchive(&oTree);

    SerializableWithNonDefaultConstructor *p
        = new SerializableWithNonDefaultConstructor(1);
    oArchive << SMD(p);

    compare_ptrees(p->expectedState("p"), ptree);
    delete p;
}

BOOST_AUTO_TEST_SUITE_END()
