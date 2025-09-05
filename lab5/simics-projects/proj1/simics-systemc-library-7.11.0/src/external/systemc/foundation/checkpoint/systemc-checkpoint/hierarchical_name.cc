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

#include <systemc-checkpoint/hierarchical_name.h>

#include <string>

namespace sc_checkpoint {

HierarchicalName::HierarchicalName(
    const std::string prefix, const std::string separator)
    : prefix_(prefix), separator_(separator) {}

void HierarchicalName::push(const std::string name) {
    name_.push_back(name);
}

void HierarchicalName::pop() {
    if (name_.size() > 0) {
        name_.pop_back();
    }
}

std::string HierarchicalName::name() const {
    std::string name = prefix_;
    for (NameVectorIt it = name_.begin(); it != name_.end(); ++it) {
        name += separator_ + *it;
    }

    return name;
}

std::string HierarchicalName::concatenate(const std::string suffix) const {
    return name() + separator_ + suffix;
}

}  // namespace sc_checkpoint
