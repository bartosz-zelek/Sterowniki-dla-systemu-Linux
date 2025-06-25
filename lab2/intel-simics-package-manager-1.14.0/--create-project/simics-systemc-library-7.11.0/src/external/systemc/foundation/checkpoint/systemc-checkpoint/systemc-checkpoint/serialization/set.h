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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SET_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SET_H

#include <systemc-checkpoint/container.h>

#include <cci/serialization/split_free.hpp>

#include <set>

namespace cci {
namespace serialization {

template <class Archive, class T, class Compare, class Allocator>
void save(Archive &archive,  // NOLINT
          const std::set<T, Compare, Allocator> &s,
          const unsigned int version) {
    sc_checkpoint::save_container(archive, s, s.size());
}

template <class Archive, class T, class Compare, class Allocator>
void load(Archive &archive, std::set<T, Compare, Allocator> &s,  // NOLINT
          const unsigned int version) {
    sc_checkpoint::load_container<
        Archive, std::set<T, Compare, Allocator>,
        sc_checkpoint::LoadElementSet<
            Archive, std::set<T, Compare, Allocator> > >(archive, s);
}

// Split up serialize into separate save and load functions
template <class Archive, class T, class Compare, class Allocator>
void serialize(Archive &archive, std::set<T, Compare, Allocator> &s,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, s, version);
}

template <class Archive, class T, class Compare, class Allocator>
void save(Archive &archive,  // NOLINT
          const std::multiset<T, Compare, Allocator> &s,
          const unsigned int version) {
    sc_checkpoint::save_container(archive, s, s.size());
}

template <class Archive, class T, class Compare, class Allocator>
void load(Archive &archive, std::multiset<T, Compare, Allocator> &s,  // NOLINT
          const unsigned int version) {
    sc_checkpoint::load_container<
        Archive, std::multiset<T, Compare, Allocator>,
        sc_checkpoint::LoadElementSet<
            Archive, std::multiset<T, Compare, Allocator> > >(archive, s);
}

// Split up serialize into separate save and load functions
template <class Archive, class T, class Compare, class Allocator>
void serialize(Archive &archive, std::multiset<T, Compare, Allocator> &s,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, s, version);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_SET_H
