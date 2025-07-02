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
using std::filesystem::remove_all;
using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::exists;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::remove_all;
using std::experimental::filesystem::path;
using std::experimental::filesystem::create_directories;
using std::experimental::filesystem::exists;
#endif

#include <boost/test/unit_test.hpp>

#include <unittest/scratch_directory.h>

#include <fstream>
#include <iostream>

namespace unittest {

ScratchDirectory::ScratchDirectory() {
    if (exists(scratchDirPath)) {
        remove_all(scratchDirPath.c_str());
    }
    create_directories(scratchDirPath.c_str());
}

std::string ScratchDirectory::create_directory(std::string name) {
    std::string directory = filename(name);
    create_directories(directory);
    return directory;
}

std::string ScratchDirectory::create_file(std::string name) {
    std::string file = filename(name);
    std::ofstream(file.c_str());
    return file;
}

std::string ScratchDirectory::filename(std::string name) {
    return (path(scratchDirPath) / path(name)).string();
}

const std::string ScratchDirectory::scratchDirPath = "scratch";  // NOLINT

}  // namespace unittest
