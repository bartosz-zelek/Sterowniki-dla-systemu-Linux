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

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::exists;
using std::filesystem::path;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::exists;
using std::experimental::filesystem::path;
#endif

#include <boost/test/unit_test.hpp>

#include <systemc-checkpoint/on_disk_systemc_state.h>
#include <on_disk_systemc_state.cc>  // NOLINT(build/include)

#include <unittest/mock_boost_serialize.h>
#include <unittest/mock_notify_callbacks.h>
#include <unittest/scratch_directory.h>

#include <map>
#include <string>
#include <vector>

using sc_checkpoint::OnDiskSystemCState;
using unittest::MockBoostSerialize;
using unittest::ScratchDirectory;

class Env {
  public:
    Env() : state_(&serializer_registry_, &kernel_, &notify_callbacks_) {}

    MockBoostSerialize serializer_registry_;
    MockBoostSerialize kernel_;
    unittest::MockNotifyCallbacks notify_callbacks_;
    OnDiskSystemCState<MockBoostSerialize, MockBoostSerialize> state_;
};

BOOST_GLOBAL_FIXTURE(ScratchDirectory);
BOOST_AUTO_TEST_SUITE(TestOnDiskState);

BOOST_FIXTURE_TEST_CASE(TestWriteSingle, Env) {
    std::string scratch = ScratchDirectory::create_directory("TestWriteSingle");

    BOOST_REQUIRE(state_.write(scratch, false));

    BOOST_CHECK_EQUAL(serializer_registry_.save_called_, 1);
    BOOST_CHECK_EQUAL(kernel_.save_called_, 1);

    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_num_invocations_, 0);
    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_NA);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_num_invocations_, 1);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_SAVE);

    BOOST_CHECK(exists(path(scratch) / path("checkpoint.json")));
}

BOOST_FIXTURE_TEST_CASE(TestReadSingle, Env) {
    std::string scratch = ScratchDirectory::create_directory("TestReadSingle");

    BOOST_REQUIRE(state_.write(scratch, false));
    BOOST_REQUIRE(exists(path(scratch) / path("checkpoint.json")));

    std::vector<std::string> checkpoint_dirs;
    checkpoint_dirs.push_back(scratch);
    BOOST_REQUIRE(state_.read(checkpoint_dirs, false));
    BOOST_CHECK_EQUAL(serializer_registry_.load_called_, 1);
    BOOST_CHECK_EQUAL(kernel_.load_called_, 1);

    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_num_invocations_, 0);
    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_NA);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_num_invocations_, 2);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_LOAD);
}

BOOST_FIXTURE_TEST_CASE(TestReadMultiple, Env) {
    std::string p1 = ScratchDirectory::create_directory("TestReadMultiple1");
    std::string p2 = ScratchDirectory::create_directory("TestReadMultiple2");

    BOOST_REQUIRE(state_.write(p1, false));
    BOOST_REQUIRE(state_.write(p2, false));

    std::vector<std::string> checkpoint_dirs;
    checkpoint_dirs.push_back(p1);
    checkpoint_dirs.push_back(p2);

    BOOST_REQUIRE(state_.read(checkpoint_dirs, false));
    BOOST_CHECK_EQUAL(serializer_registry_.load_called_, 1);
    BOOST_CHECK_EQUAL(kernel_.load_called_, 1);

    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_num_invocations_, 0);
    BOOST_CHECK_EQUAL(notify_callbacks_.pre_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_NA);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_num_invocations_, 3);
    BOOST_CHECK_EQUAL(notify_callbacks_.post_callback_arg_,
                      unittest::MockNotifyCallbacks::CallbackArg_LOAD);
}

BOOST_AUTO_TEST_SUITE_END()
