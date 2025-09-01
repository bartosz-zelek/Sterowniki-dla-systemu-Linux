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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZE_CALLBACK_INTERFACE_H
#define SYSTEMC_CHECKPOINT_SERIALIZE_CALLBACK_INTERFACE_H

namespace sc_checkpoint {

/**
 * This interface may be implemented by SystemC modules to register callbacks
 * that are invoked before and after any serialization functions are
 * called. This can be useful in order to support flashing of caches or prepare
 * the execution of threads before they're restarted.
 */

// <serialize-callback-interface>
class SerializeCallbackInterface {
  public:
    enum State {
        State_Saving = 0x1,
        State_Loading,
        State_Persistent = 0x10,
        State_Saving_Persistent = State_Saving | State_Persistent,
        State_Loading_Persistent = State_Loading | State_Persistent
    };

    virtual ~SerializeCallbackInterface() {}

    /**
     * Invoked before any save/load/serialize function is called and before any
     * state keepers are invoked.
     */
    virtual void pre_serialize_callback(State state) = 0;

    /**
     * Invoked after any save/load/serialize function has been called, but
     * before any threads have been restarted.
     */
    virtual void post_serialize_callback(State state) = 0;
};
// </serialize-callback-interface>

}  // namespace sc_checkpoint

#endif
