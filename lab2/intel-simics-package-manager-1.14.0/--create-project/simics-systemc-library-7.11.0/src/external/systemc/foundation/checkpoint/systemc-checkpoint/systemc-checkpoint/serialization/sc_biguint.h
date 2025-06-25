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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SC_BIGUINT_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SC_BIGUINT_H

#include <cci/serialization/serialization.hpp>

#include <systemc>

#include <systemc-checkpoint/serialization/sc_unsigned.h>
#include <systemc-checkpoint/serialization/smd.h>

namespace cci {
namespace serialization {

template<int W, class Archive>
inline void serialize(Archive &ar,   // NOLINT(runtime/references)
                      sc_dt::sc_biguint<W> &biguint,  // NOLINT
                      const unsigned int file_version) {
    sc_dt::sc_unsigned &base = static_cast<sc_dt::sc_unsigned &>(biguint);
    ar & SMD(base);
}

}  // namespace serialization
}  // namespace cci

#endif
