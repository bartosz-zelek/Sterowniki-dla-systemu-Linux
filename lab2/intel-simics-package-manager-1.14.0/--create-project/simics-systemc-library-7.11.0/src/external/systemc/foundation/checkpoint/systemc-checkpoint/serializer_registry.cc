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

#include <cci/serialization/string.hpp>

#include <systemc-checkpoint/all_sc_objects.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/notify_callbacks.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/serialization/set.h>
#include <systemc-checkpoint/serialization/smd.h>
#include <systemc-checkpoint/serializer_registry.h>

#include <map>
#include <set>
#include <string>

using sc_checkpoint::serialization::SerializerInterface;

namespace sc_checkpoint {

SerializerRegistry *SerializerRegistry::Instance() {
    static SerializerRegistry registry;
    return &registry;
}

void SerializerRegistry::registerSerializer(SerializerInterface *serializer) {
    serializers_.insert(serializer);
}

void SerializerRegistry::unregisterSerializer(
        SerializerInterface *serializer) {
    serializers_.erase(serializer);
}

template<class Archive>
void SerializerRegistry::save(Archive &ar,
                              const unsigned int version) const {
    std::set<std::string> objects;
    std::map<sc_core::sc_object *, std::set<SerializerInterface *> > cache;
    AllScObjects all_objects;
    AllScObjects::iterator i;
    for (i = all_objects.begin(); i != all_objects.end(); ++i) {
        cache[*i] = findSerializers(*i);
        if (cache[*i].size() > 0)
            objects.insert((*i)->name());
    }

    ar & SMD(objects);

    std::set<std::string>::iterator j;
    int idx = 0;
    for (j = objects.begin(); j != objects.end(); ++j) {
        sc_core::sc_object *obj = sc_core::sc_find_object(j->c_str());
        std::set<SerializerInterface *> &serializers = cache[obj];
        std::set<SerializerInterface *>::iterator j;
        for (j = serializers.begin(); j != serializers.end(); ++j) {
            ar.push(boost::lexical_cast<std::string>(++idx));
            object_under_serialization_ = obj;
            (*j)->save(ar, obj);
            object_under_serialization_ = NULL;
            ar.pop();
        }
    }
}
template<class Archive>
void SerializerRegistry::load(Archive &ar,
                              const unsigned int version) {
    std::set<std::string> objects;
    ar & SMD(objects);

    std::set<std::string>::iterator i;
    int idx = 0;
    for (i = objects.begin(); i != objects.end(); ++i) {
        sc_core::sc_object *obj = sc_core::sc_find_object(i->c_str());
        std::set<SerializerInterface *> serializers = findSerializers(obj);
        std::set<SerializerInterface *>::iterator j;
        for (j = serializers.begin(); j != serializers.end(); ++j) {
            ar.push(boost::lexical_cast<std::string>(++idx));
            object_under_serialization_ = obj;
            (*j)->load(ar, obj);
            object_under_serialization_ = NULL;
            ar.pop();
        }
    }
}

sc_core::sc_object *SerializerRegistry::object_under_serialization() {
    return object_under_serialization_;
}

SerializerRegistry::SerializerRegistry() : object_under_serialization_(NULL) {
}

std::set<SerializerInterface *> SerializerRegistry::findSerializers(
        sc_core::sc_object* object) const {
    std::set<SerializerInterface *> serializers;
    std::set<SerializerInterface *>::iterator i;
    for (i = serializers_.begin(); i != serializers_.end(); ++i)
        if ((*i)->isSerializable(object))
            serializers.insert(*i);

    return serializers;
}

template
void SerializerRegistry::save<OArchive>(OArchive&, unsigned int) const;

template
void SerializerRegistry::load<IArchive>(IArchive&, unsigned int);

}  // namespace sc_checkpoint
