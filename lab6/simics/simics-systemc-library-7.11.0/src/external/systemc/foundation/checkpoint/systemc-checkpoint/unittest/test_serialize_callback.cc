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

#include <boost/assign/list_of.hpp>
#include <boost/test/unit_test.hpp>

#include <systemc>

#include <systemc-checkpoint/checkpoint_control.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialize_callback_interface.h>
#include <systemc-checkpoint/state_keeper_interface.h>

#include <unittest/simulation.h>

#include <string>
#include <vector>

namespace {

class CallbackStateKeeperModule
    : public sc_core::sc_module,
      public sc_checkpoint::SerializeCallbackInterface,
      public sc_checkpoint::StateKeeperInterface {
  public:
    CallbackStateKeeperModule(sc_core::sc_module_name name,
                              std::vector<std::string> *invocations)
        : sc_module(name), invocations_(invocations) {
        SC_THREAD(thread);
    }
    SC_HAS_PROCESS(CallbackStateKeeperModule);

    void pre_serialize_callback(State state) {
        invocations_->push_back("pre_serialize_callback");
    }
    void post_serialize_callback(State state) {
        invocations_->push_back("post_serialize_callback");
    }

    bool save(void **handleOut) {
        *handleOut = this;
        invocations_->push_back("save");
        return true;
    }
    bool restore(void *handle) {
        invocations_->push_back("restore");
        return true;
    }
    bool merge(void *handlePrevious, void *handleRemove) {
        return true;
    }
    bool write(const std::string &dir,
               const std::string &prefix,
               bool persistent) {
        return true;
    }
    bool write_standalone(const std::string &dir,
                          const std::string &prefix,
                          bool persistent) {
        return true;
    }
    bool read(const std::vector<std::string> &dirs,
              const std::string prefix,
              bool persistent) {
        return true;
    }

    void thread() {
        invocations_->push_back("thread");
        wait(never_triggered_);
    }

    template <class Archive>
    void serialize(Archive &ar,  // NOLINT(runtime/references)
                   const unsigned version) {
        invocations_->push_back("serialize");
    }

    std::vector<std::string> *invocations_;
    sc_core::sc_event never_triggered_;
};

class Env {
  public:
    void checkOrder(std::vector<std::string> exp_order) {
        std::vector<std::string> got_order = invocations_;
        BOOST_CHECK_EQUAL_COLLECTIONS(got_order.begin(), got_order.end(),
                                      exp_order.begin(), exp_order.end());
    }

    unittest::Simulation simulation_;
    sc_checkpoint::CheckpointControl control_;
    std::vector<std::string> invocations_;
};

}  // namespace

using sc_checkpoint::serialization::Serializer;
static Serializer<CallbackStateKeeperModule> serializer;

BOOST_AUTO_TEST_SUITE(TestSerializeCallback)

BOOST_FIXTURE_TEST_CASE(Order, Env) {
    CallbackStateKeeperModule m("m", &invocations_);

    sc_start(sc_core::SC_ZERO_TIME);  // Kickstart thread
    invocations_.clear();

    void *handle = NULL;
    BOOST_REQUIRE(control_.save(&handle));

    checkOrder(boost::assign::list_of
               ("pre_serialize_callback")
               ("save")
               ("serialize")
               ("post_serialize_callback"));

    invocations_.clear();
    BOOST_REQUIRE(control_.restore(handle));

    checkOrder(boost::assign::list_of
               ("pre_serialize_callback")
               ("restore")
               ("serialize")
               ("post_serialize_callback")
               ("thread"));
}

BOOST_FIXTURE_TEST_CASE(OrderMultiple, Env) {
    CallbackStateKeeperModule m1("m1", &invocations_);
    CallbackStateKeeperModule m2("m2", &invocations_);

    sc_start(sc_core::SC_ZERO_TIME);
    invocations_.clear();

    void *handle = NULL;
    BOOST_REQUIRE(control_.save(&handle));

    checkOrder(boost::assign::list_of
               ("pre_serialize_callback")
               ("pre_serialize_callback")
               ("save")
               ("save")
               ("serialize")
               ("serialize")
               ("post_serialize_callback")
               ("post_serialize_callback"));

    invocations_.clear();
    BOOST_REQUIRE(control_.restore(handle));

    checkOrder(boost::assign::list_of
               ("pre_serialize_callback")
               ("pre_serialize_callback")
               ("restore")
               ("restore")
               ("serialize")
               ("serialize")
               ("post_serialize_callback")
               ("post_serialize_callback")
               ("thread")
               ("thread"));
}

BOOST_AUTO_TEST_SUITE_END()
