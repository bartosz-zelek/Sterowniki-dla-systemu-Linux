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

#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>
#include <unittest/serializer_environment.h>

using sc_checkpoint::serialization::Serializer;
using sc_checkpoint::SerializerRegistry;

namespace TestSerializer {

// TestModule has implementation of serialization outside the class.
// Example how to add support for serialization to class without
// altering it.
SC_MODULE(TestModule) {
    SC_CTOR(TestModule) : state_(0) {}
    int state_;
};

// TestModuleB has implementation of serialization inside the class.
class TestModuleB: public TestModule {
  public:
    explicit TestModuleB(sc_core::sc_module_name nm)
        : TestModule(nm), state_b_(0) {}
    int state_b_;
    template <class Archive>
    void serialize(Archive& ar,   // NOLINT(runtime/references)
                   const unsigned int version) {
        ar & SMD(state_b_);
    }
};

SC_MODULE(TestModuleUnderSerialization) {
    SC_CTOR(TestModuleUnderSerialization) : object_under_serialization_(NULL) {}
    sc_core::sc_object *object_under_serialization_;
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        object_under_serialization_ =
            SerializerRegistry::Instance()->object_under_serialization();
    }
};

}  // namespace TestSerializer

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               TestSerializer::TestModule &module,  // NOLINT
               const unsigned int version) {
    ar & SMD(module.state_);
}

}  // namespace serialization
}  // namespace cci

// Registration of the serializers into the registry.
static Serializer<TestSerializer::TestModule> serializer;
static Serializer<TestSerializer::TestModuleB> serializer_b;

BOOST_AUTO_TEST_SUITE(TestSerializer)

BOOST_FIXTURE_TEST_CASE(TestSerializerRestore,
                        unittest::SerializerEnvironment) {
    TestModule module("TestModule");
    module.state_ = 7;
    store();
    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restore();
        BOOST_CHECK_EQUAL(module.state_, 7);
    }
}

BOOST_FIXTURE_TEST_CASE(TestInheritanceSerializerRestore,
                        unittest::SerializerEnvironment) {
    TestModule a("TestModule");
    TestModuleB b("TestModuleB");
    a.state_ = 7;
    b.state_ = 8;
    b.state_b_ = 9;
    store();
    {
        unittest::Simulation s;
        TestModule a("TestModule");
        TestModuleB b("TestModuleB");
        restore();
        BOOST_CHECK_EQUAL(a.state_, 7);
        BOOST_CHECK_EQUAL(b.state_, 8);
        BOOST_CHECK_EQUAL(b.state_b_, 9);
    }
}

BOOST_FIXTURE_TEST_CASE(TestObjectUnderSerialization,
                        unittest::SerializerEnvironment) {
    Serializer<TestModuleUnderSerialization> serializer;
    TestModuleUnderSerialization module("TestModule");
    BOOST_CHECK(SerializerRegistry::
                Instance()->object_under_serialization() == NULL);
    store();
    BOOST_CHECK(SerializerRegistry::
                Instance()->object_under_serialization() == NULL);
    BOOST_CHECK_EQUAL(module.object_under_serialization_, &module);
    {
        unittest::Simulation s;
        TestModuleUnderSerialization module("TestModule");
        BOOST_CHECK(SerializerRegistry::
                    Instance()->object_under_serialization() == NULL);
        restore();
        BOOST_CHECK(SerializerRegistry::
                    Instance()->object_under_serialization() == NULL);
        BOOST_CHECK_EQUAL(module.object_under_serialization_, &module);
    }
}

BOOST_AUTO_TEST_SUITE_END()
