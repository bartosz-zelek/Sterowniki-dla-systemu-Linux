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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_MOCK_STATE_KEEPER_H
#define SYSTEMC_CHECKPOINT_UNITTEST_MOCK_STATE_KEEPER_H

#include <systemc-checkpoint/state_keeper_interface.h>

#include <string>
#include <vector>

namespace unittest {

class MockStateKeeper : public sc_checkpoint::StateKeeperInterface {
  public:
    MockStateKeeper()
        : save_handle_(0), save_called_(0), restore_called_(0),
          write_called_(0), write_standalone_called_(0), read_called_(0),
          restore_handle_(NULL), merge_handle_previous_(NULL),
          merge_handle_remove_(NULL) {}
    virtual bool save(void **handleOut) {
        ++save_called_;
        *handleOut = reinterpret_cast<void *> (++save_handle_);
        return true;
    }
    virtual bool restore(void *handle) {
        ++restore_called_;
        restore_handle_ = handle;
        return true;
    }
    virtual bool merge(void *handlePrevious, void *handleRemove)  {
        merge_handle_previous_ = handlePrevious;
        merge_handle_remove_ = handleRemove;
        return true;
    }
    virtual bool write(
        const std::string &dir, const std::string &prefix, bool persistent) {
        ++write_called_;
        write_dir_ = dir;
        return true;
    }
    virtual bool write_standalone(
        const std::string &dir, const std::string &prefix, bool persistent) {
        ++write_standalone_called_;
        write_dir_ = dir;
        return true;
    }
    virtual bool read(const std::vector<std::string> &dirs,
                      const std::string prefix, bool persistent) {
        ++read_called_;
        read_dirs_ = dirs;
        return true;
    }

    int save_handle_;
    int save_called_;
    int restore_called_;
    int write_called_;
    int write_standalone_called_;
    int read_called_;
    void *restore_handle_;
    void *merge_handle_previous_;
    void *merge_handle_remove_;
    std::string write_dir_;
    std::vector<std::string> read_dirs_;
};

}  // namespace unittest

#endif
