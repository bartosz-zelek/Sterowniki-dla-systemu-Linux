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

#ifndef SYSTEMC_CHECKPOINT_STATE_KEEPER_CHECKPOINTS_H
#define SYSTEMC_CHECKPOINT_STATE_KEEPER_CHECKPOINTS_H

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::path;
using std::filesystem::is_directory;
using std::filesystem::exists;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
using std::experimental::filesystem::is_directory;
using std::experimental::filesystem::exists;
#endif

#include <systemc-checkpoint/subdirectories.h>

#include <string>
#include <vector>

namespace sc_checkpoint {

class StateKeeperCheckpoints : public std::vector<std::string> {
  public:
    explicit StateKeeperCheckpoints(const std::vector<std::string> sc_chkpts) {
        std::vector<std::string>::const_iterator sc_chkpt_it;
        for (sc_chkpt_it = sc_chkpts.begin(); sc_chkpt_it != sc_chkpts.end();
             ++sc_chkpt_it) {
            Subdirectories sk_chkpts(*sc_chkpt_it);
            for (Subdirectories::const_iterator sk_chkpt_it = sk_chkpts.begin();
                 sk_chkpt_it != sk_chkpts.end(); ++sk_chkpt_it) {
                path sk_chkpt_path = path(*sc_chkpt_it) / path(*sk_chkpt_it);
                if (exists(sk_chkpt_path) && is_directory(sk_chkpt_path)) {
                    push_back(sk_chkpt_path.string());
                }
            }
        }
    }
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_STATE_KEEPER_CHECKPOINTS_H
