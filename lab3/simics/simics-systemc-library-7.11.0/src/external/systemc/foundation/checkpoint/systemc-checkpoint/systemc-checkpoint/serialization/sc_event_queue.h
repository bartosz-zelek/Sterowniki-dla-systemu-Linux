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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_EVENT_QUEUE_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_EVENT_QUEUE_H

#include <cci/serialization/serialization.hpp>

#include <systemc-checkpoint/serialization/sc_ppq.h>
#include <systemc-checkpoint/serialization/sc_time.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <systemc>

namespace cci {
namespace serialization {

template <class Archive>
void serialize(Archive &ar,   // NOLINT(runtime/references)
               sc_core::sc_event_queue &queue,  // NOLINT
               const unsigned int version) {
    ar & SMD(*sc_core::simContextProxyCheckpoint::event_queue_ppq(&queue));
}

}  // namespace serialization
}  // namespace cci

#endif
