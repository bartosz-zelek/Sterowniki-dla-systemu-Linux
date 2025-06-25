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

#ifndef SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZABLE_META_DATA_H
#define SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZABLE_META_DATA_H

#include <boost/type_index.hpp>

#include <string>

namespace sc_checkpoint {
namespace serialization {

// TODO(enilsson): the corresponding boost container (nvp) has save and load
// functions; it's unclear why. As far as I can tell there's no reason as to
// why the tuple itself should have to be serialized. It's only a container of
// sorts, so only the contents need be serialized.
template <typename T>
class SerializableMetaData {
  public:
    SerializableMetaData(const std::string name, T &value)  // NOLINT
        : name_(name),
          type_(boost::typeindex::type_id_runtime(value).pretty_name()),
          value_(&value) {}

    std::string name() const {
        return name_;
    }

    std::string type() const {
        return type_;
    }

    T &value() const {
        return *(value_);
    }
    const T &const_value() const {
        return *(value_);
    }

  private:
    const std::string name_;
    const std::string type_;
    T *value_;
};

// TODO(enilsson): our 'make_array' function is named as such because of the
// corresponding function in the boost serialization libraries. This function
// should be named similarly to the corresponding 'make_nvp' function.
template <typename T>
const SerializableMetaData<T> create_smd(std::string name, T &value) {  // NOLINT
    return SerializableMetaData<T>(name, value);
}

}  // namespace serialization
}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_SERIALIZATION_SERIALIZABLE_META_DATA_H
