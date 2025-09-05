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
using std::filesystem::create_directories;
using std::filesystem::path;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::create_directories;
using std::experimental::filesystem::path;
#endif

#include <boost/test/unit_test.hpp>

#include <systemc-checkpoint/state_keeper_checkpoints.h>

#include <unittest/scratch_directory.h>

#include <fstream>  // NOLINT(readability/streams)
#include <string>
#include <vector>

using sc_checkpoint::Subdirectories;
using unittest::ScratchDirectory;

bool touch(std::string filename) {
    std::ofstream ofs(filename.c_str());
    return ofs.is_open();
}

BOOST_GLOBAL_FIXTURE(ScratchDirectory);
BOOST_AUTO_TEST_SUITE(TestStateKeeperCheckpoints);

BOOST_AUTO_TEST_CASE(Merged) {
    std::string scratch = ScratchDirectory::create_directory("Merged");
    create_directories(path(scratch) / "1");
    create_directories(path(scratch) / "2");
    create_directories(path(scratch) / "3");

    std::vector<std::string> sc_chkpts;
    sc_chkpts.push_back(scratch);
    sc_checkpoint::StateKeeperCheckpoints sk_chkpts(sc_chkpts);

    std::vector<std::string> expected_sk_chkpts;
    expected_sk_chkpts.push_back((path(scratch) / "1").string());
    expected_sk_chkpts.push_back((path(scratch) / "2").string());
    expected_sk_chkpts.push_back((path(scratch) / "3").string());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        sk_chkpts.begin(), sk_chkpts.end(),
        expected_sk_chkpts.begin(), expected_sk_chkpts.end());
}

BOOST_AUTO_TEST_CASE(Multiple) {
    std::string multiple_1 = ScratchDirectory::create_directory("Multiple1");
    std::string multiple_2 = ScratchDirectory::create_directory("Multiple2");
    std::string multiple_3 = ScratchDirectory::create_directory("Multiple3");

    create_directories(path(multiple_1) / "1");
    create_directories(path(multiple_2) / "1");
    create_directories(path(multiple_3) / "1");

    std::vector<std::string> sc_chkpts;
    sc_chkpts.push_back(multiple_1);
    sc_chkpts.push_back(multiple_2);
    sc_chkpts.push_back(multiple_3);
    sc_checkpoint::StateKeeperCheckpoints sk_chkpts(sc_chkpts);

    std::vector<std::string> expected_sk_chkpts;
    expected_sk_chkpts.push_back((path(multiple_1) / "1").string());
    expected_sk_chkpts.push_back((path(multiple_2) / "1").string());
    expected_sk_chkpts.push_back((path(multiple_3) / "1").string());

    BOOST_CHECK_EQUAL_COLLECTIONS(
        sk_chkpts.begin(), sk_chkpts.end(),
        expected_sk_chkpts.begin(), expected_sk_chkpts.end());
}

BOOST_AUTO_TEST_SUITE_END()
