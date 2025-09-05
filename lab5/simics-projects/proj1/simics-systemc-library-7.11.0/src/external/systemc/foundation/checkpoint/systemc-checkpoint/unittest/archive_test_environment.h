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

#ifndef SYSTEMC_CHECKPOINT_UNITTEST_ARCHIVE_TEST_ENVIRONMENT_H
#define SYSTEMC_CHECKPOINT_UNITTEST_ARCHIVE_TEST_ENVIRONMENT_H

#include <systemc-checkpoint/serialization/smd.h>

#include <cci/serialization/serialization.hpp>
#include <cci/serialization/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/type_index.hpp>
#include <boost/test/unit_test.hpp>

#include <string>

template <typename T>
class SerializableWithSingleMember {
  public:
    SerializableWithSingleMember() {}
    explicit SerializableWithSingleMember(T init_value) : value_(init_value) {}

    T getValue() const {
        return value_;
    }
    void setValue(T test_value) {
        value_ = test_value;
    }

    boost::property_tree::ptree expectedState(std::string name) const {
        const std::string base = "checkpoint." + name;

        boost::property_tree::ptree ptree;
        ptree.put(base + ".type",
                  boost::typeindex::type_id_runtime(*this).pretty_name());
        ptree.put(base + ".tracking", false);
        ptree.put(base + ".version", 0);
        ptree.put(base + ".value_.type",
                  boost::typeindex::type_id_runtime(value_).pretty_name());
        ptree.put(base + ".value_.value", value_);

        return ptree;
    }

    void checkEqual(const SerializableWithSingleMember &other) const {
        BOOST_CHECK_EQUAL(value_, other.value_);
    }

    bool operator==(const SerializableWithSingleMember &other) const {
        return value_ == other.value_;
    }
    bool operator!=(const SerializableWithSingleMember &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(
        std::ostream &os, const SerializableWithSingleMember<T> &serializable) {
        os << serializable.value_;
        return os;
    }

  private:
    T value_;

    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(value_);
    }
};

// We wrap any type; the struct can then be serialized as a pointer. By
// default, boost serialization does not track primitive types.
template <typename T>
struct WrappedType {
  public:
    WrappedType() {}
    explicit WrappedType(T t) : value(t) {}
    T value;

    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(value);
    }
};

template <typename T>
class SerializableWithPointers {
  public:
    SerializableWithPointers() : p1_(NULL), p2_(NULL) {}
    explicit SerializableWithPointers(T expected_value)
        : expected_value_(expected_value),
          p1_(new WrappedType<T>(expected_value)),
          p2_(p1_) {}
    ~SerializableWithPointers() {
        delete p1_;
    }

    WrappedType<T> *v1() const {
        return p1_;
    }

    WrappedType<T> *v2() const {
        return p2_;
    }

    boost::property_tree::ptree expectedState(std::string name) const {
        const std::string base = "checkpoint." + name;

        boost::property_tree::ptree ptree;
        ptree.put(base + ".type",
                  boost::typeindex::type_id_runtime(*this).pretty_name());
        ptree.put(base + ".tracking", false);
        ptree.put(base + ".version", 0);

        ptree.put(base + ".p1_.type",
                  boost::typeindex::type_id_runtime(p1_).pretty_name());
        ptree.put(base + ".p1_.class_id", 1);
        ptree.put(base + ".p1_.tracking", true);
        ptree.put(base + ".p1_.version", 0);
        ptree.put(base + ".p1_.object_id", 0);
        ptree.put(base + ".p1_.value.type", boost::typeindex::type_id_runtime(
                      expected_value_).pretty_name());
        ptree.put(base + ".p1_.value.value", expected_value_);

        ptree.put(base + ".p2_.type",
                  boost::typeindex::type_id_runtime(p2_).pretty_name());
        ptree.put(base + ".p2_.class_id_reference", 1);
        ptree.put(base + ".p2_.object_reference", 0);

        return ptree;
    }

    void checkEqual(const SerializableWithPointers &other) const {
        BOOST_REQUIRE(p1_);
        BOOST_REQUIRE(p2_);

        // Pointers should point to the same instance
        BOOST_CHECK_EQUAL(p1_, p2_);
        // ...and the instance should be one that we expect
        BOOST_CHECK_EQUAL(p1_->value, other.p1_->value);
    }

  private:
    T expected_value_;

    WrappedType<T> *p1_;
    WrappedType<T> *p2_;

    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(p1_);
        archive & SMD(p2_);
    }
};

class SerializableBaseClass {
  public:
    explicit SerializableBaseClass(std::string base_str)
        : base_str_(base_str) {}

  protected:
    std::string base_str_;

  private:
    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD(base_str_);
    }
};

class SerializableDerivedClass : public SerializableBaseClass {
  public:
    SerializableDerivedClass() : SerializableBaseClass(""), derived_str_("") {}
    SerializableDerivedClass(std::string base_str, std::string derived_str)
        : SerializableBaseClass(base_str), derived_str_(derived_str) {}

    boost::property_tree::ptree expectedState(std::string name) const {
        const std::string derived = "checkpoint." + name;
        const std::string base = derived + ".SerializableBaseClass";

        boost::property_tree::ptree ptree;
        ptree.put(derived + ".type",
                  boost::typeindex::type_id_runtime(*this).pretty_name());
        ptree.put(derived + ".tracking", false);
        ptree.put(derived + ".version", 0);

        ptree.put(base + ".type", "SerializableBaseClass");
        ptree.put(base + ".tracking", "false");
        ptree.put(base + ".version", "0");
        ptree.put(base + ".base_str_.type",
                  boost::typeindex::type_id_runtime(base_str_).pretty_name());
        ptree.put(base + ".base_str_.value", base_str_);

        ptree.put(
            derived + ".derived_str_.type",
            boost::typeindex::type_id_runtime(derived_str_).pretty_name());
        ptree.put(derived + ".derived_str_.value", derived_str_);

        return ptree;
    }

    void checkEqual(const SerializableDerivedClass &other) const {
        BOOST_CHECK_EQUAL(base_str_, other.base_str_);
        BOOST_CHECK_EQUAL(derived_str_, other.derived_str_);
    }

  private:
    std::string derived_str_;

    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {
        archive & SMD_BASE_OBJECT(SerializableBaseClass);
        archive & SMD(derived_str_);
    }
};

class SerializableWithNonDefaultConstructor {
  public:
    explicit SerializableWithNonDefaultConstructor(int number)
        : number_(number) {}

    int number() const {
        return number_;
    }

    boost::property_tree::ptree expectedState(std::string name) {
        boost::property_tree::ptree ptree;
        std::string base = "checkpoint." + name;

        ptree.put(base + ".type", "SerializableWithNonDefaultConstructor*");
        ptree.put(base + ".class_id", 0);
        ptree.put(base + ".tracking", true);
        ptree.put(base + ".version", 0);
        ptree.put(base + ".object_id", 0);

        ptree.put(base + ".number.type", "int");
        ptree.put(base + ".number.value", number_);

        return ptree;
    }

    bool operator==(const SerializableWithNonDefaultConstructor &other) const {
        return number_ == other.number_;
    }
    bool operator!=(const SerializableWithNonDefaultConstructor &other) const {
        return !(*this == other);
    }
    friend std::ostream & operator<<(
        std::ostream &os,
        const SerializableWithNonDefaultConstructor &serializable) {
        os << serializable.number_;
        return os;
    }

  private:
    int number_;

    friend class cci::serialization::access;
    template <typename Archive>
    void serialize(Archive &archive,  // NOLINT(runtime/references)
                   const unsigned int version) {}
};

namespace cci {
namespace serialization {

template<class Archive>
inline void save_construct_data(Archive &archive,  // NOLINT
                                const SerializableWithNonDefaultConstructor *t,
                                const unsigned int version) {
    // Must be saved and loaded with the same identifier. This is one way of
    // doing it; another is to use the create_smd function directly.
    int number = t->number();
    archive << SMD(number);
}

template<class Archive>
inline void load_construct_data(Archive &archive,  // NOLINT
                                SerializableWithNonDefaultConstructor *t,
                                const unsigned int version) {
    int number;
    archive >> SMD(number);

    ::new(t)SerializableWithNonDefaultConstructor(number);
}

}  // namespace serialization
}  // namespace cci

#endif  // SYSTEMC_CHECKPOINT_UNITTEST_ARCHIVE_TEST_ENVIRONMENT_H
