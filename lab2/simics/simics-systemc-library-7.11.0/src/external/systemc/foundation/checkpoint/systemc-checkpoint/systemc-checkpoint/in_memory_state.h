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

#ifndef SYSTEMC_CHECKPOINT_IN_MEMORY_STATE_H
#define SYSTEMC_CHECKPOINT_IN_MEMORY_STATE_H

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/in_memory_state_interface.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialize_callback_interface.h>
#include <systemc-checkpoint/state_keeper_interface.h>

#include <boost/property_tree/ptree.hpp>

#include <set>

namespace sc_checkpoint {

template <class S, class K>
class InMemoryState : public InMemoryStateInterface {
  public:
    // TODO(enilsson): remove boolean once required kernel state is saved
    InMemoryState(StateKeeperInterface *state_keeper,
                  SerializeCallbackInterface *notify_callbacks,
                  S *state_registry, K *kernel, bool track_kernel_state = true);
    virtual ~InMemoryState();

    virtual bool save(void **handle);
    virtual bool restore(void *handle);
    virtual bool merge(void *handlePrevious, void *handleRemove);

  private:
    class State {
      public:
        State() : handle_(NULL) {}
        void *handle_;
        boost::property_tree::ptree ptree_;
    };
    StateKeeperInterface *state_keeper_;
    SerializeCallbackInterface *notify_callbacks_;
    S *state_registry_;
    K *kernel_;
    bool track_kernel_state_;
    std::set<State *> states_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_IN_MEMORY_STATE_H
