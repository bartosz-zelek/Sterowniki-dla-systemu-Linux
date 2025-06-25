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

#ifndef SYSTEMC_CHECKPOINT_STATE_TREE_H
#define SYSTEMC_CHECKPOINT_STATE_TREE_H

#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/hierarchical_name.h>

#include <string>

namespace sc_checkpoint {

class StateTree {
  public:
    explicit StateTree(boost::property_tree::ptree *property_tree);

    void push(std::string node_name);
    void pop();
    std::string current_node() const;

  protected:
    boost::property_tree::ptree *property_tree_;
    HierarchicalName current_node_;

    static const char *tagValue_;
    static const char *tagType_;
    static const char *tagObjectId_;
    static const char *tagObjectReference_;
    static const char *tagVersion_;
    static const char *tagClassId_;
    static const char *tagClassIdOptional_;
    static const char *tagClassIdReference_;
    static const char *tagClassIdName_;
    static const char *tagTracking_;
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_STATE_TREE_H
