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

#ifndef SYSTEMC_CHECKPOINT_STATE_KEEPER_H
#define SYSTEMC_CHECKPOINT_STATE_KEEPER_H

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/state_keeper_interface.h>

#include <systemc>

#include <boost/bimap.hpp>

#include <set>
#include <string>
#include <vector>

namespace sc_checkpoint {

class StateKeeper : public StateKeeperInterface {
  public:
    StateKeeper();
    virtual ~StateKeeper();
    virtual bool save(void **handleOut);
    virtual bool restore(void *handle);
    virtual bool merge(void *handlePrevious, void *handleRemove);

    virtual bool write(
        const std::string &dir, const std::string &prefix, bool persistent);
    virtual bool write_standalone(
        const std::string &dir, const std::string &prefix, bool persistent);
    virtual bool read(const std::vector<std::string> &dirs, const std::string,
                      bool persistent);
  private:
    AllScObjects *get_sc_objects();
    AllScObjects *sc_objects_;
    typedef boost::bimap<void *, StateKeeperInterface *> Handles;
    std::set<Handles *> all_handles_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_STATE_KEEPER_H
