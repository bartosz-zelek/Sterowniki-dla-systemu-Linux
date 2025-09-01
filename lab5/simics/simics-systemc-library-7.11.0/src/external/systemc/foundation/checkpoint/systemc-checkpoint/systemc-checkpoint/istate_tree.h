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

#ifndef SYSTEMC_CHECKPOINT_ISTATE_TREE_H
#define SYSTEMC_CHECKPOINT_ISTATE_TREE_H

#include <cci/archive/basic_archive.hpp>
#include <boost/property_tree/ptree.hpp>

#include <systemc-checkpoint/state_tree.h>
#include <systemc-checkpoint/hierarchical_name.h>

#include <string>

namespace sc_checkpoint {

class IStateTree : public StateTree {
  public:
    explicit IStateTree(boost::property_tree::ptree *property_tree);

    template <typename T>
    bool load_value(T &t) const {  // NOLINT
        return load(t, tagValue_);
    }

    bool load_meta_type(cci::archive::object_id_type &t);  // NOLINT
    bool load_meta_type(cci::archive::object_reference_type &t);  // NOLINT
    bool load_meta_type(cci::archive::version_type &t);  // NOLINT
    bool load_meta_type(cci::archive::class_id_type &t);  // NOLINT
    bool load_meta_type(cci::archive::class_id_optional_type &t);  // NOLINT
    bool load_meta_type(cci::archive::class_id_reference_type &t);  // NOLINT
    bool load_meta_type(cci::archive::class_name_type &t);  // NOLINT
    bool load_meta_type(cci::archive::tracking_type &t);  // NOLINT

  private:
    template <typename T>
    bool load(T &t, const std::string suffix) const {  // NOLINT
        boost::optional<boost::property_tree::ptree &> node
            = property_tree_->get_child_optional(
                current_node_.concatenate(suffix));
        if (node) {
            t = node.get().get<T>("");
            return true;
        }
        return false;
    }

    template <typename T, typename LoadAs>
    bool load_as(T &t, const std::string suffix) {  // NOLINT
        LoadAs &old_value = t;
        LoadAs new_value;
        bool found = load(new_value, suffix);
        if (found) {
            old_value = new_value;
        }
        return found;
    }
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_ISTATE_TREE_H
