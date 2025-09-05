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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_VECTOR_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_VECTOR_H

#include <systemc-checkpoint/container.h>
#include <systemc-checkpoint/serialization/list.h>
#include <systemc-checkpoint/serialization/serializable_meta_data.h>

#include <cci/serialization/string.hpp>
#include <cci/serialization/split_free.hpp>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>

namespace cci {
namespace serialization {

template <class Archive, class U, class Allocator>
void save(Archive &archive, const std::vector<U, Allocator> &v,  // NOLINT
          const unsigned int version) {
    sc_checkpoint::save_container(archive, v, v.size());
}

template <class Archive, class U, class Allocator>
void load(Archive &archive, std::vector<U, Allocator> &v,  // NOLINT
          const unsigned int version) {
    sc_checkpoint::load_container<
        Archive, std::vector<U, Allocator>,
        sc_checkpoint::LoadElementSequence<
            Archive, std::vector<U, Allocator> > >(
                archive, v);
}

// Split up serialize into separate save and load functions
template <class Archive, class U, class Allocator>
void serialize(Archive &archive, std::vector<U, Allocator> &v,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, v, version);
}

// std::vector<bool> is specialized: dereferencing iterators does not return
// references to boolean elements. Therefore, a specialization is in order
template <class Archive, class Allocator>
void save(Archive &archive, const std::vector<bool, Allocator> &v,  // NOLINT
          const unsigned int version) {
    std::ostringstream ss;
    std::copy(v.begin(), v.end(), std::ostream_iterator<bool>(ss, ""));
    const std::string bits(ss.str());
    archive << sc_checkpoint::serialization::create_smd(
        "std::vector<bool>", bits);
}

template <class Archive, class Allocator>
void load(Archive &archive, std::vector<bool, Allocator> &v,  // NOLINT
          const unsigned int version) {
    v.clear();

    std::string bits;
    archive >> sc_checkpoint::serialization::create_smd(
        "std::vector<bool>", bits);

    for (std::string::iterator it = bits.begin(); it != bits.end(); ++it) {
        if (*it == '0') {
            v.push_back(false);
        } else {
            v.push_back(true);
        }
    }
}

template <class Archive, class Allocator>
void serialize(Archive &archive, std::vector<bool, Allocator> &v,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, v, version);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_VECTOR_H
