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

#include <string>
#include <vector>

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::path;
using std::filesystem::exists;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
using std::experimental::filesystem::exists;
#endif

#include <boost/test/unit_test.hpp>

#include <systemc>

#include <systemc-checkpoint/in_memory_state.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/notify_callbacks.h>
#include <systemc-checkpoint/on_disk_state.h>
#include <systemc-checkpoint/serialization/serializer.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>
#include <systemc-checkpoint/state_keeper.h>

#include <unittest/scratch_directory.h>
#include <unittest/simulation.h>
#include "mock_sparse_memory.h"

using sc_checkpoint::InMemoryState;
using sc_checkpoint::Kernel;
using sc_checkpoint::OnDiskState;
using sc_checkpoint::SerializerRegistry;
using sc_core::sc_time_stamp;
using sc_core::sc_start;
using unittest::ScratchDirectory;

BOOST_GLOBAL_FIXTURE(ScratchDirectory);
BOOST_AUTO_TEST_SUITE(TestInModule);

SC_MODULE(TestModule) {
    SC_CTOR(TestModule) : state_(0) {}
    int state_;
    template <class Archive>
    void serialize(Archive& ar,
                   const unsigned int version) {
        ar & SMD(state_);
    }
};

static sc_checkpoint::serialization::Serializer<TestModule> serializer;

class Env {
  public:
    Env() : module_("TestModule"), registry_(SerializerRegistry::Instance()),
            mock_("mock"), mock2_("mock2") {}

    void write(unittest::MockSparseMemory *m, int addr, int v1, int v2) {
        char data[2];
        data[0] = v1;
        data[1] = v2;

        m->write_to(addr, data, 2);
    }
    void read_check(unittest::MockSparseMemory *m, int addr, int v1, int v2) {
        char data[2] = {0, 0};
        m->read_from(addr, data, 2);
        BOOST_CHECK_EQUAL(data[0], v1);
        BOOST_CHECK_EQUAL(data[1], v2);
    }

    unittest::Simulation simulation_;
    TestModule module_;
    sc_checkpoint::SerializerRegistry *registry_;
    sc_checkpoint::StateKeeper keeper_;
    sc_checkpoint::NotifyCallbacks notify_callbacks_;
    sc_checkpoint::Kernel kernel_;
    unittest::MockSparseMemory mock_;
    unittest::MockSparseMemory mock2_;
};

class InMemoryEnv : public Env {
  public:
    InMemoryEnv() : Env(), in_memory_state_(&keeper_, &notify_callbacks_,
                                            registry_, &kernel_) {}

    InMemoryState<SerializerRegistry, Kernel> in_memory_state_;
};

class OnDiskEnv : public Env {
  public:
    OnDiskEnv() : Env(), on_disk_state_(registry_, &kernel_,
                                        &keeper_, &notify_callbacks_) {}

    OnDiskState on_disk_state_;
};

BOOST_FIXTURE_TEST_CASE(TestInMemorySaveRestore, InMemoryEnv) {
    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 10, 20);
    write(&mock2_, 2000, 10, 20);
    module_.state_ = 1;

    void *s1 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s1));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 20, 30);
    write(&mock2_, 2000, 20, 30);
    module_.state_ = 2;

    BOOST_REQUIRE(in_memory_state_.restore(s1));

    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    read_check(&mock_, 1000, 10, 20);
    read_check(&mock2_, 2000, 10, 20);
    BOOST_CHECK_EQUAL(module_.state_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestInMemoryMultipleSaveRestore, InMemoryEnv) {
    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 10, 20);
    write(&mock2_, 2000, 10, 20);
    module_.state_ = 1;

    void *s1 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s1));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 20, 30);
    write(&mock2_, 2000, 20, 30);
    module_.state_ = 2;

    void *s2 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s2));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 30, 40);
    write(&mock2_, 2000, 30, 40);
    module_.state_ = 3;

    void *s3 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s3));

    BOOST_REQUIRE(in_memory_state_.restore(s1));
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    read_check(&mock_, 1000, 10, 20);
    read_check(&mock2_, 2000, 10, 20);
    BOOST_CHECK_EQUAL(module_.state_, 1);

    BOOST_REQUIRE(in_memory_state_.restore(s2));
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(10));
    read_check(&mock_, 1000, 20, 30);
    read_check(&mock2_, 2000, 20, 30);
    BOOST_CHECK_EQUAL(module_.state_, 2);

    BOOST_REQUIRE(in_memory_state_.restore(s3));
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(15));
    read_check(&mock_, 1000, 30, 40);
    read_check(&mock2_, 2000, 30, 40);
    BOOST_CHECK_EQUAL(module_.state_, 3);

    sc_start(5, sc_core::SC_PS);
    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(20));
}

BOOST_FIXTURE_TEST_CASE(TestInMemoryMerge, InMemoryEnv) {
    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 10, 20);
    write(&mock2_, 2000, 10, 20);
    module_.state_ = 1;

    void *s1 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s1));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 20, 30);
    write(&mock2_, 2000, 20, 30);
    module_.state_ = 2;

    void *s2 = NULL;
    BOOST_REQUIRE(in_memory_state_.save(&s2));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 30, 40);
    write(&mock2_, 2000, 30, 40);
    module_.state_ = 3;

    BOOST_REQUIRE(in_memory_state_.merge(s1, s2));
    BOOST_REQUIRE(in_memory_state_.restore(s1));

    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    read_check(&mock_, 1000, 20, 30);
    read_check(&mock2_, 2000, 20, 30);
    BOOST_CHECK_EQUAL(module_.state_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteDirectoryExists, OnDiskEnv) {
    std::string sc_chkpt = ScratchDirectory::create_directory(
        "TestOnDiskWriteDirectoryExists");
    BOOST_CHECK(!on_disk_state_.write(sc_chkpt, false));
    BOOST_CHECK(!on_disk_state_.write_standalone(sc_chkpt, false));
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteFileExists, OnDiskEnv) {
    std::string sc_chkpt = ScratchDirectory::create_file(
        "TestOnDiskWriteFileExists");
    BOOST_CHECK(!on_disk_state_.write(sc_chkpt, false));
    BOOST_CHECK(!on_disk_state_.write_standalone(sc_chkpt, false));
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteComplete, OnDiskEnv) {
    std::string sc_chkpt = ScratchDirectory::filename(
        "TestOnDiskWriteComplete");
    BOOST_REQUIRE(on_disk_state_.write_standalone(sc_chkpt, false));
    BOOST_CHECK(exists(path(sc_chkpt) / path("1")));
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWritePartial, OnDiskEnv) {
    std::string sc_chkpt = ScratchDirectory::filename("TestOnDiskWritePartial");
    BOOST_REQUIRE(on_disk_state_.write(sc_chkpt, false));
    BOOST_CHECK(exists(path(sc_chkpt) / path("1")));
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteRead, OnDiskEnv) {
    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 10, 20);
    write(&mock2_, 2000, 10, 20);
    module_.state_ = 1;

    std::vector<std::string> checkpoints;
    checkpoints.push_back(ScratchDirectory::filename("TestOnDiskSaveRestore"));
    BOOST_REQUIRE(on_disk_state_.write_standalone(checkpoints.back(), false));

    sc_start(5, sc_core::SC_PS);
    write(&mock_, 1000, 20, 30);
    write(&mock2_, 2000, 20, 30);
    module_.state_ = 2;

    BOOST_REQUIRE(on_disk_state_.read(checkpoints, false));

    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(5));
    read_check(&mock_, 1000, 10, 20);
    read_check(&mock2_, 2000, 10, 20);
    BOOST_CHECK_EQUAL(module_.state_, 1);
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteReadMultipleComplete, OnDiskEnv) {
    sc_start(1, sc_core::SC_PS);
    write(&mock_, 1000, 1, 1);
    module_.state_ = 1;

    std::vector<std::string> checkpoints;
    checkpoints.push_back(ScratchDirectory::filename("MultipleComplete1"));
    BOOST_REQUIRE(on_disk_state_.write_standalone(checkpoints.back(), false));

    sc_start(1, sc_core::SC_PS);
    write(&mock2_, 2000, 2, 2);

    checkpoints.push_back(ScratchDirectory::filename("MultipleComplete2"));
    BOOST_REQUIRE(on_disk_state_.write_standalone(checkpoints.back(), false));

    write(&mock_, 1000, 1, 3);
    module_.state_ = 3;

    checkpoints.push_back(ScratchDirectory::filename("MultipleComplete3"));
    BOOST_REQUIRE(on_disk_state_.write_standalone(checkpoints.back(), false));

    sc_start(10, sc_core::SC_PS);
    write(&mock_, 1000, 0, 0);
    write(&mock2_, 2000, 0, 0);
    module_.state_ = 0;

    BOOST_REQUIRE(on_disk_state_.read(checkpoints, false));

    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(2));
    read_check(&mock_, 1000, 1, 3);
    read_check(&mock2_, 2000, 2, 2);
    BOOST_CHECK_EQUAL(module_.state_, 3);
}

BOOST_FIXTURE_TEST_CASE(TestOnDiskWriteReadMultiplePartial, OnDiskEnv) {
    sc_start(1, sc_core::SC_PS);
    write(&mock_, 1000, 1, 1);
    module_.state_ = 1;

    std::vector<std::string> checkpoints;
    checkpoints.push_back(ScratchDirectory::filename("MultiplePartial1"));
    BOOST_REQUIRE(on_disk_state_.write_standalone(checkpoints.back(), false));

    sc_start(1, sc_core::SC_PS);
    write(&mock2_, 2000, 2, 2);

    checkpoints.push_back(ScratchDirectory::filename("MultiplePartial2"));
    BOOST_REQUIRE(on_disk_state_.write(checkpoints.back(), false));

    write(&mock_, 1000, 1, 3);
    module_.state_ = 3;

    checkpoints.push_back(ScratchDirectory::filename("MultiplePartial3"));
    BOOST_REQUIRE(on_disk_state_.write(checkpoints.back(), false));

    sc_start(10, sc_core::SC_PS);
    write(&mock_, 1000, 0, 0);
    write(&mock2_, 2000, 0, 0);
    module_.state_ = 0;

    BOOST_REQUIRE(on_disk_state_.read(checkpoints, false));

    BOOST_CHECK_EQUAL(sc_time_stamp().value(), static_cast<unsigned>(2));
    read_check(&mock_, 1000, 1, 3);
    read_check(&mock2_, 2000, 2, 2);
    BOOST_CHECK_EQUAL(module_.state_, 3);
}

BOOST_AUTO_TEST_SUITE_END()
