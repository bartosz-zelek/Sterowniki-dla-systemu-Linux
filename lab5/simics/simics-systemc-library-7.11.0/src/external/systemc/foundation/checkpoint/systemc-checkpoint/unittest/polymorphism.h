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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_POLYMORPHISM_H
#define SYSTEMC_CHECKPOINT_UNITTEST_POLYMORPHISM_H

#include <systemc-checkpoint/serialization/smd.h>

#include <cci/serialization/serialization.hpp>

class Base {
  public:
    Base() : i_(0) {}
    explicit Base(int i) : i_(i) {}
    virtual ~Base() {}

    int i_;

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {  // NOLINT
        archive & SMD(i_);
    }

    bool operator==(const Base &other) const {
        return i_ == other.i_;
    }
    bool operator!=(const Base &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(std::ostream &os, const Base &t) {
        os << t.i_;
        return os;
    }
};

class DerivedA : public Base {
  public:
    DerivedA() : Base(0), j_(0) {}
    explicit DerivedA(int i, int j) : Base(i), j_(j) {}
    virtual ~DerivedA() {}

    int j_;

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {  // NOLINT
        archive & SMD_BASE_OBJECT(Base);
        archive & SMD(j_);
    }

    bool operator==(const DerivedA &other) const {
        return i_ == other.i_ && j_ == other.j_;
    }
    bool operator!=(const DerivedA &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(std::ostream &os, const DerivedA &t) {
        os << static_cast<const Base &>(t) << ", " << t.j_;
        return os;
    }
};

class DerivedB : public Base {
  public:
    DerivedB() : Base(0), j_(0) {}
    explicit DerivedB(int i, int j, int k) : Base(i), j_(j), k_(k) {}
    virtual ~DerivedB() {}

    int j_;
    int k_;

    template <class Archive>
    void serialize(Archive &archive, const unsigned int version) {  // NOLINT
        archive & SMD_BASE_OBJECT(Base);
        archive & SMD(j_);
        archive & SMD(k_);
    }

    bool operator==(const DerivedB &other) const {
        return i_ == other.i_ && j_ == other.j_ && k_ == other.k_;
    }
    bool operator!=(const DerivedB &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(std::ostream &os, const DerivedB &t) {
        os << static_cast<const Base &>(t) << ", " << t.j_ << ", " << t.k_;
        return os;
    }
};

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_POLYMORPHISM_H
