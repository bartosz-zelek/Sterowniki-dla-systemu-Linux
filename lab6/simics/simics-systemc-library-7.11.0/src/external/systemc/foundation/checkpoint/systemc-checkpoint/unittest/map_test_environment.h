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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_MAP_TEST_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_MAP_TEST_ENVIRONMENT_H

#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/serialization/map.h>
#include <unittest/polymorphism.h>
#include <unittest/stl_test_environment.h>

#include <functional>
#include <memory>
#include <utility>

template <template <class, class, class, class> class Container,
          class First, class Second, class Compare, class Allocator>
void checkEqual(Container<First, Second, Compare, Allocator> got,
                Container<First, Second, Compare, Allocator> exp) {
    BOOST_REQUIRE_EQUAL(got.size(), exp.size());

    typedef typename Container<First, Second, Compare, Allocator>::iterator It;
    for (It expIt = exp.begin(); expIt != exp.end(); expIt++) {
        It gotIt = got.find(expIt->first);
        BOOST_REQUIRE(gotIt != got.end());
        BOOST_CHECK_EQUAL(gotIt->second, expIt->second);
    }
}

template <template <class, class, class, class> class Container,
          class First, class Second, class Compare, class Allocator>
void saveLoadCheckEqual(
    const Container<First, Second, Compare, Allocator> &exp) {
    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Container<First, Second, Compare, Allocator> got;
    load(got, ptree);

    checkEqual(got, exp);
}

template <template <class = int, class = int, class = std::less<int>,
                    class = std::allocator<std::pair<const int, int> > >
          class Container>
void saveLoadInt() {
    Container<> c;
    c.insert(std::make_pair(1, -1));
    c.insert(std::make_pair(2, -2));
    c.insert(std::make_pair(3, -3));

    saveLoadCheckEqual(c);
}

template <template <class = UserType, class = int, class = UserComparison,
                    class = std::allocator<std::pair<const UserType, int> > >
          class Container>
void saveLoadUserType() {
    Container<> c;
    c.insert(std::make_pair(UserType(1), -1));
    c.insert(std::make_pair(UserType(2), -2));
    c.insert(std::make_pair(UserType(3), -3));

    saveLoadCheckEqual(c);
}

template <template <class, class, class, class> class Container>
void saveLoadContainerOfContainers() {
    typedef Container<int, int, std::less<int>,
                      std::allocator<std::pair<const int, int> > > ContainerInt;
    typedef Container<int, ContainerInt, std::less<int>,
                      std::allocator<
                          std::pair<const int, ContainerInt> > > ContainerSq;
    ContainerSq exp;

    // Inserting with iterators because insertion return values differ on map
    // type for other insertion methods
    typename ContainerSq::iterator it0 = exp.insert(
        exp.begin(), std::make_pair(0, ContainerInt()));
    it0->second.insert(std::make_pair(0, 1));
    it0->second.insert(std::make_pair(1, 2));
    it0->second.insert(std::make_pair(2, 3));

    typename ContainerSq::iterator it1 = exp.insert(
        it0, std::make_pair(1, ContainerInt()));
    it1->second.insert(std::make_pair(0, 4));
    it1->second.insert(std::make_pair(1, 5));
    it1->second.insert(std::make_pair(2, 6));

    typename ContainerSq::iterator it2 = exp.insert(
        it1, std::make_pair(2, ContainerInt()));
    it2->second.insert(std::make_pair(0, 7));
    it2->second.insert(std::make_pair(1, 8));
    it2->second.insert(std::make_pair(2, 9));

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    ContainerSq got;
    load(got, ptree);

    checkEqual(got.find(0)->second, it0->second);
    checkEqual(got.find(1)->second, it1->second);
    checkEqual(got.find(2)->second, it2->second);
}

template <template <class = int, class = UserType *, class = std::less<int>,
                    class = std::allocator<std::pair<const int, UserType *> > >
          class Container>
void saveLoadPointers() {
    Container<> exp;

    UserType *exp0 = exp.insert(
        exp.begin(), std::make_pair(0, new UserType(0)))->second;
    UserType *exp1 = exp.insert(
        exp.begin(), std::make_pair(1, new UserType(-1)))->second;
    UserType *exp2 = exp.insert(
        exp.begin(), std::make_pair(2, new UserType(-2)))->second;

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Container<> got;
    load(got, ptree);

    UserType *got0 = got.find(0)->second;
    BOOST_REQUIRE(got0 != exp0);
    BOOST_CHECK_EQUAL(*got0, *exp0);

    UserType *got1 = got.find(1)->second;
    BOOST_REQUIRE(got1 != exp1);
    BOOST_CHECK_EQUAL(*got1, *exp1);

    UserType *got2 = got.find(2)->second;
    BOOST_REQUIRE(got2 != exp2);
    BOOST_CHECK_EQUAL(*got2, *exp2);
}

template <template <class = int, class = UserType, class = std::less<int>,
                    class = std::allocator<std::pair<const int, UserType> > >
          class Container>
void saveLoadResetObjectAddress() {
    Container<> exp_container;
    UserType *exp_pointer = &exp_container.insert(
        exp_container.begin(), std::make_pair(0, UserType(0)))->second;

    boost::property_tree::ptree ptree;
    sc_checkpoint::OStateTree otree(&ptree);
    sc_checkpoint::OArchive oarchive(&otree);
    oarchive << sc_checkpoint::serialization::create_smd("container",
                                                         exp_container)
             << sc_checkpoint::serialization::create_smd("pointer",
                                                         exp_pointer);

    Container<> got_container;
    UserType *got_pointer = NULL;

    sc_checkpoint::IStateTree itree(&ptree);
    sc_checkpoint::IArchive iarchive(&itree);
    iarchive >> sc_checkpoint::serialization::create_smd("container",
                                                         got_container)
             >> sc_checkpoint::serialization::create_smd("pointer",
                                                         got_pointer);

    BOOST_REQUIRE(got_pointer != exp_pointer);
    BOOST_CHECK_EQUAL(*got_pointer, *exp_pointer);
    BOOST_CHECK_EQUAL(got_pointer, &got_container.find(0)->second);
}

template <template <class = int, class = DerivedA, class = std::less<int>,
                    class = std::allocator<std::pair<const int, DerivedA> > >
          class Container>
void saveLoadBaseObject() {
    Container<> c;
    c.insert(c.begin(), std::make_pair(0, DerivedA(0, 0)));
    c.insert(c.begin(), std::make_pair(1, DerivedA(1, -1)));
    c.insert(c.begin(), std::make_pair(2, DerivedA(2, -2)));

    saveLoadCheckEqual(c);
}

template <template <class = int, class = Base *, class = std::less<int>,
                    class = std::allocator<std::pair<const int, Base *> > >
          class Container>
void saveLoadPolymorphism() {
    Container<> exp;
    DerivedA *exp0 = dynamic_cast<DerivedA *>(
        exp.insert(exp.begin(), std::make_pair(0, new DerivedA(1, 2)))->second);
    DerivedB *exp1 = dynamic_cast<DerivedB *>(
        exp.insert(exp.begin(), std::make_pair(
                       1, new DerivedB(3, 4, 5)))->second);
    DerivedA *exp2 = dynamic_cast<DerivedA *>(
        exp.insert(exp.begin(), std::make_pair(2, new DerivedA(6, 7)))->second);

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Container<> got;
    load(got, ptree);

    DerivedA *got0 = dynamic_cast<DerivedA *>(got.find(0)->second);
    BOOST_REQUIRE(got0 != exp0);
    BOOST_CHECK_EQUAL(*got0, *exp0);

    DerivedB *got1 = dynamic_cast<DerivedB *>(got.find(1)->second);
    BOOST_REQUIRE(got1 != exp1);
    BOOST_CHECK_EQUAL(*got1, *exp1);

    DerivedA *got2 = dynamic_cast<DerivedA *>(got.find(2)->second);
    BOOST_REQUIRE(got2 != exp2);
    BOOST_CHECK_EQUAL(*got2, *exp2);
}

template <template <class = int, class = int, class = std::less<int>,
                    class = std::allocator<std::pair<const int, int> > >
          class Container>
void saveLoadSize() {
    Container<> c;

    c.insert(std::make_pair(0, 0));
    c.insert(std::make_pair(1, 0));
    c.insert(std::make_pair(2, 0));

    boost::property_tree::ptree ptree;
    save(c, ptree);
    load(c, ptree);

    BOOST_CHECK_EQUAL(c.size(), 3u);
}

template <template <class = char, class = char, class = std::less<char>,
                    class = std::allocator<std::pair<const char, char> > >
          class Container>
void saveLoadOrder() {
    Container<> capitals;
    capitals.insert(std::make_pair('b', 'B'));
    capitals.insert(std::make_pair('c', 'C'));
    capitals.insert(std::make_pair('a', 'A'));

    boost::property_tree::ptree ptree;
    save(capitals, ptree);

    Container<> got;
    load(got, ptree);
    BOOST_REQUIRE_EQUAL(got.size(), capitals.size());

    typename Container<>::const_iterator it = got.begin();
    BOOST_CHECK_EQUAL(it++->first, 'a');
    BOOST_CHECK_EQUAL(it++->first, 'b');
    BOOST_CHECK_EQUAL(it++->first, 'c');
    BOOST_CHECK(it == got.end());
}

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_MAP_TEST_ENVIRONMENT_H
