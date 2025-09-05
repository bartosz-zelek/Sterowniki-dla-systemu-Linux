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

#ifndef SYSTEMC_CHECKPOINT_PAYLOAD_UPDATE_REGISTRY_H
#define SYSTEMC_CHECKPOINT_PAYLOAD_UPDATE_REGISTRY_H

#include <systemc-checkpoint/payload_update_interface.h>

#include <systemc>

#include <map>
#include <set>
#include <utility>

namespace sc_checkpoint {

class PayloadUpdateRegistry {
  public:
    enum Update {
        UPDATE_NO_UPDATE = 0,
        UPDATE_DATA_PTR = 1,
        UPDATE_BYTE_ENABLE_PTR = 2,
        UPDATE_EXTENSIONS = 4,
        UPDATE_MEMORY_MANAGER = 8
    };

    static PayloadUpdateRegistry *Instance();
    void registerUpdater(PayloadUpdateInterface *updater);
    void unregisterUpdater(PayloadUpdateInterface *updater);
    void update(sc_core::sc_object *object,
                tlm::tlm_generic_payload *payload,
                Update update);
    void update(sc_core::sc_object *object,
                tlm::tlm_generic_payload *payload);

  private:
    void dispatch(sc_core::sc_object *object,
                  tlm::tlm_generic_payload *payload,
                  Update update);
    PayloadUpdateRegistry();
    std::set<PayloadUpdateInterface *> updaters_;
    std::map<tlm::tlm_generic_payload *,
             std::pair<sc_core::sc_object *, Update> > cache_;
};

inline PayloadUpdateRegistry::Update operator|(
        PayloadUpdateRegistry::Update a, PayloadUpdateRegistry::Update b) {
    return static_cast<PayloadUpdateRegistry::Update>(static_cast<int>(a) |
                                                      static_cast<int>(b));
}

inline PayloadUpdateRegistry::Update &operator|=(
        PayloadUpdateRegistry::Update &a,  // NOLINT
        PayloadUpdateRegistry::Update b) {
    return a = a | b;
}

}  // namespace sc_checkpoint

#endif
