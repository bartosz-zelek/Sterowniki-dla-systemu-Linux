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

#ifndef SYSTEMC_CHECKPOINT_HIERARCHICAL_NAME_H
#define SYSTEMC_CHECKPOINT_HIERARCHICAL_NAME_H

#include <string>
#include <vector>

namespace sc_checkpoint {

class HierarchicalName {
  public:
    HierarchicalName(const std::string prefix = "checkpoint",
                     const std::string separator = ".");

    void push(const std::string name);
    void pop();
    std::string name() const;
    std::string concatenate(const std::string suffix) const;

  private:
    typedef std::vector<std::string> NameVector;
    typedef NameVector::const_iterator NameVectorIt;

    NameVector name_;
    const std::string prefix_;
    const std::string separator_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_HIERARCHICAL_NAME_H
