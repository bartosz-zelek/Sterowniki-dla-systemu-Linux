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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_VALARRAY_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_VALARRAY_H

#include <systemc-checkpoint/serialization/array.h>
#include <systemc-checkpoint/serialization/serializable_meta_data.h>

#include <cci/serialization/split_free.hpp>

#include <cassert>
#include <valarray>

namespace cci {
namespace serialization {

template <class Archive, class T>
void save(Archive &archive, const std::valarray<T> &v,  // NOLINT
          const unsigned int version) {
    const std::size_t size = v.size();
    archive << sc_checkpoint::serialization::create_smd("size", size);
    if (size > 0) {
        archive << sc_checkpoint::serialization::create_smd(
            "array", sc_checkpoint::serialization::make_array(&v[0], size));
    }
}

template <class Archive, class T>
void load(Archive &archive, std::valarray<T> &v,  // NOLINT
          const unsigned int version) {
    std::size_t size;
    archive >> sc_checkpoint::serialization::create_smd("size", size);
    assert(v.size() == size);
    if (size > 0) {
        archive >> sc_checkpoint::serialization::create_smd(
            "array", sc_checkpoint::serialization::make_array(&v[0], size));
    }
}

template <class Archive, class T>
void serialize(Archive &archive, std::valarray<T> &v,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, v, version);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_VALARRAY_H
