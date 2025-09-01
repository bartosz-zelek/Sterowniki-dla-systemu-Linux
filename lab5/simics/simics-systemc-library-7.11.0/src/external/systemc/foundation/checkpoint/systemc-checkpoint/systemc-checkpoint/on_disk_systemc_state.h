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

#ifndef SYSTEMC_CHECKPOINT_ON_DISK_SYSTEMC_STATE_H
#define SYSTEMC_CHECKPOINT_ON_DISK_SYSTEMC_STATE_H

#include <systemc-checkpoint/serialize_callback_interface.h>

#include <string>
#include <vector>

namespace sc_checkpoint {

template <typename RegistryImpl, typename KernelImpl>
class OnDiskSystemCState {
  public:
    OnDiskSystemCState(RegistryImpl *serializer_registry,
                       KernelImpl *kernel,
                       SerializeCallbackInterface *notify_callbacks,
                       bool track_kernel_state = true);
    virtual ~OnDiskSystemCState();

    virtual bool write(const std::string &dir, bool persistent);
    virtual bool read(const std::vector<std::string> &dirs, bool persistent);

  private:
    RegistryImpl *serializer_registry_;
    KernelImpl *kernel_;
    SerializeCallbackInterface *notify_callbacks_;
    bool track_kernel_state_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_ON_DISK_SYSTEMC_STATE_H
