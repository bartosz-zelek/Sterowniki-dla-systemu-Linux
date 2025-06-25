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

#include <systemc-checkpoint/state_keeper.h>

#include <boost/lexical_cast.hpp>

#include <set>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace {
bool implementsStateKeeperInterface(const sc_core::sc_object *object) {
    return dynamic_cast<
        const sc_checkpoint::StateKeeperInterface *>(object) != NULL;
}
}  // namespace

namespace sc_checkpoint {

StateKeeper::StateKeeper() : sc_objects_(NULL) {}

StateKeeper::~StateKeeper() {
    std::set<Handles *>::iterator i;
    for (i = all_handles_.begin(); i != all_handles_.end(); ++i)
        delete *i;

    delete sc_objects_;
}

bool StateKeeper::save(void **handleOut) {
    Handles *h = new Handles();
    all_handles_.insert(h);
    *handleOut = h;
    AllScObjects *objects = get_sc_objects();
    AllScObjects::iterator i;
    for (i = objects->begin(); i != objects->end(); ++i) {
        void *handle = NULL;
        StateKeeperInterface *k = dynamic_cast<StateKeeperInterface *>(*i);
        if (k && !k->save(&handle))
            return false;

        h->insert(Handles::value_type(handle, k));
    }

    return true;
}

bool StateKeeper::restore(void *handle) {
    Handles *h = static_cast<Handles *>(handle);
    if (all_handles_.find(h) == all_handles_.end())
        return false;

    Handles::left_iterator i;
    for (i = h->left.begin(); i != h->left.end(); ++i) {
        if (!i->second->restore(i->first))
            return false;
    }

    return true;
}

bool StateKeeper::merge(void *handlePrevious, void *handleRemove) {
    Handles *p = static_cast<Handles *>(handlePrevious);
    Handles *r = static_cast<Handles *>(handleRemove);
    if (all_handles_.find(r) == all_handles_.end())
        return false;

    if (handlePrevious == NULL) {
        // special case: only delete state when previous is NULL
        all_handles_.erase(r);
        delete r;
        return true;
    }

    if (all_handles_.find(p) == all_handles_.end()) {
        return false;
    }

    AllScObjects *objects = get_sc_objects();
    AllScObjects::iterator i;
    for (i = objects->begin(); i != objects->end(); ++i) {
        StateKeeperInterface *k = dynamic_cast<StateKeeperInterface *>(*i);
        if (k && !k->merge(p->right.at(k), r->right.at(k)))
            return false;
    }

    all_handles_.erase(r);
    delete r;
    return true;
}

bool StateKeeper::write(
    const std::string &dir, const std::string &prefix, bool persistent) {
    AllScObjects *objects = get_sc_objects();
    for (size_t idx = 0; idx < objects->size(); ++idx) {
        StateKeeperInterface *k =
            dynamic_cast<StateKeeperInterface *>(objects->at(idx));
        if (k && !k->write(
                dir, boost::lexical_cast<string>(idx), persistent))
            return false;
    }

    return true;
}

bool StateKeeper::write_standalone(
    const std::string &dir, const std::string &prefix, bool persistent) {
    AllScObjects *objects = get_sc_objects();
    for (size_t idx = 0; idx < objects->size(); ++idx) {
        StateKeeperInterface *k =
            dynamic_cast<StateKeeperInterface *>(objects->at(idx));
        if (k && !k->write_standalone(
                dir, boost::lexical_cast<string>(idx), persistent))
            return false;
    }

    return true;
}

bool StateKeeper::read(const vector<std::string> &dirs, const std::string,
                       bool persistent) {
    AllScObjects *objects = get_sc_objects();
    for (size_t idx = 0; idx < objects->size(); ++idx) {
        StateKeeperInterface *k =
            dynamic_cast<StateKeeperInterface *>(objects->at(idx));
        if (k && !k->read(dirs, boost::lexical_cast<string>(idx), persistent))
            return false;
    }

    return true;
}

AllScObjects *StateKeeper::get_sc_objects() {
    if (!sc_objects_)
        sc_objects_ = new AllScObjects(implementsStateKeeperInterface);

    return sc_objects_;
}

}  // namespace sc_checkpoint
