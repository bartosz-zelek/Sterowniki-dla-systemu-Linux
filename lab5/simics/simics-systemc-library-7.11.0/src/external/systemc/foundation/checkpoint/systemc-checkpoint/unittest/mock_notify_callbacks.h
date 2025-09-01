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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_MOCK_NOTIFY_CALLBACKS_H
#define SYSTEMC_CHECKPOINT_UNITTEST_MOCK_NOTIFY_CALLBACKS_H

#include <systemc-checkpoint/serialize_callback_interface.h>

#include <cassert>

namespace unittest {

class MockNotifyCallbacks : public sc_checkpoint::SerializeCallbackInterface {
  public:
    enum CallbackArg {
        CallbackArg_NA,
        CallbackArg_SAVE,
        CallbackArg_LOAD
    };

    MockNotifyCallbacks() : pre_callback_arg_(CallbackArg_NA),
                            post_callback_arg_(CallbackArg_NA),
                            pre_callback_num_invocations_(0),
                            post_callback_num_invocations_(0) {}

    void pre_serialize_callback(State state) {
        assert(!(state & State_Persistent));
        pre_callback_num_invocations_++;
        pre_callback_arg_ = state == State_Saving
            ? CallbackArg_SAVE : CallbackArg_LOAD;
    }
    void post_serialize_callback(State state) {
        assert(!(state & State_Persistent));
        post_callback_num_invocations_++;
        post_callback_arg_ = state == State_Saving
            ? CallbackArg_SAVE : CallbackArg_LOAD;
    }

    CallbackArg pre_callback_arg_;
    CallbackArg post_callback_arg_;

    int pre_callback_num_invocations_;
    int post_callback_num_invocations_;
};

}  // namespace unittest

#endif
