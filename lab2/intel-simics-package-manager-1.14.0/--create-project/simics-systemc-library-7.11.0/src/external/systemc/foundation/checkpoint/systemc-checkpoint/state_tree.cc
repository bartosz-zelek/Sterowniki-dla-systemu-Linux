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

#include <systemc-checkpoint/state_tree.h>

#include <string>

using boost::property_tree::ptree;

namespace sc_checkpoint {

const char *StateTree::tagValue_ = "value";
const char *StateTree::tagType_ = "type";
const char *StateTree::tagObjectId_ = "object_id";
const char *StateTree::tagObjectReference_ = "object_reference";
const char *StateTree::tagVersion_ = "version";
const char *StateTree::tagClassId_ = "class_id";
const char *StateTree::tagClassIdOptional_ = "class_id_optional";
const char *StateTree::tagClassIdReference_ = "class_id_reference";
const char *StateTree::tagClassIdName_ = "class_name";
const char *StateTree::tagTracking_ = "tracking";

StateTree::StateTree(ptree *property_tree) : property_tree_(property_tree) {
    assert(property_tree_);
}

void StateTree::push(std::string node_name) {
    current_node_.push(node_name);
}

void StateTree::pop() {
    current_node_.pop();
}

std::string StateTree::current_node() const {
    return current_node_.name();
}

}  // namespace sc_checkpoint
