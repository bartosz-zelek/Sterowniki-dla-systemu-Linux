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

#include <cci/serialization/extended_type_info.hpp>

#include <systemc-checkpoint/istate_tree.h>

#include <algorithm>
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

IStateTree::IStateTree(ptree *property_tree) : StateTree(property_tree) {}

bool IStateTree::load_meta_type(object_id_type &t) {
    return load_as<object_id_type, uint_least32_t>(t, tagObjectId_);
}

bool IStateTree::load_meta_type(object_reference_type &t) {
    return load_as<object_reference_type, uint_least32_t>(
        t, tagObjectReference_);
}

bool IStateTree::load_meta_type(version_type &t) {
    return load_as<version_type, uint_least32_t>(t, tagVersion_);
}

bool IStateTree::load_meta_type(class_id_type &t) {
    return load_as<class_id_type, int_least16_t>(t, tagClassId_);
}

bool IStateTree::load_meta_type(class_id_optional_type &t) {
    return load_as<class_id_optional_type, int_least16_t>(
        t, tagClassIdOptional_);
}

bool IStateTree::load_meta_type(class_id_reference_type &t) {
    return load_as<class_id_reference_type, int_least16_t>(
        t, tagClassIdReference_);
}

// No need to allocate any space for the string here, the boost serialization
// libraries have already taken care of this.
bool IStateTree::load_meta_type(class_name_type &t) {
    std::string str;
    bool found = load(str, tagClassIdName_);
    if (found) {
        BOOST_ASSERT_MSG(str.size() <= CCI_SERIALIZATION_MAX_KEY_SIZE,
                         "The loaded string is too big for a class_name_type!");
        char *c_str = t;
        std::copy(str.begin(), str.end(), c_str);
        c_str[str.size()] = '\0';
    }

    return found;
}

bool IStateTree::load_meta_type(tracking_type &t) {
    return load_as<tracking_type, bool>(t, tagTracking_);
}

}  // namespace sc_checkpoint
