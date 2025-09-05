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

#include <systemc-checkpoint/payload_update_registry.h>

#include <map>
#include <set>
#include <utility>

namespace sc_checkpoint {

PayloadUpdateRegistry *PayloadUpdateRegistry::Instance() {
    static PayloadUpdateRegistry registry;
    return &registry;
}

void PayloadUpdateRegistry::registerUpdater(PayloadUpdateInterface *updater) {
    updaters_.insert(updater);
}

void PayloadUpdateRegistry::unregisterUpdater(
        PayloadUpdateInterface *updater) {
    updaters_.erase(updater);
}

void PayloadUpdateRegistry::update(sc_core::sc_object *object,
                                   tlm::tlm_generic_payload *payload,
                                   Update update) {
    if (!object || !payload)
        return;

    cache_[payload] = std::make_pair(object, update);
    if (update == UPDATE_NO_UPDATE)
        return;

    dispatch(object, payload, update);
}

void PayloadUpdateRegistry::update(sc_core::sc_object *object,
                                   tlm::tlm_generic_payload *payload) {
    std::map<tlm::tlm_generic_payload *,
             std::pair<sc_core::sc_object *, Update> >::iterator i;
    i = cache_.find(payload);
    if (i == cache_.end())
        return;

    if (i->second.first == object)
        return;

    dispatch(object, payload, i->second.second);
}

void PayloadUpdateRegistry::dispatch(sc_core::sc_object *object,
                                     tlm::tlm_generic_payload *payload,
                                     Update update) {
    std::set<PayloadUpdateInterface *>::iterator i;
    for (i = updaters_.begin(); i != updaters_.end(); ++i) {
        if (!(*i)->isUpdatable(object))
            continue;

        if (update & UPDATE_DATA_PTR)
            (*i)->updateDataPtr(object, payload);

        if (update & UPDATE_BYTE_ENABLE_PTR)
            (*i)->updateByteEnablePtr(object, payload);

        if (update & UPDATE_EXTENSIONS)
            (*i)->updateExtensions(object, payload);

        if (update & UPDATE_MEMORY_MANAGER)
            (*i)->updateMemoryManager(object, payload);
    }
}

PayloadUpdateRegistry::PayloadUpdateRegistry() {
}

}  // namespace sc_checkpoint
