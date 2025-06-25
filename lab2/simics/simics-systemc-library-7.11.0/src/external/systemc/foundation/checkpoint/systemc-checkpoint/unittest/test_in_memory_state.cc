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

#include <systemc-checkpoint/in_memory_state.h>
#include <in_memory_state.cc>  // NOLINT(build/include)

#include <unittest/simulation.h>
#include <unittest/mock_state_keeper.h>
#include <unittest/mock_notify_callbacks.h>
#include <unittest/mock_boost_serialize.h>

using sc_checkpoint::InMemoryState;
using unittest::MockBoostSerialize;

BOOST_AUTO_TEST_SUITE(TestInMemoryState)

class Env {
  public:
    Env() : memory_(&keeper_, &notify_callbacks_,
                   &state_registry_, &kernel_) {
        state_registry_.value_ = 1;
        kernel_.value_ = 1;
    }
    unittest::MockStateKeeper keeper_;
    unittest::MockNotifyCallbacks notify_callbacks_;
    MockBoostSerialize state_registry_;
    MockBoostSerialize kernel_;
    InMemoryState<MockBoostSerialize, MockBoostSerialize> memory_;
};

BOOST_FIXTURE_TEST_CASE(TestSaveAndRestore, Env) {
    void *s1 = NULL;
    BOOST_REQUIRE(memory_.save(&s1));

    BOOST_CHECK_EQUAL(keeper_.save_called_, 1);
    BOOST_CHECK_EQUAL(keeper_.save_handle_, 1);

    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_num_invocations_, 1);
    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_SAVE);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_num_invocations_, 1);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_SAVE);

    BOOST_CHECK_EQUAL(state_registry_.save_called_, 1);
    BOOST_CHECK_EQUAL(kernel_.save_called_, 1);

    state_registry_.value_ = 2;
    kernel_.value_ = 2;

    BOOST_REQUIRE(memory_.restore(s1));

    BOOST_CHECK_EQUAL(keeper_.restore_called_, 1);

    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_num_invocations_, 2);
    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_LOAD);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_num_invocations_, 2);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_LOAD);

    BOOST_CHECK_EQUAL(state_registry_.load_called_, 1);
    BOOST_CHECK_EQUAL(kernel_.load_called_, 1);

    BOOST_CHECK_EQUAL(state_registry_.value_, 1);
    BOOST_CHECK_EQUAL(kernel_.value_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestMerge, Env) {
    state_registry_.value_ = 1;
    kernel_.value_ = 1;

    void *s1 = NULL;
    BOOST_REQUIRE(memory_.save(&s1));

    state_registry_.value_ = 2;
    kernel_.value_ = 2;

    void *s2 = NULL;
    BOOST_REQUIRE(memory_.save(&s2));

    state_registry_.value_ = 3;
    kernel_.value_ = 3;

    BOOST_REQUIRE(memory_.merge(s1, s2));

    BOOST_CHECK_EQUAL(state_registry_.value_, 3);
    BOOST_CHECK_EQUAL(kernel_.value_, 3);
    BOOST_CHECK_EQUAL(keeper_.merge_handle_previous_,
                      reinterpret_cast<void *>(1));
    BOOST_CHECK_EQUAL(keeper_.merge_handle_remove_,
                      reinterpret_cast<void *>(2));

    BOOST_REQUIRE(memory_.restore(s1));

    BOOST_CHECK_EQUAL(state_registry_.value_, 1);
    BOOST_CHECK_EQUAL(kernel_.value_, 1);
    BOOST_CHECK_EQUAL(memory_.restore(s2), false);

    BOOST_REQUIRE(memory_.merge(NULL, s1));
    BOOST_CHECK_EQUAL(keeper_.merge_handle_remove_,
                      reinterpret_cast<void *>(1));
    BOOST_CHECK_EQUAL(memory_.restore(s1), false);
}

BOOST_AUTO_TEST_SUITE_END()
