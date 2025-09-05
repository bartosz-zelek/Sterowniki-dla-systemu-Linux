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
#include <boost/property_tree/ptree.hpp>
#include <cci/serialization/serialization.hpp>

#include <systemc>
#include <tlm>

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/smd.h>

#include <unittest/simulation.h>

#include <deque>

using tlm::tlm_generic_payload;

BOOST_AUTO_TEST_SUITE(TestPayload)

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) {}
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp_);
    }

    tlm_generic_payload gp_;
};

class TransactionPool : public tlm::tlm_mm_interface {
  public:
    virtual ~TransactionPool() {
        for (std::deque<tlm_generic_payload *>::iterator it = pool_.begin();
            it != pool_.end(); ++it) {
            delete *it;
        }
    }

    tlm_generic_payload *new_payload() {
        tlm_generic_payload *gp =
            pool_.empty() ? new tlm_generic_payload(this) : pool_.front();
        gp->acquire();
        return gp;
    }

    virtual void free(tlm_generic_payload *gp) {
        pool_.push_back(gp);
    }

    std::deque<tlm_generic_payload *> pool_;
};

SC_MODULE(TestModuleWithPool) {
    SC_CTOR(TestModuleWithPool) : gp_(pool_.new_payload()) {
    }
    virtual ~TestModuleWithPool() {
        gp_->release();
    }
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(gp_);
    }

    TransactionPool pool_;
    tlm_generic_payload *gp_;
};

SC_MODULE(TestModuleReceiver) {
    SC_CTOR(TestModuleReceiver) : gp_(NULL) {
    }
    void set_gp(tlm_generic_payload *gp) {
        gp_ = gp;
        gp_->acquire();
    }
    virtual ~TestModuleReceiver() {
        if (gp_)
            gp_->release();
    }
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar.push("scope");
        ar & SMD(gp_);
        ar.pop();
    }

    tlm_generic_payload *gp_;
};

template <class T>
class Env {
  public:
    Env() : module_("module"), istate_(&ptree), ia_(&istate_),
            ostate_(&ptree), oa_(&ostate_) {}
    void store() {
        module_.serialize(oa_, 0);
    }
    template<class M>
    void store(M *module) {
        module->serialize(oa_, 0);
    }
    template<class M>
    void restore(M *module) {
        module->serialize(ia_, 0);
    }
    void set_values(tlm_generic_payload *gp) {
        gp->set_address(0x1000);
        gp->set_command(tlm::TLM_WRITE_COMMAND);
        gp->set_byte_enable_length(8);
        gp->set_data_length(64);
        gp->set_dmi_allowed(true);
        gp->set_gp_option(tlm::TLM_FULL_PAYLOAD_ACCEPTED);
        gp->set_response_status(tlm::TLM_OK_RESPONSE);
        gp->set_streaming_width(16);
    }
    void check_values(tlm_generic_payload *gp) {
        BOOST_CHECK_EQUAL(gp->get_address(),
                          static_cast<sc_dt::uint64>(0x1000));
        BOOST_CHECK_EQUAL(gp->get_command(), tlm::TLM_WRITE_COMMAND);
        BOOST_CHECK_EQUAL(gp->get_byte_enable_length(),
                          static_cast<unsigned int>(8));
        BOOST_CHECK_EQUAL(gp->get_data_length(),
                          static_cast<unsigned int>(64));
        BOOST_CHECK_EQUAL(gp->is_dmi_allowed(), true);
        BOOST_CHECK_EQUAL(gp->get_gp_option(),
                          tlm::TLM_FULL_PAYLOAD_ACCEPTED);
        BOOST_CHECK_EQUAL(gp->get_response_status(),
                          tlm::TLM_OK_RESPONSE);
        BOOST_CHECK_EQUAL(gp->get_streaming_width(),
                          static_cast<unsigned int>(16));
    }

    unittest::Simulation simulation_;
    T module_;
    boost::property_tree::ptree ptree;
    sc_checkpoint::IStateTree istate_;
    sc_checkpoint::IArchive ia_;
    sc_checkpoint::OStateTree ostate_;
    sc_checkpoint::OArchive oa_;
};

BOOST_FIXTURE_TEST_CASE(TestGenericPayloadRestore, Env<TestModule>) {
    set_values(&module_.gp_);
    store();
    {
        unittest::Simulation s;
        TestModule module("TestModule");
        restore(&module);
        check_values(&module.gp_);
    }
}

BOOST_FIXTURE_TEST_CASE(TestPoolRestore, Env<TestModuleWithPool>) {
    set_values(module_.gp_);
    store();
    {
        unittest::Simulation s;
        TestModuleWithPool module("TestModule");
        restore(&module);
        check_values(module.gp_);
    }
}

BOOST_FIXTURE_TEST_CASE(TestSharedPoolGPRestore, Env<TestModuleWithPool>) {
    TestModuleReceiver receiver("TestModuleReceiving");
    receiver.set_gp(module_.gp_);
    set_values(module_.gp_);
    store();
    store(&receiver);
    {
        unittest::Simulation s;
        TestModuleWithPool sender("TestModuleWithPool");
        TestModuleReceiver receiver("TestModuleReceiving");
        restore(&sender);
        restore(&receiver);
        BOOST_CHECK_EQUAL(sender.gp_, receiver.gp_);
        check_values(receiver.gp_);
    }
}

BOOST_AUTO_TEST_SUITE_END()
