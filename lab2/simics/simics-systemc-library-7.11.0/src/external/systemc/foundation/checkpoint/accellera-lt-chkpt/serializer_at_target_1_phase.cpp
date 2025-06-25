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

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/serialization/serializer.h>

#include "at_target_1_phase.h"
#include "serializer_memory.cpp"

using sc_checkpoint::serialization::Serializer;

struct at_target_1_phase::checkpoint_access {
    template <class Archive>
    static void serialize(Archive& ar,   // NOLINT(runtime/references)
                          at_target_1_phase &module,
                          const unsigned int version) {
        ar & SMD(module.m_target_memory);
    }
};

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive& ar,   // NOLINT(runtime/references)
               at_target_1_phase &module, const unsigned int version) {
    at_target_1_phase::checkpoint_access::serialize(ar, module, version);
}

}  // namespace serialization
}  // namespace cci

static Serializer<at_target_1_phase> at_target_1_phase_serializer;
