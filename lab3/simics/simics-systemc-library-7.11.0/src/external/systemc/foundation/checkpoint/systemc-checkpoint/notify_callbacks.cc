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

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/notify_callbacks.h>

#include <cassert>

namespace {
bool implementsSerializeCallbackInterface(const sc_core::sc_object *object) {
    return dynamic_cast<
        const sc_checkpoint::SerializeCallbackInterface *>(object) != NULL;
}
}  // namespace

namespace sc_checkpoint {

NotifyCallbacks::NotifyCallbacks() : callbacks_(NULL) {}
NotifyCallbacks::~NotifyCallbacks() {
    delete callbacks_;
}

void NotifyCallbacks::pre_serialize_callback(State state) {
    if (!callbacks_) {
        callbacks_ = new AllScObjects(implementsSerializeCallbackInterface);
    }

    for (AllScObjects::iterator it = callbacks_->begin();
         it != callbacks_->end(); ++it) {
        SerializeCallbackInterface *callback = dynamic_cast<
            SerializeCallbackInterface *>(*it);
        assert(callback);
        callback->pre_serialize_callback(state);
    }
}

void NotifyCallbacks::post_serialize_callback(State state) {
    if (!callbacks_) {
        callbacks_ = new AllScObjects(implementsSerializeCallbackInterface);
    }

    for (AllScObjects::iterator it = callbacks_->begin();
         it != callbacks_->end(); ++it) {
        SerializeCallbackInterface *callback = dynamic_cast<
            SerializeCallbackInterface *>(*it);
        assert(callback);
        callback->post_serialize_callback(state);
    }
}

}  // namespace sc_checkpoint
