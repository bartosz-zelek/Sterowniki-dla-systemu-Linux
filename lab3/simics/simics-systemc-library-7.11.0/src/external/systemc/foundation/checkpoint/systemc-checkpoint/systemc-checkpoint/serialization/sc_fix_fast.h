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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FIX_FAST_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_FIX_FAST_H

#ifndef SC_INCLUDE_FX
#define SC_INCLUDE_FX
#endif

#include <cci/serialization/split_free.hpp>

#include <systemc>

#include <systemc-checkpoint/serialization/serializer_macros.h>

CCI_SERIALIZATION_SPLIT_FREE(sc_dt::sc_fix_fast);

namespace cci {
namespace serialization {

template <class Archive>
void load(Archive &ar,   // NOLINT(runtime/references)
          sc_dt::sc_fix_fast &value,  // NOLINT
          const unsigned int version) {
    LOAD_TYPE_FROM_BIT_VECTOR(ar, value);
}

template <class Archive>
void save(Archive &ar,   // NOLINT(runtime/references)
          const sc_dt::sc_fix_fast &value, const unsigned int version) {
    SAVE_TYPE_TO_BIT_VECTOR(ar, value);
}


}  // namespace serialization
}  // namespace cci

#endif
