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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_UINT_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_UINT_H

#include <cci/serialization/split_free.hpp>

#include <stdint.h>
#include <systemc>

#include <systemc-checkpoint/serialization/smd.h>

namespace cci {
namespace serialization {

template<int W, class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      sc_dt::sc_uint<W> &value,  // NOLINT
                      const unsigned int file_version) {
    split_free(ar, value, file_version);
}

template <int W, class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_dt::sc_uint<W> &value,  // NOLINT
          const unsigned int version) {
    uint64_t int_value = 0;
    ar & SMD(int_value);
    value = int_value;
}

template <int W, class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_dt::sc_uint<W> &value, const unsigned int version) {
    uint64_t int_value = value.value();
    ar & SMD(int_value);
}

}  // namespace serialization
}  // namespace cci

#endif
