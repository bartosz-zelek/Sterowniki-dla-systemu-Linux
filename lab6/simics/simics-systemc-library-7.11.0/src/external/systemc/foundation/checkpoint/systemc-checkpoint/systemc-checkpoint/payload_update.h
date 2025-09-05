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

#ifndef SYSTEMC_CHECKPOINT_PAYLOAD_UPDATE_H
#define SYSTEMC_CHECKPOINT_PAYLOAD_UPDATE_H

#include <systemc-checkpoint/payload_update_interface.h>
#include <systemc-checkpoint/payload_update_registry.h>

#include <systemc>
#include <tlm>

namespace sc_checkpoint {

// <payload-update>
template <class T>
class PayloadUpdate : public PayloadUpdateInterface {
  public:
    PayloadUpdate() {
        PayloadUpdateRegistry::Instance()->registerUpdater(this);
    }
    virtual ~PayloadUpdate() {
        PayloadUpdateRegistry::Instance()->unregisterUpdater(this);
    }

    virtual void set_data_ptr(T *t, tlm::tlm_generic_payload *payload) {}
    virtual void set_byte_enable_ptr(T *t,
                                     tlm::tlm_generic_payload *payload) {}
    virtual void set_extensions(T *t, tlm::tlm_generic_payload *payload) {}
    virtual void set_memory_manager(T *t, tlm::tlm_generic_payload *payload) {}

  private:
    virtual bool isUpdatable(sc_core::sc_object *object) {
        return dynamic_cast<T *> (object) != NULL;
    }
    virtual void updateDataPtr(sc_core::sc_object *object,
                               tlm::tlm_generic_payload *payload) {
        set_data_ptr(static_cast<T *>(object), payload);
    }
    virtual void updateByteEnablePtr(sc_core::sc_object *object,
                                     tlm::tlm_generic_payload *payload) {
        set_byte_enable_ptr(static_cast<T *>(object), payload);
    }
    virtual void updateExtensions(sc_core::sc_object *object,
                                  tlm::tlm_generic_payload *payload) {
        set_extensions(static_cast<T *>(object), payload);
    }
    virtual void updateMemoryManager(sc_core::sc_object *object,
                                     tlm::tlm_generic_payload *payload) {
        set_memory_manager(static_cast<T *>(object), payload);
    }
};
// </payload-update>

}  // namespace sc_checkpoint

#endif
