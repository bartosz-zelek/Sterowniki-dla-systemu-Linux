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

#ifndef SYSTEMC_CHECKPOINT_NOTIFY_CALLBACKS_H
#define SYSTEMC_CHECKPOINT_NOTIFY_CALLBACKS_H

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/serialize_callback_interface.h>

namespace sc_checkpoint {

class NotifyCallbacks : public SerializeCallbackInterface {
  public:
    NotifyCallbacks();
    ~NotifyCallbacks();

    void pre_serialize_callback(State state);
    void post_serialize_callback(State state);

  private:
    AllScObjects *callbacks_;
};

}  // namespace sc_checkpoint

#endif
