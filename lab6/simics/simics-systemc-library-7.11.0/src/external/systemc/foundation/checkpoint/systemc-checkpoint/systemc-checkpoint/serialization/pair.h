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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_PAIR_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_PAIR_H

#include <systemc-checkpoint/serialization/serializable_meta_data.h>

#include <boost/type_traits/remove_const.hpp>
#include <cci/serialization/serialization.hpp>

#include <utility>

namespace cci {
namespace serialization {

template <class Archive, class First, class Second>
inline void serialize(Archive &archive,  // NOLINT
                      std::pair<First, Second> &p,  // NOLINT
                      const unsigned int version) {
    archive & sc_checkpoint::serialization::create_smd(
        "first",
        const_cast<typename boost::remove_const<First>::type &>(p.first));
    archive & sc_checkpoint::serialization::create_smd("second", p.second);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_PAIR_H
