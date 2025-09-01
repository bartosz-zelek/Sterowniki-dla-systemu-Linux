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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_QUEUE_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_QUEUE_H

#include <systemc-checkpoint/expose_container.h>
#include <systemc-checkpoint/serialization/deque.h>
#include <systemc-checkpoint/serialization/list.h>
#include <systemc-checkpoint/serialization/vector.h>

#include <cci/serialization/split_free.hpp>

#include <queue>

namespace cci {
namespace serialization {

template <class Archive, class T, class Container>
void save(Archive &archive,  // NOLINT
          const std::queue<T, Container> &s,
          const unsigned int version) {
    save(archive, sc_checkpoint::containerOf(s), version);
}

template <class Archive, class T, class Container>
void load(Archive &archive, std::queue<T, Container> &s,  // NOLINT
          const unsigned int version) {
    load(archive, sc_checkpoint::containerOf(s), version);
}

template <class Archive, class T, class Container>
void serialize(Archive &archive, std::queue<T, Container> &s,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, s, version);
}

template <class Archive, class T, class Container, class Cmp>
void save(Archive &archive,  // NOLINT
          const std::priority_queue<T, Container, Cmp> &s,
          const unsigned int version) {
    save(archive, sc_checkpoint::containerOf(s), version);
}

template <class Archive, class T, class Container, class Cmp>
void load(Archive &archive, std::priority_queue<T, Container, Cmp> &s,  // NOLINT
          const unsigned int version) {
    load(archive, sc_checkpoint::containerOf(s), version);
}

template <class Archive, class T, class Container, class Cmp>
void serialize(Archive &archive, std::priority_queue<T, Container, Cmp> &s,  // NOLINT
               const unsigned int version) {
    cci::serialization::split_free(archive, s, version);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_QUEUE_H
