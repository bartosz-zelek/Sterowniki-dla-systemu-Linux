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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FXVAL_FAST_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FXVAL_FAST_H

#ifndef SC_INCLUDE_FX
#define SC_INCLUDE_FX
#endif

#include <cci/serialization/split_free.hpp>

#include <systemc>

#include <systemc-checkpoint/serialization/smd.h>

#include <string>

namespace cci {
namespace serialization {

template<class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      sc_dt::sc_fxval_fast &value,  // NOLINT
                      const unsigned int file_version) {
    split_free(ar, value, file_version);
}

template <class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_dt::sc_fxval_fast &value,  // NOLINT
          const unsigned int version) {
    double content = 0.0;
    ar & SMD(content);
    value.set_val(content);
}

template <class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_dt::sc_fxval_fast &value, const unsigned int version) {
    double content = value.get_val();
    ar & SMD(content);
}

}  // namespace serialization
}  // namespace cci

#endif
