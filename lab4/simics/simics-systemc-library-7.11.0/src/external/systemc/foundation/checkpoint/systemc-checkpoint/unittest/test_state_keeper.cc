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

#include <systemc-checkpoint/state_keeper.h>
#include <unittest/simulation.h>
#include "mock_sparse_memory.h"

#include <string>
#include <vector>

BOOST_AUTO_TEST_SUITE(TestStateKeeper)

class Env {
  public:
    Env() : mock("Mock"), mock2("Mock2") {}
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

    unittest::Simulation simulation;
    unittest::MockSparseMemory mock;
    unittest::MockSparseMemory mock2;
    sc_checkpoint::StateKeeper keeper;
};

BOOST_FIXTURE_TEST_CASE(TestStateKeeperMock, Env) {
    char dataIn[2] = {2, 3};

    mock.write_to(4000, dataIn, 1);
    read_check(&mock, 4000, 2, 0);

    void *handle = NULL;
    mock.save(&handle);

    mock.write_to(4000, dataIn, 2);
    read_check(&mock, 4000, 2, 3);

    mock.restore(handle);
    read_check(&mock, 4000, 2, 0);

    mock.write_to(4000, dataIn, 2);
    void *handleRemove = NULL;
    mock.save(&handleRemove);

    mock.merge(handle, handleRemove);
    mock.restore(handle);
    read_check(&mock, 4000, 2, 3);

    std::vector<std::string> dirs;
    dirs.push_back("path");
    mock.write_standalone(dirs.back(), "prefix", false);
    mock.write_to(6000, dataIn, 2);

    mock.read(dirs, "prefix", false);
    read_check(&mock, 4000, 2, 3);
    read_check(&mock, 6000, 0, 0);

    mock.write_to(1000, dataIn, 2);
    dirs.push_back("path");
    mock.write_standalone(dirs.back(), "A", false);
    mock.write_to(2000, dataIn, 2);
    dirs.push_back("path");
    mock.write_standalone(dirs.back(), "B", false);

    mock.read(dirs, "B", false);

    read_check(&mock, 1000, 2, 3);
    read_check(&mock, 2000, 2, 3);
    read_check(&mock, 4000, 2, 3);
    read_check(&mock, 6000, 0, 0);
}

BOOST_FIXTURE_TEST_CASE(TestInMemorySaveAndRestore, Env) {
    void *handle_t0 = NULL;
    BOOST_CHECK_EQUAL(keeper.save(&handle_t0), true);

    write(&mock, 1000, 11, 12);
    write(&mock2, 1000, 13, 14);

    void *handle_t1 = NULL;
    BOOST_CHECK_EQUAL(keeper.save(&handle_t1), true);
    BOOST_CHECK_EQUAL(keeper.restore(handle_t0), true);

    read_check(&mock, 1000, 0, 0);
    read_check(&mock2, 1000, 0, 0);

    BOOST_CHECK_EQUAL(keeper.restore(handle_t1), true);

    read_check(&mock, 1000, 11, 12);
    read_check(&mock2, 1000, 13, 14);
}

BOOST_FIXTURE_TEST_CASE(TestInMemoryMerge, Env) {
    write(&mock, 1000, 11, 12);
    write(&mock2, 1000, 13, 14);

    void *handle_t0 = NULL;
    BOOST_CHECK_EQUAL(keeper.save(&handle_t0), true);

    write(&mock, 2000, 21, 22);
    write(&mock2, 2000, 23, 24);

    void *handle_t1 = NULL;
    BOOST_CHECK_EQUAL(keeper.save(&handle_t1), true);

    write(&mock, 3000, 31, 32);
    write(&mock2, 3000, 33, 34);

    void *handle_t2 = NULL;
    BOOST_CHECK_EQUAL(keeper.save(&handle_t2), true);

    BOOST_CHECK_EQUAL(keeper.merge(handle_t0, handle_t1), true);
    BOOST_CHECK_EQUAL(keeper.restore(handle_t0), true);

    read_check(&mock, 1000, 11, 12);
    read_check(&mock2, 1000, 13, 14);

    read_check(&mock, 2000, 21, 22);
    read_check(&mock2, 2000, 23, 24);

    read_check(&mock, 3000, 0, 0);
    read_check(&mock2, 3000, 0, 0);

    BOOST_CHECK_EQUAL(keeper.restore(&handle_t1), false);
    BOOST_CHECK_EQUAL(keeper.merge(handle_t0, handle_t1), false);

    // Test deleting states
    BOOST_CHECK_EQUAL(keeper.restore(handle_t2), true);
    BOOST_CHECK_EQUAL(keeper.merge(NULL, handle_t2), true);
    BOOST_CHECK_EQUAL(keeper.restore(handle_t2), false);
}

BOOST_FIXTURE_TEST_CASE(TestFullFileReadAndWrite, Env) {
    write(&mock, 1000, 11, 12);
    write(&mock2, 1000, 13, 14);

    std::vector<std::string> dirs;
    dirs.push_back("path");

    BOOST_CHECK(keeper.write_standalone(dirs.back(), "", false));

    write(&mock, 2000, 21, 22);
    write(&mock2, 2000, 23, 24);

    BOOST_CHECK_EQUAL(keeper.read(dirs, "", false), true);

    read_check(&mock, 1000, 11, 12);
    read_check(&mock2, 1000, 13, 14);
    read_check(&mock, 2000, 0, 0);
    read_check(&mock2, 2000, 0, 0);
}

BOOST_FIXTURE_TEST_CASE(TestIncrementalFileReadAndWrite, Env) {
    write(&mock, 1000, 11, 12);
    write(&mock2, 1000, 13, 14);

    std::vector<std::string> dirs;
    dirs.push_back("path_A");
    BOOST_CHECK(keeper.write_standalone(dirs.back(), "", false));

    write(&mock, 2000, 21, 22);
    write(&mock2, 2000, 23, 24);

    dirs.push_back("path_B");
    BOOST_CHECK(keeper.write(dirs.back(), "", false));

    write(&mock, 3000, 31, 32);
    write(&mock2, 3000, 33, 34);

    dirs.push_back("path_C");
    BOOST_CHECK(keeper.write(dirs.back(), "", false));

    write(&mock, 4000, 41, 42);
    write(&mock2, 4000, 43, 44);

    BOOST_CHECK_EQUAL(keeper.read(dirs, "", false), true);

    read_check(&mock, 1000, 11, 12);
    read_check(&mock2, 1000, 13, 14);
    read_check(&mock, 2000, 21, 22);
    read_check(&mock2, 2000, 23, 24);
    read_check(&mock, 3000, 31, 32);
    read_check(&mock2, 3000, 33, 34);
    read_check(&mock, 4000, 0, 0);
    read_check(&mock2, 4000, 0, 0);
}

BOOST_FIXTURE_TEST_CASE(TestFilePrefixWithObjectInstanceNumber, Env) {
    std::vector<std::string> dirs;
    dirs.push_back("path_A");
    BOOST_REQUIRE(keeper.write_standalone(dirs.back(), "", false));

    dirs.push_back("path_B");
    BOOST_REQUIRE(keeper.write(dirs.back(), "", false));

    dirs.push_back("path_C");
    BOOST_REQUIRE(keeper.write(dirs.back(), "", false));

    BOOST_CHECK(mock.read(dirs, "0", false));
    BOOST_CHECK(mock2.read(dirs, "1", false));
}

BOOST_AUTO_TEST_SUITE_END()
