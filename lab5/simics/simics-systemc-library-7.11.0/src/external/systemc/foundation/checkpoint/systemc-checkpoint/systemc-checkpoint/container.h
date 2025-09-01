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

#ifndef SYSTEMC_CHECKPOINT_CONTAINER_H
#define SYSTEMC_CHECKPOINT_CONTAINER_H

#include <systemc-checkpoint/serialization/smd.h>

#include <boost/lexical_cast.hpp>
// include before stack_constructor.hpp
#include <cci/serialization/serialization.hpp>
#include <cci/serialization/detail/stack_constructor.hpp>
#include <cci/serialization/item_version_type.hpp>
#include <cci/serialization/version.hpp>

#include <string>

namespace sc_checkpoint {

template <class Archive, class T>
struct LoadConstructData
    : public cci::serialization::detail::stack_allocate<T> {
    LoadConstructData(Archive &archive, std::string id,  // NOLINT
                      const unsigned int version) {
        archive.push(id);
        cci::serialization::load_construct_data_adl(
            archive, this->address(), version);
        archive.pop();
    }

    ~LoadConstructData() {
        this->address()->~T();
    }
};

inline std::string item_id(size_t idx) {
    return "item_" + boost::lexical_cast<std::string>(idx);
}

template <class Archive, class Container>
void save_container(Archive &archive,  // NOLINT
                    const Container &container,
                    size_t size) {
    const cci::serialization::item_version_type item_version(
        cci::serialization::version<typename Container::value_type>::value);
    archive << SMD(item_version);
    archive << SMD(size);

    size_t idx = 0;
    for (typename Container::const_iterator it = container.begin();
         it != container.end(); ++it) {
        std::string id = item_id(idx++);

        archive.push(id);
        cci::serialization::save_construct_data_adl(
            archive, &(*it), item_version);
        archive.pop();

        archive << sc_checkpoint::serialization::create_smd(id, *it);
    }
}

template <class Archive, class Container>
struct LoadElementSequence {
    typedef typename Container::iterator Iterator;
    typedef typename Container::value_type Value;
    Iterator operator() (Archive &archive, Container &container, std::string id,  // NOLINT
                         const unsigned int version, Iterator it) {
        LoadConstructData<Archive, Value> item(archive, id, version);
        archive >> sc_checkpoint::serialization::create_smd(
            id, item.reference());
        container.push_back(item.reference());

        // Instruct the archive that the newly created object is moved into the
        // container to make sure that any pointers to the object are restored
        // properly.
        archive.reset_object_address(&container.back(), &item.reference());

        return it;  // not used
    }
};

template <class Archive, class Container>
struct LoadElementMap {
    typedef typename Container::iterator Iterator;
    typedef typename Container::value_type Value;
    Iterator operator() (Archive &archive, Container &container, std::string id,  // NOLINT
                         const unsigned int version, Iterator it) {
        LoadConstructData<Archive, Value> item(archive, id, version);
        archive >> sc_checkpoint::serialization::create_smd(
            id, item.reference());

        Iterator new_it = container.insert(it, item.reference());
        archive.reset_object_address(&new_it->second, &item.reference().second);
        return new_it;
    }
};

template <class Archive, class Container>
struct LoadElementSet {
    typedef typename Container::iterator Iterator;
    typedef typename Container::value_type Value;
    Iterator operator() (Archive &archive, Container &container, std::string id,  // NOLINT
                         const unsigned int version, Iterator it) {
        LoadConstructData<Archive, Value> item(archive, id, version);
        archive >> sc_checkpoint::serialization::create_smd(
            id, item.reference());

        Iterator new_it = container.insert(it, item.reference());
        archive.reset_object_address(&*new_it, &item.reference());
        return new_it;
    }
};

template <class Archive, class Container, class LoadElement>
void load_container(Archive &archive, Container &container) {  // NOLINT
    container.clear();

    cci::serialization::item_version_type item_version(0);
    size_t size = 0;
    archive >> SMD(item_version);
    archive >> SMD(size);

    LoadElement load;
    typename Container::iterator it = container.begin();
    for (size_t idx = 0; idx < size; ++idx) {
        it = load(archive, container, item_id(idx), item_version, it);
    }
}

}  // namespace sc_checkpoint

#endif  // SYSTEMC_CHECKPOINT_CONTAINER_H
