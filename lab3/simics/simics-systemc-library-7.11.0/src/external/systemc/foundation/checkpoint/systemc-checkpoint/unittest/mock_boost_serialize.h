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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_MOCK_BOOST_SERIALIZE_H
#define SYSTEMC_CHECKPOINT_UNITTEST_MOCK_BOOST_SERIALIZE_H

#include <systemc-checkpoint/serialization/smd.h>

#include <cci/serialization/split_member.hpp>

namespace unittest {

class MockBoostSerialize {
  public:
    MockBoostSerialize()
        : value_(0), save_called_(0), load_called_(0),
          reverse_execution_(false) {}
    template<class Archive>
    void save(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version) const {
        ar & SMD(value_);
        ++save_called_;
    }
    template<class Archive>
    void load(Archive &ar,  // NOLINT(runtime/references)
              const unsigned int version) {
        ar & SMD(value_);
        ++load_called_;
    }
    CCI_SERIALIZATION_SPLIT_MEMBER()
    void set_reverse_execution(bool isReverseExecution) {
        reverse_execution_ = isReverseExecution;
    }

    int value_;
    mutable int save_called_;
    int load_called_;
    bool reverse_execution_;
};

}  // namespace unittest

#endif
