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
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
#endif

#include <boost/test/unit_test.hpp>

#include <systemc-checkpoint/subdirectories.h>

#include <unittest/scratch_directory.h>

using sc_checkpoint::Subdirectories;
using unittest::ScratchDirectory;

BOOST_GLOBAL_FIXTURE(ScratchDirectory);
BOOST_AUTO_TEST_SUITE(TestSubdirectories);

BOOST_AUTO_TEST_CASE(TestOrder) {
    std::string top_dir = ScratchDirectory::create_directory("SubdirsOrder");
    ScratchDirectory::create_directory(
        (path("SubdirsOrder") / path("1")).string());
    ScratchDirectory::create_directory(
        (path("SubdirsOrder") / path("2")).string());
    ScratchDirectory::create_directory(
        (path("SubdirsOrder") / path("3")).string());

    std::vector<std::string> expected_subdirs;
    expected_subdirs.push_back("1");
    expected_subdirs.push_back("2");
    expected_subdirs.push_back("3");

    Subdirectories subdirs(top_dir);
    BOOST_CHECK_EQUAL_COLLECTIONS(
        subdirs.begin(), subdirs.end(),
        expected_subdirs.begin(), expected_subdirs.end());
}

BOOST_AUTO_TEST_CASE(TestNonRecursive) {
    std::string top_dir = ScratchDirectory::create_directory("Top");
    std::string sub_dir = ScratchDirectory::create_directory(
        (path("Top") / path("Sub")).string());

    ScratchDirectory::create_directory(
        (path("Top") / path("Sub") / path("SubSub")).string());
    ScratchDirectory::create_file(
        (path("Top") / path("Sub") / path("UsrState")).string());
    ScratchDirectory::create_directory(
        (path("Top") / path("Sub2")).string());

    Subdirectories subdirs(top_dir);
    BOOST_CHECK_EQUAL(subdirs.size(), static_cast<size_t>(2));
    BOOST_CHECK(
        std::find(subdirs.begin(), subdirs.end(), "Sub") != subdirs.end());
    BOOST_CHECK(
        std::find(subdirs.begin(), subdirs.end(), "Sub2") != subdirs.end());
    BOOST_CHECK(
        std::find(subdirs.begin(), subdirs.end(), "SubSub") == subdirs.end());
    BOOST_CHECK(
        std::find(subdirs.begin(), subdirs.end(), "UsrState") == subdirs.end());
}

BOOST_AUTO_TEST_CASE(TestNonExistent) {
    Subdirectories subdirs("");
    BOOST_CHECK_EQUAL(subdirs.size(), static_cast<size_t>(0));
}

BOOST_AUTO_TEST_CASE(TestNonDirectory) {
    Subdirectories subdirs(ScratchDirectory::filename("NotADirectory"));
    BOOST_CHECK_EQUAL(subdirs.size(), static_cast<size_t>(0));
}

BOOST_AUTO_TEST_SUITE_END()
