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

#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/payload_update_registry.h>
#include <systemc-checkpoint/serialization/payload.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <cci/archive/detail/archive_serializer_map.hpp>
#include <cci/archive/impl/archive_serializer_map.ipp>

#include <iostream>  // NOLINT(readability/streams)
#include <string>

using cci::archive::object_id_type;
using cci::archive::object_reference_type;
using cci::archive::version_type;
using cci::archive::class_id_type;
using cci::archive::class_id_optional_type;
using cci::archive::class_id_reference_type;
using cci::archive::class_name_type;
using cci::archive::tracking_type;

using cci::archive::detail::common_iarchive;
using cci::archive::detail::archive_serializer_map;

using sc_checkpoint::serialization::SerializableMetaData;

namespace cci {
namespace archive {
template class archive_serializer_map<sc_checkpoint::IArchive>;
}  // namespace archive
}  // namespace cci

namespace sc_checkpoint {

IArchive::IArchive(IStateTree *istate_tree)
    : common_iarchive<IArchive>(0), istate_tree_(istate_tree) {}

void IArchive::load_override(
    const SerializableMetaData<tlm::tlm_generic_payload*> &t) {
    istate_tree_->push(t.name());
    cci::archive::detail::common_iarchive<IArchive>::load_override(t.value());
    istate_tree_->pop();

    sc_checkpoint::PayloadUpdateRegistry::Instance()->update(
        sc_checkpoint::SerializerRegistry::
            Instance()->object_under_serialization(),
        static_cast<tlm::tlm_generic_payload*>(t.value()));
}

void IArchive::load_override(object_id_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        object_reference_type ref(t);
        if (istate_tree_->load_meta_type(ref)) {
            t = ref;
        } else {
            print_error(
                "Neither an object ID nor an object ID reference found!");
        }
    }
}

void IArchive::load_override(object_reference_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        print_error(
            "Could not find an object ID reference where one was expected!");
    }
}
void IArchive::load_override(version_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        print_error("Could not find a version where one was expected!");
    }
}
void IArchive::load_override(class_id_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        class_id_reference_type ref(t);
        if (istate_tree_->load_meta_type(ref)) {
            t = ref;
        } else {
            print_error("Neither a class ID nor a class ID reference found!");
        }
    }
}

void IArchive::load_override(class_id_optional_type &t) {}

void IArchive::load_override(class_id_reference_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        print_error(
            "Could not find a class ID reference where one was expected!");
    }
}
void IArchive::load_override(class_name_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        print_error("Could not find a class name where one was expected!");
    }
}
void IArchive::load_override(tracking_type &t) {
    if (!istate_tree_->load_meta_type(t)) {
        print_error(
            "Could not find tracking information where it was expected!");
    }
}

void IArchive::print_error(const char *msg) const {
    std::cerr << istate_tree_->current_node() << ": " << msg << std::endl;
}

void IArchive::push(const std::string &name) {
    istate_tree_->push(name);
}
void IArchive::pop() {
    istate_tree_->pop();
}

}  // namespace sc_checkpoint
