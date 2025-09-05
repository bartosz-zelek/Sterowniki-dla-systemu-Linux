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

#ifndef SYSTEMC_CHECKPOINT_ON_DISK_STATE_H
#define SYSTEMC_CHECKPOINT_ON_DISK_STATE_H

#include <systemc-checkpoint/kernel.h>
#include <systemc-checkpoint/on_disk_state_interface.h>
#include <systemc-checkpoint/on_disk_systemc_state.h>
#include <systemc-checkpoint/serialize_callback_interface.h>
#include <systemc-checkpoint/serializer_registry.h>
#include <systemc-checkpoint/state_keeper_interface.h>

#include <string>
#include <vector>

namespace sc_checkpoint {

class OnDiskState : public OnDiskStateInterface {
  public:
    OnDiskState(SerializerRegistry *serializer_registry,
                Kernel *kernel,
                StateKeeperInterface *state_keeper,
                SerializeCallbackInterface *notify_callbacks,
                bool track_kernel_state = true);

    virtual bool write(const std::string &dir, bool persistent);
    virtual bool write_standalone(const std::string &dir, bool persistent);
    virtual bool read(const std::vector<std::string> &dirs, bool persistent);

  private:
    bool write(const std::string &dir, bool complete, bool persistent);

    StateKeeperInterface *state_keeper_;
    SerializeCallbackInterface *notify_callbacks_;
    OnDiskSystemCState<SerializerRegistry, Kernel> sc_state;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_ON_DISK_STATE_H
