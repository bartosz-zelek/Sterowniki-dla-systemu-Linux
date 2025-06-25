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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_SEQUENCE_TEST_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_SEQUENCE_TEST_ENVIRONMENT_H

#include <boost/test/unit_test.hpp>

#include <unittest/polymorphism.h>
#include <unittest/stl_test_environment.h>

#include <iterator>
#include <memory>

// std::list does not have an []-operator
template <template <class, class> class Container, class T>
T atIdx(Container<T, std::allocator<T> > c, size_t idx) {
    typename Container<T, std::allocator<T> >::iterator it = c.begin();
    std::advance(it, idx);
    return *it;
}

template <class T>
void saveLoadCheckEqual(const T &exp) {
    boost::property_tree::ptree ptree;
    save(exp, ptree);

    T l;
    load(l, ptree);

    BOOST_CHECK_EQUAL_COLLECTIONS(l.begin(), l.end(), exp.begin(), exp.end());
}

template <template <class = int, class = std::allocator<int> > class Container>
void saveLoadInt() {
    Container<> c;
    c.push_back(0);
    c.push_back(1);
    c.push_back(2);

    saveLoadCheckEqual(c);
}

template <template <class = UserType, class = std::allocator<UserType> >
          class Container>
void saveLoadUserType() {
    Container<> c;
    c.push_back(UserType(0));
    c.push_back(UserType(1));
    c.push_back(UserType(2));

    saveLoadCheckEqual(c);
}

template <template <class = UserTypeNonDefaultConstructor,
                    class = std::allocator<UserTypeNonDefaultConstructor> >
          class Container>
void saveLoadNonDefaultConstructor() {
    Container<> c;
    c.push_back(UserTypeNonDefaultConstructor(0));
    c.push_back(UserTypeNonDefaultConstructor(1));
    c.push_back(UserTypeNonDefaultConstructor(2));

    saveLoadCheckEqual(c);
}

template <template <class, class> class Container>
void saveLoadContainerOfContainers() {
    typedef Container<int, std::allocator<int> > ContainerInt;
    typedef Container<ContainerInt, std::allocator<ContainerInt> > ContainerSq;

    ContainerSq exp;

    ContainerInt c;
    c.push_back(0);
    c.push_back(1);
    c.push_back(2);
    exp.push_back(c);

    c.clear();
    c.push_back(3);
    c.push_back(4);
    c.push_back(5);
    exp.push_back(c);

    c.clear();
    c.push_back(6);
    c.push_back(7);
    c.push_back(8);
    exp.push_back(c);

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    ContainerSq got;
    load(got, ptree);

    BOOST_REQUIRE_EQUAL(exp.size(), got.size());

    typename ContainerSq::const_iterator exp_it = exp.begin();
    typename ContainerSq::const_iterator got_it = got.begin();
    for (; got_it != got.end(); ++got_it, ++exp_it) {
        BOOST_CHECK_EQUAL_COLLECTIONS((*got_it).begin(), (*got_it).end(),
                                      (*exp_it).begin(), (*exp_it).end());
    }
}

template <template <class = UserType *,
                    class = std::allocator<UserType *> > class Container>
void saveLoadPointers() {
    Container<> exp;
    exp.push_back(new UserType(1));
    exp.push_back(new UserType(2));
    exp.push_back(new UserType(3));

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Container<> got;
    load(got, ptree);

    BOOST_REQUIRE_EQUAL(got.size(), exp.size());
    for (unsigned i = 0; i < exp.size(); ++i) {
        UserType *got_p = atIdx(got, i);
        UserType *exp_p = atIdx(exp, i);

        BOOST_REQUIRE(got_p != exp_p);
        BOOST_CHECK_EQUAL(*got_p, *exp_p);
    }
}

template <template <class = UserType,
                    class = std::allocator<UserType> > class Container>
void saveLoadPointersResetObjectAddress() {
    Container<> exp_container;
    exp_container.push_back(UserType(42));
    UserType *exp_pointer = &exp_container.front();

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
    BOOST_CHECK_EQUAL(got_pointer, &got_container.front());
}

template <template <class = DerivedA,
                    class = std::allocator<DerivedA> > class Container>
void saveLoadBaseObject() {
    Container<> c;
    c.push_back(DerivedA(1, -1));
    c.push_back(DerivedA(2, -2));
    c.push_back(DerivedA(3, -3));

    saveLoadCheckEqual(c);
}

template <template <class = Base *,
                    class = std::allocator<Base *> > class Container>
void saveLoadPolymorphism() {
    Container<> exp;
    exp.push_back(new DerivedA(1, 2));
    exp.push_back(new DerivedB(3, 4, 5));
    exp.push_back(new DerivedA(6, 7));

    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Container<> got;
    load(got, ptree);

    BOOST_REQUIRE_EQUAL(got.size(), exp.size());
    BOOST_REQUIRE(dynamic_cast<DerivedA *>(atIdx(got, 0)));
    BOOST_REQUIRE(dynamic_cast<DerivedB *>(atIdx(got, 1)));
    BOOST_REQUIRE(dynamic_cast<DerivedA *>(atIdx(got, 2)));

    BOOST_REQUIRE(atIdx(got, 0) != atIdx(exp, 0));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA *>(atIdx(got, 0)),
                      *dynamic_cast<DerivedA *>(atIdx(exp, 0)));

    BOOST_REQUIRE(atIdx(got, 1) != atIdx(exp, 1));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedB *>(atIdx(got, 1)),
                      *dynamic_cast<DerivedB *>(atIdx(exp, 1)));

    BOOST_REQUIRE(atIdx(got, 2) != atIdx(exp, 2));
    BOOST_CHECK_EQUAL(*dynamic_cast<DerivedA *>(atIdx(got, 2)),
                      *dynamic_cast<DerivedA *>(atIdx(exp, 2)));
}

template <template <class = int, class = std::allocator<int> > class Container>
void saveLoadSize() {
    Container<> c;
    c.push_back(0);
    c.push_back(0);
    c.push_back(0);

    boost::property_tree::ptree ptree;
    save(c, ptree);
    load(c, ptree);

    BOOST_CHECK_EQUAL(c.size(), 3u);
}

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_SEQUENCE_TEST_ENVIRONMENT_H
