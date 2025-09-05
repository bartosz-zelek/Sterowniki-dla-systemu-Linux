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
#include <systemc-checkpoint/serialization/serializer.h>

#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cci/serialization/serialization.hpp>
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

#include <systemc>

using boost::assign::list_of;
using sc_core::sc_module_name;
using std::vector;

namespace {

SC_MODULE(A) {
    A(sc_module_name n) : sc_module(n) {}
    template <class Archive>
    void serialize(Archive &, const unsigned int) {}
};

SC_MODULE(B) {
    B(sc_module_name n) : sc_module(n) {}
    template <class Archive>
    void serialize(Archive &, const unsigned int) {}
};

SC_MODULE(Ordered) {
    Ordered(sc_module_name n) : sc_module(n), a_("a"), b_("b"), c_("c") {}
    template <class Archive>
    void serialize(Archive &, const unsigned int) {}

    A a_;
    B b_;
    A c_;
};

SC_MODULE(Inverted) {
    Inverted(sc_module_name n) : sc_module(n), c_("c"), b_("b"), a_("a") {}
    template <class Archive>
    void serialize(Archive &, const unsigned int) {}

    A c_;
    B b_;
    A a_;
};

}  // namespace

static sc_checkpoint::serialization::Serializer<A> a_serializer;
static sc_checkpoint::serialization::Serializer<B> b_serializer;
static sc_checkpoint::serialization::Serializer<Ordered> ordered_serializer;
static sc_checkpoint::serialization::Serializer<Inverted> inverted_serializer;

static vector<std::string> alphabetical = list_of("m")("m.a")("m.b")("m.c");

template <class Module>
class SerializedModule {
  public:
    SerializedModule() : m_("m") {
        sc_checkpoint::OStateTree otree(&ptree_);
        sc_checkpoint::OArchive oarchive(&otree);
        sc_checkpoint::SerializerRegistry::Instance()->save(oarchive, 0);
    }

    vector<std::string> objects() {
        vector<std::string> objects;
        for (std::size_t i = 0; i < 4; ++i) {
            objects.push_back(ptree_.get<std::string>(itemId(i)));
        }
        return objects;
    }

  private:
    std::string itemId(std::size_t idx) {
        return "checkpoint.objects.item_" + boost::lexical_cast<
            std::string>(idx) + ".value";
    }

    Module m_;
    boost::property_tree::ptree ptree_;
};

BOOST_AUTO_TEST_SUITE(ModuleOrder)

BOOST_FIXTURE_TEST_CASE(MaintainAlphabeticalOrder, SerializedModule<Ordered>) {
    vector<std::string> order = objects();
    BOOST_CHECK_EQUAL_COLLECTIONS(order.begin(), order.end(),
                                  alphabetical.begin(), alphabetical.end());
}

BOOST_FIXTURE_TEST_CASE(EnforceAlphabeticalOrder, SerializedModule<Inverted>) {
    vector<std::string> order = objects();
    BOOST_CHECK_EQUAL_COLLECTIONS(order.begin(), order.end(),
                                  alphabetical.begin(), alphabetical.end());
}

BOOST_AUTO_TEST_SUITE_END()
