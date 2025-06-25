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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FIFO_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FIFO_H

#include <cci/serialization/split_free.hpp>

#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/vector.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <systemc>

#include <vector>

namespace cci {
namespace serialization {

template<class T, class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      sc_core::sc_fifo<T> &fifo,  // NOLINT
                      const unsigned int file_version) {
    split_free(ar, fifo, file_version);
}

template <class T, class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_core::sc_fifo<T> &fifo,  // NOLINT
          const unsigned int version) {
    std::vector<T> fifo_vector;
    ar & SMD(fifo_vector);
    sc_core::simContextProxyCheckpoint::set_sc_fifo_buf(&fifo, fifo_vector);
}

template <class T, class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_core::sc_fifo<T> &fifo, const unsigned int version) {
    std::vector<T> fifo_vector =
        sc_core::simContextProxyCheckpoint::sc_fifo_buf(&fifo);
    ar & SMD(fifo_vector);
}

}  // namespace serialization
}  // namespace cci

#endif
