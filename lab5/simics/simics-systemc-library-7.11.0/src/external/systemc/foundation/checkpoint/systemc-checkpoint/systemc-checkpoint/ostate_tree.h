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

#ifndef SYSTEMC_CHECKPOINT_OSTATE_TREE_H
#define SYSTEMC_CHECKPOINT_OSTATE_TREE_H

#include <cci/archive/basic_archive.hpp>

#include <systemc-checkpoint/state_tree.h>
#include <systemc-checkpoint/hierarchical_name.h>

#include <string>

namespace sc_checkpoint {

class OStateTree : public StateTree {
  public:
    explicit OStateTree(boost::property_tree::ptree *property_tree);

    template <typename T>
    void store_value(const T &t) {
        store(t, tagValue_);
    }

    void store_meta_type(const std::string &t);
    void store_meta_type(const cci::archive::object_id_type &t);
    void store_meta_type(const cci::archive::object_reference_type &t);
    void store_meta_type(const cci::archive::version_type &t);
    void store_meta_type(const cci::archive::class_id_type &t);
    void store_meta_type(const cci::archive::class_id_optional_type &t);
    void store_meta_type(const cci::archive::class_id_reference_type &t);
    void store_meta_type(const cci::archive::class_name_type &t);
    void store_meta_type(const cci::archive::tracking_type &t);

  private:
    template <typename T>
    void store(const T &t, const std::string suffix) {
        const std::string store_at = current_node_.concatenate(suffix);
        boost::optional<boost::property_tree::ptree &> node
            = property_tree_->get_child_optional(store_at);
        assert(!node);

        property_tree_->put(store_at, t);
    }

    template <typename T, typename StoreAs>
    void store_as(const T &t, const std::string suffix) {
        store(static_cast<StoreAs>(t), suffix);
    }
};

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_OSTATE_TREE_H
