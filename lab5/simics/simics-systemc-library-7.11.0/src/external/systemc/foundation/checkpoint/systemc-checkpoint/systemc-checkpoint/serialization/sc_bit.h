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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_BIT_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_BIT_H

#include <cci/serialization/split_free.hpp>

#include <systemc-checkpoint/serialization/smd.h>

#include <systemc>

CCI_SERIALIZATION_SPLIT_FREE(sc_dt::sc_bit)

namespace cci {
namespace serialization {

template <class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_dt::sc_bit &bit,  // NOLINT(runtime/references)
          const unsigned int version) {
    bool bit_value = 0;
    ar & SMD(bit_value);
    bit = bit_value;
}

template <class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_dt::sc_bit &bit, const unsigned int version) {
    bool bit_value = bit;
    ar & SMD(bit_value);
}

}  // namespace serialization
}  // namespace cci

#endif
