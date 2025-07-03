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

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/archive_test_environment.h>

#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cci/serialization/serialization.hpp>

#include <string>

template <typename T>
void loadAndCompare(const T expected) {
    boost::property_tree::ptree ptree = expected.expectedState("loaded");
    sc_checkpoint::IStateTree iTree(&ptree);
    sc_checkpoint::IArchive iArchive(&iTree);

    T loaded;
    iArchive >> SMD(loaded);

    expected.checkEqual(loaded);
}

BOOST_AUTO_TEST_SUITE(TestIArchive)

BOOST_AUTO_TEST_CASE(LoadAndCompareInteger) {
    loadAndCompare(SerializableWithSingleMember<int>(42));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareDouble) {
    loadAndCompare(SerializableWithSingleMember<double>(0.05));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareBool) {
    loadAndCompare(SerializableWithSingleMember<bool>(false));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareString) {
    loadAndCompare(SerializableWithSingleMember<std::string>("Hello"));
}

BOOST_AUTO_TEST_CASE(LoadAndCompareIntegerPointer) {
    loadAndCompare(SerializableWithPointers<int>(42));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareDoublePointer) {
    loadAndCompare(SerializableWithPointers<double>(0.05));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareBoolPointer) {
    loadAndCompare(SerializableWithPointers<bool>(false));
}
BOOST_AUTO_TEST_CASE(LoadAndCompareStringPointer) {
    loadAndCompare(SerializableWithPointers<std::string>("Hello"));
}

BOOST_AUTO_TEST_CASE(LoadAndCompareBaseClass) {
    loadAndCompare(SerializableDerivedClass("Hello", "World"));
}

BOOST_AUTO_TEST_CASE(LoadAndCompareNonDefaultConstructorPointer) {
    SerializableWithNonDefaultConstructor *t
        = new SerializableWithNonDefaultConstructor(1);

    boost::property_tree::ptree ptree = t->expectedState("t");
    sc_checkpoint::IStateTree iTree(&ptree);
    sc_checkpoint::IArchive iArchive(&iTree);

    delete t;
    iArchive >> SMD(t);

    BOOST_CHECK_EQUAL(t->number(), 1);
    delete t;
}

BOOST_AUTO_TEST_SUITE_END()
