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

#ifndef SYSTEMC_CHECKPOINT_CHECKPOINT_CONTROL_H
#define SYSTEMC_CHECKPOINT_CHECKPOINT_CONTROL_H

#include <systemc-checkpoint/in_memory_state.h>
#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/notify_callbacks.h>
#include <systemc-checkpoint/on_disk_state.h>
#include <systemc-checkpoint/serializer_registry.h>
#include <systemc-checkpoint/state_keeper.h>

namespace sc_checkpoint {

class CheckpointControl
    : public InMemoryState<SerializerRegistry, Kernel>,
      public OnDiskState {
  public:
    CheckpointControl()
        : InMemoryState<SerializerRegistry, Kernel>(
            &state_keeper_, &notify_callbacks_, SerializerRegistry::Instance(),
            &kernel_),
        OnDiskState(SerializerRegistry::Instance(), &kernel_, &state_keeper_,
                    &notify_callbacks_) {}

  protected:
    Kernel kernel_;

  private:
    StateKeeper state_keeper_;
    NotifyCallbacks notify_callbacks_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_CHECKPOINT_CONTROL_H
