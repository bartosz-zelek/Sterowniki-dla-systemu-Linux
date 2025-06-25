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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZER_REGISTRY_H
#define SYSTEMC_CHECKPOINT_SERIALIZER_REGISTRY_H

#include <systemc-checkpoint/serialization/serializer_interface.h>

#include <cci/serialization/split_member.hpp>
#include <systemc>

#include <set>

namespace sc_checkpoint {

class SerializerRegistry {
  public:
    static SerializerRegistry *Instance();
    void registerSerializer(serialization::SerializerInterface *serializer);
    void unregisterSerializer(serialization::SerializerInterface *serializer);
    template<class Archive>
    void save(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version) const;
    template<class Archive>
    void load(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version);
    CCI_SERIALIZATION_SPLIT_MEMBER()
    sc_core::sc_object *object_under_serialization();

  private:
    SerializerRegistry();
    std::set<serialization::SerializerInterface *> findSerializers(
        sc_core::sc_object *object) const;
    std::set<serialization::SerializerInterface *> serializers_;
    mutable sc_core::sc_object *object_under_serialization_;
};

}  // namespace sc_checkpoint

#endif
