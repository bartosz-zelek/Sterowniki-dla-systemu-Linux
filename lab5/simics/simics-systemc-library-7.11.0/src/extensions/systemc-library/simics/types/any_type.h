// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_TYPES_ANY_TYPE_H
#define SIMICS_TYPES_ANY_TYPE_H

#include <stddef.h>

namespace simics {
namespace types {

/** Generic type class */
class AnyType {
    class ValueInterface {
      public:
        virtual ValueInterface *copy() const = 0;
        virtual ~ValueInterface() {}
    };

    template<typename T>
    class Value : public ValueInterface {
      public:
        explicit Value(const T &value) : value_(value) {}
        virtual ValueInterface *copy() const {
            return new Value<T>(value_);
        }
        T &value() {
            return value_;
        }
      private:
        T value_;
    };

  public:
    AnyType() : value_(NULL) {}
    AnyType(const AnyType &other) {
        value_ = other.value_ ? other.value_->copy() : NULL;
    }
    AnyType(AnyType &&other) {
        value_ = other.value_;
        other.value_ = NULL;
    }
    template<typename T>
    AnyType(const T &value) : value_(new Value<T>(value)) {} // NOLINT

    AnyType &operator=(AnyType &&other) {
        if (this == &other)
            return *this;

        if (value_)
            delete value_;

        value_ = other.value_;
        other.value_ = NULL;
        return *this;
    }
    AnyType &operator=(const AnyType &other) {
        if (this == &other)
            return *this;

        if (value_)
            delete value_;

        value_ = other.value_ ? other.value_->copy() : NULL;
        return *this;
    }

    template<typename T>
    AnyType &operator=(const T &value) {
        if (value_)
            delete value_;
        value_ = new Value<T>(value);
        return *this;
    }
    template<typename T>
    T value() {
        return static_cast<Value<T> *>(value_)->value();
    }
    bool isSet() {
        return !!value_;
    }
    ~AnyType() {
        if (value_)
            delete value_;
    }

  private:
    ValueInterface *value_;
};

}  // namespace types
}  // namespace simics

#endif
