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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_STL_TEST_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_STL_TEST_ENVIRONMENT_H

#include <systemc-checkpoint/istate_tree.h>
#include <systemc-checkpoint/iarchive.h>
#include <systemc-checkpoint/oarchive.h>
#include <systemc-checkpoint/ostate_tree.h>
#include <systemc-checkpoint/serialization/smd.h>

template <class T>
void save(const T &t,
          boost::property_tree::ptree &ptree) {  // NOLINT(runtime/references)
    sc_checkpoint::OStateTree tree(&ptree);
    sc_checkpoint::OArchive archive(&tree);

    archive << SMD(t);
}

template <class T>
void load(T &t, boost::property_tree::ptree &ptree) {  // NOLINT
    sc_checkpoint::IStateTree tree(&ptree);
    sc_checkpoint::IArchive archive(&tree);

    archive >> SMD(t);
}

class UserType {
  public:
    UserType() {}
    explicit UserType(int i) : i_(i) {}

    int i_;

    template <class Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(i_);
    }

    bool operator==(const UserType &other) const {
        return i_ == other.i_;
    }
    bool operator!=(const UserType &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(std::ostream &os, const UserType &t) {
        os << t.i_;
        return os;
    }
};

struct UserComparison {
    bool operator() (UserType const &l,
                     UserType const &r) const {
        return l.i_ < r.i_;
    }
};

class UserTypeNonDefaultConstructor {
  public:
    explicit UserTypeNonDefaultConstructor(int i) : i_(i) {}

    bool operator==(const UserTypeNonDefaultConstructor &other) const {
        return i_ == other.i_;
    }
    bool operator!=(const UserTypeNonDefaultConstructor &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(
        std::ostream &os, const UserTypeNonDefaultConstructor &t) {
        os << t.i_;
        return os;
    }

    int i_;

    template <class Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {}
};

namespace cci {
namespace serialization {

template<class Archive>
inline void save_construct_data(Archive &archive,  // NOLINT
                                const UserTypeNonDefaultConstructor *t,
                                const unsigned int version) {
    int i = t->i_;
    archive << SMD(i);
}

template<class Archive>
inline void load_construct_data(Archive &archive,  // NOLINT
                                UserTypeNonDefaultConstructor *t,
                                const unsigned int version) {
    int i;
    archive >> SMD(i);

    ::new(t) UserTypeNonDefaultConstructor(i);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_STL_TEST_ENVIRONMENT_H
