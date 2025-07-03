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

#ifndef SYSTEMC_CHECKPOINT_ALL_SC_OBJECTS_H
#define SYSTEMC_CHECKPOINT_ALL_SC_OBJECTS_H

#include <systemc>

#include <vector>

namespace sc_checkpoint {

class AllScObjects : public std::vector<sc_core::sc_object *> {
  public:
    AllScObjects() {
        const std::vector<sc_core::sc_object*> &v
            = sc_core::sc_get_top_level_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
             i != v.end(); ++i) {
            pushBackTree(*i);
        }
    }
    template<class T>
    explicit AllScObjects(T predicate) {
        const std::vector<sc_core::sc_object*> &v
            = sc_core::sc_get_top_level_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
             i != v.end(); ++i) {
            pushBackTree(*i, predicate);
        }
    }
    void pushBackTree(sc_core::sc_object *obj) {
        push_back(obj);
        const std::vector<sc_core::sc_object*> &v = obj->get_child_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
            i != v.end(); ++i) {
            pushBackTree(*i);
        }
    }
    template<class T>
    void pushBackTree(sc_core::sc_object *obj, T predicate) {
        if (predicate(obj))
            push_back(obj);
        const std::vector<sc_core::sc_object*> &v = obj->get_child_objects();
        for (std::vector<sc_core::sc_object*>::const_iterator i = v.begin();
            i != v.end(); ++i) {
            pushBackTree(*i, predicate);
        }
    }
};

}  // namespace sc_checkpoint

#endif
