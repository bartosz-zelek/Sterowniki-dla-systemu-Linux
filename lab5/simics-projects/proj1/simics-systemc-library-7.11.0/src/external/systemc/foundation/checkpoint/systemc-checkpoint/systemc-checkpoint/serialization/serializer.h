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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZER_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZER_H

#include <systemc-checkpoint/serialization/serializer_interface.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <systemc>
#include <typeinfo>

namespace sc_checkpoint {
namespace serialization {

template <class T>
class Serializer : public SerializerInterface {
  public:
    Serializer() {
        SerializerRegistry::Instance()->registerSerializer(this);
    }
    virtual ~Serializer() {
        SerializerRegistry::Instance()->unregisterSerializer(this);
    }
    virtual bool isSerializable(sc_core::sc_object *object) {
        return dynamic_cast<T *> (object) != NULL;
    }
    virtual void save(OArchive &oa,  // NOLINT
                      sc_core::sc_object *object) {
        T *t = static_cast<T *> (object);
        oa & sc_checkpoint::serialization::create_smd(typeid(T).name(), *t);
    }
    virtual void load(IArchive &ia,  // NOLINT
                      sc_core::sc_object *object) {
        T *t = static_cast<T *> (object);
        ia & sc_checkpoint::serialization::create_smd(typeid(T).name(), *t);
    }
};

}  // namespace serialization
}  // namespace sc_checkpoint

#endif
