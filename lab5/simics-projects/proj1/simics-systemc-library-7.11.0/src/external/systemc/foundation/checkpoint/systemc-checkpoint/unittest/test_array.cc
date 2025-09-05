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

#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/array.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <unittest/polymorphism.h>
#include <unittest/stl_test_environment.h>

#include <boost/property_tree/ptree.hpp>
#include <cci/serialization/export.hpp>
#include <boost/test/unit_test.hpp>

// These types should only be exported once to avoid conflicting definitions
CCI_CLASS_EXPORT(DerivedA)
CCI_CLASS_EXPORT(DerivedB)

BOOST_AUTO_TEST_SUITE(TestArray)

template <class T>
boost::property_tree::ptree save_array(const T &t, size_t size) {
    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd(
        "array", sc_checkpoint::serialization::make_array(t, size));
    return ptree;
}

template <class T>
void load_array(T &t, size_t size, boost::property_tree::ptree ptree) {  // NOLINT
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd(
        "array", sc_checkpoint::serialization::make_array(t, size));
}

BOOST_AUTO_TEST_CASE(StaticArray) {
    int a[3] = {1, 2, 3};
    boost::property_tree::ptree ptree = save_array(a, 3);

    int b[3] = {0, 0, 0};
    load_array(b, 3, ptree);

    BOOST_CHECK_EQUAL(a[0], b[0]);
    BOOST_CHECK_EQUAL(a[1], b[1]);
    BOOST_CHECK_EQUAL(a[2], b[2]);
}

BOOST_AUTO_TEST_CASE(TwoDimensionalArray) {
    int a[2][2] = {1, 2, 3, 4};
    boost::property_tree::ptree ptree = save_array(a, 2);

    int b[2][2] = {0, 0, 0, 0};
    load_array(b, 2, ptree);

    BOOST_CHECK_EQUAL(a[0][0], b[0][0]);
    BOOST_CHECK_EQUAL(a[0][1], b[0][1]);
    BOOST_CHECK_EQUAL(a[1][0], b[1][0]);
    BOOST_CHECK_EQUAL(a[1][1], b[1][1]);
}

BOOST_AUTO_TEST_CASE(ThreeDimensionalArray) {
    int a[1][1][1] = {1};
    boost::property_tree::ptree ptree = save_array(a, 1);

    int b[1][1][1] = {0};
    load_array(b, 1, ptree);

    BOOST_CHECK_EQUAL(a[0][0][0], b[0][0][0]);
}

BOOST_AUTO_TEST_CASE(ImplicitArray) {
    int a[3] = {1, 2, 3};
    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd("array", a);

    int b[3] = {0, 0, 0};
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd("array", b);

    BOOST_CHECK_EQUAL(a[0], b[0]);
    BOOST_CHECK_EQUAL(a[1], b[1]);
    BOOST_CHECK_EQUAL(a[2], b[2]);
}

BOOST_AUTO_TEST_CASE(ImplicitTwoDimensionalArray) {
    int a[2][2] = {1, 2, 3, 4};
    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd("array", a);

    int b[2][2] = {0, 0, 0, 0};
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd("array", b);

    BOOST_CHECK_EQUAL(a[0][0], b[0][0]);
    BOOST_CHECK_EQUAL(a[0][1], b[0][1]);
    BOOST_CHECK_EQUAL(a[1][0], b[1][0]);
    BOOST_CHECK_EQUAL(a[1][1], b[1][1]);
}

BOOST_AUTO_TEST_CASE(ImplicitArrayMacro) {
    boost::property_tree::ptree ptree;
    {
        int a[3] = {1, 2, 3};
        sc_checkpoint::OStateTree otree(&ptree);
        sc_checkpoint::OArchive oarchive(&otree);
        oarchive << SMD(a);
    }

    int a[3] = {0, 0, 0};
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> SMD(a);

    BOOST_CHECK_EQUAL(1, a[0]);
    BOOST_CHECK_EQUAL(2, a[1]);
    BOOST_CHECK_EQUAL(3, a[2]);
}

BOOST_AUTO_TEST_CASE(ImplicitTwoDimensionalArrayMacro) {
    boost::property_tree::ptree ptree;
    {
        int a[2][2] = {1, 2, 3, 4};
        sc_checkpoint::OStateTree otree(&ptree);
        sc_checkpoint::OArchive oarchive(&otree);
        oarchive << SMD(a);
    }

    int a[2][2] = {0, 0, 0, 0};
    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> SMD(a);

    BOOST_CHECK_EQUAL(1, a[0][0]);
    BOOST_CHECK_EQUAL(2, a[0][1]);
    BOOST_CHECK_EQUAL(3, a[1][0]);
    BOOST_CHECK_EQUAL(4, a[1][1]);
}

// A dynamic array is fine, but two-dimensional arrays (or more) are not
// supported, since we cannot distinguish between an array and a regular
// pointer
BOOST_AUTO_TEST_CASE(DynamicArray) {
    int *a = new int[3];
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;

    boost::property_tree::ptree ptree = save_array(a, 3);

    int *b = new int[3]();
    load_array(b, 3, ptree);

    BOOST_CHECK_EQUAL(a[0], b[0]);
    BOOST_CHECK_EQUAL(a[1], b[1]);
    BOOST_CHECK_EQUAL(a[2], b[2]);
}

BOOST_AUTO_TEST_CASE(StaticArrayUserType) {
    UserType a[3] = {
        UserType(1),
        UserType(2),
        UserType(3)
    };
    boost::property_tree::ptree ptree = save_array(a, 3);

    UserType b[3];
    load_array(b, 3, ptree);

    BOOST_CHECK_EQUAL(a[0], b[0]);
    BOOST_CHECK_EQUAL(a[1], b[1]);
    BOOST_CHECK_EQUAL(a[2], b[2]);
}

BOOST_AUTO_TEST_CASE(StaticArrayPointers) {
    UserType *a[3] = {
        new UserType(1),
        new UserType(2),
        new UserType(3)
    };
    boost::property_tree::ptree ptree = save_array(a, 3);

    UserType *b[3] = {NULL, NULL, NULL};
    load_array(b, 3, ptree);

    BOOST_CHECK_EQUAL(*a[0], *b[0]);
    BOOST_CHECK_EQUAL(*a[1], *b[1]);
    BOOST_CHECK_EQUAL(*a[2], *b[2]);
}

BOOST_AUTO_TEST_CASE(StaticArrayBaseObject) {
    DerivedA a[3] = {
        DerivedA(1, -1),
        DerivedA(2, -2),
        DerivedA(3, -3)
    };
    boost::property_tree::ptree ptree = save_array(a, 3);

    DerivedA b[3];
    load_array(b, 3, ptree);

    BOOST_CHECK_EQUAL(a[0], b[0]);
    BOOST_CHECK_EQUAL(a[1], b[1]);
    BOOST_CHECK_EQUAL(a[2], b[2]);
}

BOOST_AUTO_TEST_CASE(StaticArrayPolymorphism) {
    Base *a[3] = {
        new DerivedA(1, 2),
        new DerivedB(3, 4, 5),
        new DerivedA(6, 7)
    };
    boost::property_tree::ptree ptree = save_array(a, 3);

    Base *b[3] = {NULL, NULL, NULL};
    load_array(b, 3, ptree);

    BOOST_REQUIRE(a[0] != b[0]);
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA *>(a[0]),
                      *dynamic_cast<DerivedA *>(b[0]));
    BOOST_REQUIRE(a[1] != b[1]);
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedB *>(a[1]),
                      *dynamic_cast<DerivedB *>(b[1]));
    BOOST_REQUIRE(a[2] != b[2]);
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA *>(a[2]),
                      *dynamic_cast<DerivedA *>(b[2]));
}

// Boost serialization libraries don't seem to handle the serialization of
// arrays containing elements with non-default constructors
// BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES(StaticArrayNonDefaultConstructor, 3)
// BOOST_AUTO_TEST_CASE(StaticArrayNonDefaultConstructor) {
//     UserTypeNonDefaultConstructor a[3] = {
//         UserTypeNonDefaultConstructor(1),
//         UserTypeNonDefaultConstructor(2),
//         UserTypeNonDefaultConstructor(3)
//     };
//     boost::property_tree::ptree ptree = save_array(a, 3);

//     UserTypeNonDefaultConstructor b[3] = {
//         UserTypeNonDefaultConstructor(0),
//         UserTypeNonDefaultConstructor(0),
//         UserTypeNonDefaultConstructor(0)
//     };
//     load_array(b, 3, ptree);

//     BOOST_CHECK_EQUAL(a[0], b[0]);
//     BOOST_CHECK_EQUAL(a[1], b[1]);
//     BOOST_CHECK_EQUAL(a[2], b[2]);
// }

BOOST_AUTO_TEST_SUITE_END()
