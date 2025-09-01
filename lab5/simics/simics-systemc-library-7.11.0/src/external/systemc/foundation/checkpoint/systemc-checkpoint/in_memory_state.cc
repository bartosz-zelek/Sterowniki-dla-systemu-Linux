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

#include <systemc-checkpoint/in_memory_state.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <set>

namespace sc_checkpoint {

template <class S, class K>
InMemoryState<S, K>::InMemoryState(StateKeeperInterface *state_keeper,
                                   SerializeCallbackInterface *notify_callbacks,
                                   S *state_registry, K *kernel,
                                   bool track_kernel_state) {
    state_keeper_ = state_keeper;
    notify_callbacks_ = notify_callbacks;
    state_registry_ = state_registry;
    kernel_ = kernel;
    track_kernel_state_ = track_kernel_state;
}

template <class S, class K>
InMemoryState<S, K>::~InMemoryState() {
    typename std::set<State *>::iterator i;
    for (i = states_.begin(); i != states_.end(); ++i)
        delete *i;
}

template <class S, class K>
bool InMemoryState<S, K>::save(void **handle) {
    State *state = new State();
    *handle = state;
    states_.insert(state);

    notify_callbacks_->pre_serialize_callback(
        SerializeCallbackInterface::State_Saving);

    if (!state_keeper_->save(&state->handle_))
        return false;

    OStateTree ostate_tree_(&state->ptree_);
    OArchive oarchive_(&ostate_tree_);
    oarchive_ & serialization::create_smd("modules", *state_registry_);

    notify_callbacks_->post_serialize_callback(
        SerializeCallbackInterface::State_Saving);

    if (track_kernel_state_) {
        oarchive_ & serialization::create_smd("kernel", *kernel_);
    }

    return true;
}

template <class S, class K>
bool InMemoryState<S, K>::restore(void *handle) {
    State *state = static_cast<State *>(handle);
    if (states_.find(state) == states_.end())
        return false;

    notify_callbacks_->pre_serialize_callback(
        SerializeCallbackInterface::State_Loading);

    state_keeper_->restore(state->handle_);

    IStateTree istate_tree_(&state->ptree_);
    IArchive iarchive_(&istate_tree_);
    iarchive_ & serialization::create_smd("modules", *state_registry_);

    notify_callbacks_->post_serialize_callback(
        SerializeCallbackInterface::State_Loading);

    if (track_kernel_state_) {
        kernel_->set_reverse_execution(true);
        iarchive_ & serialization::create_smd("kernel", *kernel_);
    }

    return true;
}

template <class S, class K>
bool InMemoryState<S, K>::merge(void *handlePrevious, void *handleRemove) {
    State *s1 = static_cast<State *>(handleRemove);
    if (states_.find(s1) == states_.end())
        return false;

    State *s0 = static_cast<State *>(handlePrevious);
    if (s0) {
        if (states_.find(s0) == states_.end())
            return false;
        if (!state_keeper_->merge(s0->handle_, s1->handle_))
            return false;
    } else {
        if (!state_keeper_->merge(NULL, s1->handle_))
            return false;
    }

    states_.erase(s1);
    delete s1;

    return true;
}

template
InMemoryState<SerializerRegistry, Kernel>::InMemoryState(
    StateKeeperInterface*, SerializeCallbackInterface *, SerializerRegistry*,
    Kernel*, bool);

template
InMemoryState<SerializerRegistry, Kernel>::~InMemoryState();

template
bool InMemoryState<SerializerRegistry, Kernel>::save(void **handle);

template
bool InMemoryState<SerializerRegistry, Kernel>::restore(void *handle);

}  // namespace sc_checkpoint
