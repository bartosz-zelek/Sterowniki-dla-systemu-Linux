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

#include <systemc-checkpoint/oarchive.h>

#include <cci/archive/detail/archive_serializer_map.hpp>
#include <cci/archive/impl/archive_serializer_map.ipp>

#include <string>

using cci::archive::object_id_type;
using cci::archive::object_reference_type;
using cci::archive::version_type;
using cci::archive::class_id_type;
using cci::archive::class_id_optional_type;
using cci::archive::class_id_reference_type;
using cci::archive::class_name_type;
using cci::archive::tracking_type;

using cci::archive::detail::common_oarchive;
using cci::archive::detail::archive_serializer_map;

namespace cci {
namespace archive {
template class archive_serializer_map<sc_checkpoint::OArchive>;
}  // namespace archive
}  // namespace cci

namespace sc_checkpoint {

OArchive::OArchive(OStateTree *ostate_tree)
    : common_oarchive<OArchive>(0), ostate_tree_(ostate_tree) {}

void OArchive::save_override(const object_id_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const object_reference_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const version_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const class_id_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const class_id_optional_type &t) {}

void OArchive::save_override(const class_id_reference_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const class_name_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::save_override(const tracking_type &t) {
    ostate_tree_->store_meta_type(t);
}

void OArchive::push(const std::string &name) {
    ostate_tree_->push(name);
}
void OArchive::pop() {
    ostate_tree_->pop();
}

}  // namespace sc_checkpoint
