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

#ifndef SYSTEMC_CHECKPOINT_SUBDIRECTORIES_H
#define SYSTEMC_CHECKPOINT_SUBDIRECTORIES_H

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::exists;
using std::filesystem::is_directory;
using std::filesystem::directory_iterator;
using std::filesystem::directory_entry;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::exists;
using std::experimental::filesystem::is_directory;
using std::experimental::filesystem::directory_iterator;
using std::experimental::filesystem::directory_entry;
#endif

#include <algorithm>
#include <string>
#include <vector>

namespace sc_checkpoint {

class Subdirectories : public std::vector<std::string> {
  public:
    explicit Subdirectories(const std::string &dir) {
        if (!exists(dir) || !is_directory(dir)) {
            return;
        }

        for (directory_iterator it(dir); it != directory_iterator(); ++it) {
            directory_entry entry = *it;
            push_back(entry.path().filename().string());
        }
        std::sort(begin(), end());
    }
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_SUBDIRECTORIES_H
