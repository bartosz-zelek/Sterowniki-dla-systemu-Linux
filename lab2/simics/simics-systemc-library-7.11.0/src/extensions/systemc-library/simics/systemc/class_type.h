// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_CLASS_TYPE_H
#define SIMICS_SYSTEMC_CLASS_TYPE_H

#include <string>
#include <typeinfo>

namespace simics {
namespace systemc {

class ClassType {
  public:
    ClassType() { implementor_ = this; }
    std::string type() const {
        return type_;
    }
    bool operator ==(const ClassType &type) const {
        return type_ == type.type_;
    }
    bool operator !=(const ClassType &type) const {
        return type_ != type.type_;
    }
    bool operator <(const ClassType &type) const {
        return type_ < type.type_;
    }
    template<class T>
    T *get_interface() {
        return dynamic_cast<T *>(implementor_);
    }
    template<class T>
    static ClassType typeForClass() {
        ClassType type;
        type.type_ =  typeid(T).name();
        return type;
    }
    virtual ~ClassType() {}

  protected:
    void set_type() {
        type_ =  typeid(*this).name();
    }
    std::string type_;
    ClassType *implementor_;
};

}  // namespace systemc
}  // namespace simics

#endif
