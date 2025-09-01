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

#include <systemc-checkpoint/ostate_tree.h>

#include <string>

using cci::archive::object_id_type;
using cci::archive::object_reference_type;
using cci::archive::version_type;
using cci::archive::class_id_type;
using cci::archive::class_id_optional_type;
using cci::archive::class_id_reference_type;
using cci::archive::class_name_type;
using cci::archive::tracking_type;

using boost::property_tree::ptree;

namespace sc_checkpoint {

OStateTree::OStateTree(ptree *property_tree) : StateTree(property_tree) {}

void OStateTree::store_meta_type(const std::string &t) {
    store(t, tagType_);
}

void OStateTree::store_meta_type(const object_id_type &t) {
    store_as<object_id_type, uint_least32_t>(t, tagObjectId_);
}

void OStateTree::store_meta_type(const object_reference_type &t) {
    store_as<object_reference_type, uint_least32_t>(t, tagObjectReference_);
}

void OStateTree::store_meta_type(const version_type &t) {
    store_as<version_type, uint_least32_t>(t, tagVersion_);
}

void OStateTree::store_meta_type(const class_id_type &t) {
    store_as<class_id_type, int_least16_t>(t, tagClassId_);
}

void OStateTree::store_meta_type(const class_id_optional_type &t) {
    store_as<class_id_optional_type, int_least16_t>(t, tagClassIdOptional_);
}

void OStateTree::store_meta_type(const class_id_reference_type &t) {
    store_as<class_id_reference_type, int_least16_t>(t, tagClassIdReference_);
}

void OStateTree::store_meta_type(const class_name_type &t) {
    store(std::string(t), tagClassIdName_);
}

void OStateTree::store_meta_type(const tracking_type &t) {
    store_as<tracking_type, bool>(t, tagTracking_);
}

}  // namespace sc_checkpoint
