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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_SIGNAL_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_SIGNAL_H

#include <cci/serialization/split_free.hpp>

#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <systemc>

namespace cci {
namespace serialization {

template<class T, sc_core::sc_writer_policy POL, class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      sc_core::sc_signal<T, POL> &signal,  // NOLINT
                      const unsigned int file_version) {
    split_free(ar, signal, file_version);
}

template <class T, sc_core::sc_writer_policy POL, class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_core::sc_signal<T, POL> &signal,  // NOLINT
          const unsigned int version) {
    T cur_val = T(0);
    T new_val = T(0);
    sc_dt::uint64 time_stamp = 0;
    ar & SMD(cur_val);
    ar & SMD(new_val);
    ar & SMD(time_stamp);

    sc_core::simContextProxyCheckpoint::set_sc_signal_cur_val(&signal, cur_val);
    sc_core::simContextProxyCheckpoint::set_sc_signal_new_val(&signal, new_val);
    sc_core::simContextProxyCheckpoint::set_sc_signal_change_stamp(&signal,
                                                                   time_stamp);
}

template <class T, sc_core::sc_writer_policy POL, class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_core::sc_signal<T, POL> &signal,
          const unsigned int version) {
    T cur_val = sc_core::simContextProxyCheckpoint::sc_signal_cur_val(&signal);
    T new_val = sc_core::simContextProxyCheckpoint::sc_signal_new_val(&signal);
    sc_dt::uint64 time_stamp =
        sc_core::simContextProxyCheckpoint::sc_signal_change_stamp(&signal);

    ar & SMD(cur_val);
    ar & SMD(new_val);
    ar & SMD(time_stamp);
}

}  // namespace serialization
}  // namespace cci

#endif
