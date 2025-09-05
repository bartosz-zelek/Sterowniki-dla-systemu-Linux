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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_PEQ_WITH_GET_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_PEQ_WITH_GET_H

#include <cci/serialization/split_free.hpp>

#include <tlm>
#include <tlm_utils/peq_with_get.h>

#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serialization/sc_time.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/vector.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <map>
#include <utility>
#include <vector>

namespace cci {
namespace serialization {

template<class T, class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      tlm_utils::peq_with_get<T> &peq,  // NOLINT
                      const unsigned int file_version) {
    split_free(ar, peq, file_version);
}

template <class T, class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          tlm_utils::peq_with_get<T> &peq,  // NOLINT
          const unsigned int version) {
    std::size_t size = 0;
    std::vector<sc_core::sc_time> times;
    std::vector<T*> payloads;
    ar & SMD(size);
    ar & SMD(times);
    ar & SMD(payloads);
    std::multimap<const sc_core::sc_time, T*> events;

    for (std::size_t i = 0; i < size; ++i)
        events.insert(std::make_pair(times[i], payloads[i]));

    sc_core::simContextProxyCheckpoint::set_peq_with_get_scheduled_events(
        &peq, events);
}

template <class T, class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const tlm_utils::peq_with_get<T> &peq, const unsigned int version) {
    std::multimap<const sc_core::sc_time, T*> events =
        sc_core::simContextProxyCheckpoint::peq_with_get_scheduled_events(&peq);
    std::vector<sc_core::sc_time> times;
    std::vector<T*> payloads;
    std::size_t size = events.size();

    typename std::multimap<const sc_core::sc_time, T*>::iterator i;
    for (i = events.begin(); i != events.end(); ++i) {
        times.push_back(i->first);
        payloads.push_back(i->second);
    }

    ar & SMD(size);
    ar & SMD(times);
    ar & SMD(payloads);
}

}  // namespace serialization
}  // namespace cci

#endif
