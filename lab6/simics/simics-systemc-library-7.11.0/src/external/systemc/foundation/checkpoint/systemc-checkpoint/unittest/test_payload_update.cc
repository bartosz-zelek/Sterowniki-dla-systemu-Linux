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
#include <cci/serialization/serialization.hpp>

#include <systemc>
#include <tlm>

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/payload_update.h>
#include <systemc-checkpoint/payload_update_interface.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <unittest/simulation.h>

using tlm::tlm_generic_payload;
using sc_checkpoint::PayloadUpdate;
using sc_checkpoint::PayloadUpdateRegistry;
using sc_checkpoint::serialization::Serializer;

BOOST_AUTO_TEST_SUITE(TestPayloadUpdate)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp_);
    }

    tlm_generic_payload gp_;
};

SC_MODULE(TestModulePointer) {
    SC_CTOR(TestModulePointer) {
        gp_ = new tlm_generic_payload;
    }
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp_);
    }

    tlm_generic_payload *gp_;
};

SC_MODULE(TestModuleShared) {
    SC_CTOR(TestModuleShared) : gp_(NULL) {
    }
    void set_gp(tlm_generic_payload *gp) {
        gp_ = gp;
    }
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp_);
    }

    tlm_generic_payload *gp_;
};

static Serializer<TestModule> serializer_;
static Serializer<TestModulePointer> serializer_pointer_;
static Serializer<TestModuleShared> serializer_shared_;

template<class T>
class UpdateBase : public PayloadUpdate<T> {
  public:
    UpdateBase() : set_data_ptr_called_(0),
                   set_byte_enable_ptr_called_(0),
                   set_extensions_called_(0),
                   set_memory_manager_called_(0) {}
    virtual void set_data_ptr(T *t, tlm_generic_payload *payload) {
        ++set_data_ptr_called_;
    }
    virtual void set_byte_enable_ptr(T *t,
                                     tlm_generic_payload *payload) {
        ++set_byte_enable_ptr_called_;
    }
    virtual void set_extensions(T *t, tlm_generic_payload *payload) {
        ++set_extensions_called_;
    }
    virtual void set_memory_manager(T *t, tlm_generic_payload *payload) {
        ++set_memory_manager_called_;
    }
    int set_data_ptr_called_;
    int set_byte_enable_ptr_called_;
    int set_extensions_called_;
    int set_memory_manager_called_;
};

class UpdateTestModule : public UpdateBase<TestModule> {
  public:
};

class UpdateTestModulePointer : public UpdateBase<TestModulePointer> {
  public:
};

class UpdateTestModuleShared : public UpdateBase<TestModuleShared> {
  public:
};

class TestExtension : public tlm::tlm_extension<TestExtension> {
  public:
    virtual tlm::tlm_extension_base *clone() const {
        return new TestExtension(*this);
    }
    virtual void copy_from(tlm::tlm_extension_base const &ext) {
        *this = static_cast<const TestExtension &>(ext);
    }
};

class Env {
  public:
    Env() : istate_(&ptree), ia_(&istate_),
            ostate_(&ptree), oa_(&ostate_) {}
    void store() {
        sc_checkpoint::SerializerRegistry::Instance()->save(oa_, 0);
    }
    void restore() {
        sc_checkpoint::SerializerRegistry::Instance()->load(ia_, 0);
    }

    unittest::Simulation simulation_;
    boost::property_tree::ptree ptree;
    sc_checkpoint::IStateTree istate_;
    sc_checkpoint::IArchive ia_;
    sc_checkpoint::OStateTree ostate_;
    sc_checkpoint::OArchive oa_;
};

BOOST_AUTO_TEST_CASE(TestGenericPayloadRegistry) {
    unittest::Simulation simulation_;
    TestModule module("module");
    UpdateTestModule payloadUpdate;

    PayloadUpdateRegistry::Update update =
        PayloadUpdateRegistry::UPDATE_NO_UPDATE;

    PayloadUpdateRegistry::Instance()->update(&module, &module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 0);

    update = PayloadUpdateRegistry::UPDATE_DATA_PTR;
    PayloadUpdateRegistry::Instance()->update(&module, &module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 0);

    update = PayloadUpdateRegistry::UPDATE_BYTE_ENABLE_PTR;
    PayloadUpdateRegistry::Instance()->update(&module, &module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 1);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 0);

    update = PayloadUpdateRegistry::UPDATE_EXTENSIONS;
    PayloadUpdateRegistry::Instance()->update(&module, &module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 1);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 1);

    update |= PayloadUpdateRegistry::UPDATE_DATA_PTR;
    update |= PayloadUpdateRegistry::UPDATE_BYTE_ENABLE_PTR;

    PayloadUpdateRegistry::Instance()->update(&module, &module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 2);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 2);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 2);
}

BOOST_AUTO_TEST_CASE(TestGenericPayloadRegistryOnlyForMatchingModule) {
    unittest::Simulation simulation_;
    // No update registered for TestModulePointer.
    // The update for TestModule must not be invoked.
    TestModulePointer module("module");
    UpdateTestModule payloadUpdate;

    PayloadUpdateRegistry::Update update =
        PayloadUpdateRegistry::UPDATE_NO_UPDATE;

    PayloadUpdateRegistry::Instance()->update(&module, module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 0);

    update = PayloadUpdateRegistry::UPDATE_DATA_PTR;
    update |= PayloadUpdateRegistry::UPDATE_EXTENSIONS;
    update |= PayloadUpdateRegistry::UPDATE_BYTE_ENABLE_PTR;

    PayloadUpdateRegistry::Instance()->update(&module, module.gp_, update);
    BOOST_CHECK_EQUAL(payloadUpdate.set_data_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_byte_enable_ptr_called_, 0);
    BOOST_CHECK_EQUAL(payloadUpdate.set_extensions_called_, 0);
}

BOOST_AUTO_TEST_CASE(TestGenericPayloadRegistryPointerCaching) {
    unittest::Simulation simulation_;
    TestModulePointer module("module");
    TestModuleShared shared("shared");
    UpdateTestModulePointer pointerUpdate;
    UpdateTestModuleShared sharedUpdate;

    PayloadUpdateRegistry::Update update =
        PayloadUpdateRegistry::UPDATE_DATA_PTR |
        PayloadUpdateRegistry::UPDATE_EXTENSIONS |
        PayloadUpdateRegistry::UPDATE_BYTE_ENABLE_PTR;

    // The first time when boost allocates the payload,
    // the update is invoked from payload.h. This occurs
    // when a new payload pointer occurs.
    PayloadUpdateRegistry::Instance()->update(&module, module.gp_, update);
    BOOST_CHECK_EQUAL(pointerUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(pointerUpdate.set_byte_enable_ptr_called_, 1);
    BOOST_CHECK_EQUAL(pointerUpdate.set_extensions_called_, 1);

    // If the payload pointer is shared with an other module,
    // we want to have an update for the other module as well.
    // In this case, it is done by IArchive.
    PayloadUpdateRegistry::Instance()->update(&shared, module.gp_);
    BOOST_CHECK_EQUAL(sharedUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(sharedUpdate.set_byte_enable_ptr_called_, 1);
    BOOST_CHECK_EQUAL(sharedUpdate.set_extensions_called_, 1);

    // If the payload was allocated and stored in a pointer,
    // we already got the first update, the second by IArchive,
    // must be filtered.
    PayloadUpdateRegistry::Instance()->update(&module, module.gp_);
    BOOST_CHECK_EQUAL(pointerUpdate.set_data_ptr_called_, 1);
    BOOST_CHECK_EQUAL(pointerUpdate.set_byte_enable_ptr_called_, 1);
    BOOST_CHECK_EQUAL(pointerUpdate.set_extensions_called_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadSetDataPtr, Env) {
    UpdateTestModule update;
    TestModule module("module");
    unsigned char data[1] = {};
    module.gp_.set_data_ptr(data);
    store();
    {
        unittest::Simulation simulation_;
        TestModule module("module");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadSetByteEnablePtr, Env) {
    UpdateTestModule update;
    TestModule module("module");
    unsigned char data[1] = {};
    module.gp_.set_byte_enable_ptr(data);
    store();
    {
        unittest::Simulation simulation_;
        TestModule module("module");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 0);
    }
}

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadSetExtensions, Env) {
    UpdateTestModule update;
    TestModule module("module");
    module.gp_.set_extension(new TestExtension);
    store();
    {
        unittest::Simulation simulation_;
        TestModule module("module");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadSetMemoryManager, Env) {
    UpdateTestModule update;
    TestModule module("module");
    static struct : public tlm::tlm_mm_interface {
        virtual void free(tlm::tlm_generic_payload *gp) {
        }
    } memoryManager;

    module.gp_.set_mm(&memoryManager);
    store();
    {
        unittest::Simulation simulation_;
        TestModule module("module");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 0);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 0);
        BOOST_CHECK_EQUAL(update.set_memory_manager_called_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadRestore, Env) {
    UpdateTestModule update;
    TestModule module("module");
    module.gp_.set_extension(new TestExtension);
    unsigned char data[1] = {};
    module.gp_.set_data_ptr(data);
    module.gp_.set_byte_enable_ptr(data);
    store();
    {
        unittest::Simulation simulation_;
        TestModule module("module");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 1);
    }
}

BOOST_FIXTURE_TEST_CASE(TestSharedGenericPayloadPointerRestore, Env ) {
    UpdateTestModulePointer update;
    UpdateTestModuleShared updateShared;
    TestModulePointer module("module");
    TestModuleShared shared("shared");
    shared.set_gp(module.gp_);
    module.gp_->set_extension(new TestExtension);
    unsigned char data[1] = {};
    module.gp_->set_data_ptr(data);
    module.gp_->set_byte_enable_ptr(data);
    store();
    {
        unittest::Simulation simulation_;
        TestModulePointer module("module");
        TestModuleShared shared("shared");
        restore();
        BOOST_CHECK_EQUAL(update.set_data_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_byte_enable_ptr_called_, 1);
        BOOST_CHECK_EQUAL(update.set_extensions_called_, 1);
        BOOST_CHECK_EQUAL(updateShared.set_data_ptr_called_, 1);
        BOOST_CHECK_EQUAL(updateShared.set_byte_enable_ptr_called_, 1);
        BOOST_CHECK_EQUAL(updateShared.set_extensions_called_, 1);
        BOOST_CHECK_EQUAL(module.gp_, shared.gp_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
