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

#include <systemc-checkpoint/on_disk_state.h>
#include <systemc-checkpoint/state_keeper_checkpoints.h>

#include <iostream>
#include <string>
#include <vector>

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#include <filesystem>  // NOLINT(build/c++17)
using std::filesystem::path;
using std::filesystem::create_directories;
using std::filesystem::exists;
#else
#include <experimental/filesystem>
using std::experimental::filesystem::path;
using std::experimental::filesystem::create_directories;
using std::experimental::filesystem::exists;
#endif

using std::string;
using std::vector;

namespace sc_checkpoint {

OnDiskState::OnDiskState(SerializerRegistry *serializer_registry,
                         Kernel *kernel,
                         StateKeeperInterface *state_keeper,
                         SerializeCallbackInterface *notify_callbacks,
                         bool track_kernel_state)
    : state_keeper_(state_keeper), notify_callbacks_(notify_callbacks),
      sc_state(serializer_registry, kernel,
               notify_callbacks, track_kernel_state) {}

bool OnDiskState::write(const std::string &dir, bool persistent) {
    return write(dir, false, persistent);
}

bool OnDiskState::write_standalone(const std::string &dir, bool persistent) {
    return write(dir, true, persistent);
}

bool OnDiskState::read(const vector<string> &dirs, bool persistent) {
    if (dirs.size() == 0) {
        return false;
    }

    notify_callbacks_->pre_serialize_callback(
        SerializeCallbackInterface::State_Loading);

    const StateKeeperCheckpoints sk_chkpts(dirs);
    if (sk_chkpts.size() == 0
        || !state_keeper_->read(sk_chkpts, "", persistent)) {
        return false;
    }

    return sc_state.read(dirs, persistent);
}

bool OnDiskState::write(const std::string &dir,
                        bool complete, bool persistent) {
    if (dir == "") {
        return false;
    }

    if (exists(path(dir))) {
        std::cerr << dir << " already exists!" << std::endl;
        return false;
    }

    path sk_chkpt = path(dir) / path("1");
    create_directories(sk_chkpt);

    notify_callbacks_->pre_serialize_callback(
        SerializeCallbackInterface::State_Saving);

    bool success = complete
        ? state_keeper_->write_standalone(sk_chkpt.string(), "", persistent)
        : state_keeper_->write(sk_chkpt.string(), "", persistent);
    if (!success) {
        return false;
    }

    // TODO(enilsson): currently on disk SystemC state is always full; write
    // partial disk checkpoints when we support it in the future
    return sc_state.write(dir, persistent);
}

}  // namespace sc_checkpoint
