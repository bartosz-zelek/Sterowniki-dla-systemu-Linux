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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_BITSET_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_BITSET_H

#include <cci/serialization/split_free.hpp>
#include <cci/serialization/string.hpp>

#include <systemc-checkpoint/serialization/serializable_meta_data.h>

#include <bitset>
#include <string>

namespace cci {
namespace serialization {

template <class Archive, std::size_t N>
void save(Archive &archive,  // NOLINT
          const std::bitset<N> &b,
          const unsigned int version) {
    const std::string bitset = b.to_string();
    archive << sc_checkpoint::serialization::create_smd("std::bitset", bitset);
}

template <class Archive, std::size_t N>
void load(Archive &archive, std::bitset<N> &b,  // NOLINT
          const unsigned int version) {
    std::string bitset;
    archive >> sc_checkpoint::serialization::create_smd("std::bitset", bitset);
    b = std::bitset<N>(bitset);
}

template <class Archive, std::size_t N>
void serialize(Archive &archive, std::bitset<N> &b,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, b, version);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_BITSET_H
