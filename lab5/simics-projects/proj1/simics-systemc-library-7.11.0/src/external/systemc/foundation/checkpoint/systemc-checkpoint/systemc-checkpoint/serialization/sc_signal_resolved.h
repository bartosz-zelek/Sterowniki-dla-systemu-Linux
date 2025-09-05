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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_SIGNAL_RESOLVED_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_SIGNAL_RESOLVED_H

#include <cci/serialization/string.hpp>
#include <cci/serialization/split_free.hpp>

#include <systemc>

#include <systemc-checkpoint/serialization/sc_logic.h>
#include <systemc-checkpoint/serialization/sc_signal.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serialization/vector.h>
#include <systemc-checkpoint/sim_context_proxy_checkpoint.h>

#include <string>
#include <vector>

CCI_SERIALIZATION_SPLIT_FREE(sc_core::sc_signal_resolved)

namespace cci {
namespace serialization {

template <class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_core::sc_signal_resolved &signal,  // NOLINT
          const unsigned int version) {
    std::vector<sc_dt::sc_logic> val_vec;
    std::vector<sc_core::sc_process_b *> proc_vec;
    std::vector<std::string> processes;

    sc_core::sc_signal<sc_dt::sc_logic, sc_core::SC_MANY_WRITERS> &base =
        static_cast<sc_core::sc_signal<sc_dt::sc_logic,
                                       sc_core::SC_MANY_WRITERS> &> (signal);

    ar & SMD(base);
    ar & SMD(val_vec);
    ar & SMD(processes);

    std::vector<std::string>::iterator i;
    for (i = processes.begin(); i != processes.end(); ++i) {
        sc_core::sc_process_b *p = dynamic_cast<sc_core::sc_process_b *>(
            sc_core::sc_find_object(i->c_str()));
        proc_vec.push_back(p);
    }

    sc_core::simContextProxyCheckpoint::set_sc_signal_resolved_val_vec(
        &signal, val_vec);
    sc_core::simContextProxyCheckpoint::set_sc_signal_resolved_proc_vec(
        &signal, proc_vec);
}

template <class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_core::sc_signal_resolved &signal,
          const unsigned int version) {
    std::vector<sc_dt::sc_logic> val_vec =
        sc_core::simContextProxyCheckpoint::sc_signal_resolved_val_vec(&signal);
    std::vector<sc_core::sc_process_b *> proc_vec =
        sc_core::simContextProxyCheckpoint::sc_signal_resolved_proc_vec(
            &signal);

    std::vector<std::string> processes;
    std::vector<sc_core::sc_process_b *>::iterator i;
    for (i = proc_vec.begin(); i != proc_vec.end(); ++i)
        processes.push_back((*i)->name());

    const sc_core::sc_signal<sc_dt::sc_logic, sc_core::SC_MANY_WRITERS> &base =
        static_cast<const sc_core::sc_signal<sc_dt::sc_logic,
                                             sc_core::SC_MANY_WRITERS> &> (
                                                 signal);

    ar & SMD(base);
    ar & SMD(val_vec);
    ar & SMD(processes);
}

}  // namespace serialization
}  // namespace cci

#endif
