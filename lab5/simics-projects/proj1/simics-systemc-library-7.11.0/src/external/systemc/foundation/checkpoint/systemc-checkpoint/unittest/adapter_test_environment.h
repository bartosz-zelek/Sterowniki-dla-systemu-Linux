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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_ADAPTER_TEST_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_ADAPTER_TEST_ENVIRONMENT_H

#include <systemc-checkpoint/expose_container.h>
#include <unittest/stl_test_environment.h>

// adapters don't provide iterators, so we compare the underlying container
template <template <class, class> class Adapter, class T, class Container>
void checkEqual(Adapter<T, Container> got, Adapter<T, Container> exp) {
    typedef sc_checkpoint::ExposeContainer<Adapter<T, Container> > Expose;
    Container gotC = static_cast<Expose &>(got).container();
    Container expC = static_cast<Expose &>(exp).container();
    BOOST_CHECK_EQUAL_COLLECTIONS(
        expC.begin(), expC.end(), gotC.begin(), gotC.end());
}

template <template <class, class> class Adapter, class T, class Container>
void saveLoadCheckEqual(const Adapter<T, Container> &exp) {
    boost::property_tree::ptree ptree;
    save(exp, ptree);

    Adapter<T, Container> got;
    load(got, ptree);

    checkEqual(got, exp);
}

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_ADAPTER_TEST_ENVIRONMENT_H
