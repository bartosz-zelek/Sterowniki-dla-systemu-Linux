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
#include <boost/type_index.hpp>

#include <systemc-checkpoint/serialization/serializable_meta_data.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <string>

class A {};
class B : public A {};

namespace space {
struct C {};
};

using sc_checkpoint::serialization::SerializableMetaData;

BOOST_AUTO_TEST_SUITE(TestSerializableMetaData)

template <typename T>
void checkMetaDataName(std::string expected, T t) {
    SerializableMetaData<T> smd = SMD(t);
    BOOST_CHECK_EQUAL(expected, smd.type());
}

template <typename T>
void checkMetaDataNamePointer(std::string expected, T t) {
    checkMetaDataName(expected, t);
    delete t;
}

BOOST_AUTO_TEST_CASE(TypeInt) {
    checkMetaDataName<int>("int", 0);
    checkMetaDataNamePointer<int*>("int*", new int(0));
}

BOOST_AUTO_TEST_CASE(TypeDouble) {
    checkMetaDataName<double>("double", 0.0f);
    checkMetaDataNamePointer<double*>("double*", new double(0.0f));
}

BOOST_AUTO_TEST_CASE(TypeUnsigned) {
    checkMetaDataName<unsigned>("unsigned int", 0);
    checkMetaDataNamePointer<unsigned*>("unsigned int*", new unsigned(0));
}

BOOST_AUTO_TEST_CASE(TypeObject) {
    checkMetaDataName<A>("A", A());
    checkMetaDataNamePointer<A*>("A*", new A());
}

BOOST_AUTO_TEST_CASE(TypeDerivedObject) {
    checkMetaDataName<B>("B", B());
    checkMetaDataNamePointer<B*>("B*", new B());
}

BOOST_AUTO_TEST_CASE(TypeNamespace) {
    checkMetaDataName<space::C>("space::C", space::C());
    checkMetaDataNamePointer<space::C*>("space::C*", new space::C());
}

BOOST_AUTO_TEST_CASE(NameInt) {
    int a;
    SerializableMetaData<int> smd_int = SMD(a);
    BOOST_CHECK_EQUAL("a", smd_int.name());
}

BOOST_AUTO_TEST_CASE(NameUnsigned) {
    unsigned b_;
    SerializableMetaData<unsigned> smd_unsigned = SMD(b_);
    BOOST_CHECK_EQUAL("b_", smd_unsigned.name());
}

BOOST_AUTO_TEST_CASE(NameDouble) {
    double r2d2;
    SerializableMetaData<double> smd_double = SMD(r2d2);
    BOOST_CHECK_EQUAL("r2d2", smd_double.name());
}

template <typename T>
void testValue(T original_value, T updated_value) {
    T t = original_value;
    SerializableMetaData<T> smd = SMD(t);

    BOOST_CHECK_EQUAL(original_value, smd.const_value());
    smd.value() = updated_value;
    BOOST_CHECK_EQUAL(updated_value, smd.const_value());
}

BOOST_AUTO_TEST_CASE(ValueInt) {
    testValue<int>(0, -1);
}

BOOST_AUTO_TEST_CASE(ValueUnsigned) {
    testValue<unsigned>(0, 1);
}

BOOST_AUTO_TEST_CASE(ValueDouble) {
    testValue<double>(0.0, 1.0);
}

BOOST_AUTO_TEST_SUITE_END()
