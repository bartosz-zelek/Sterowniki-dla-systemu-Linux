// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_ATTRIBUTES_H
#define SIMICS_SYSTEMC_AWARENESS_ATTRIBUTES_H

#include <simics/systemc/awareness/attribute.h>

#include <memory>
#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

class Attributes {
  public:
    template<typename T>
    static T *defineAttribute() {
        std::shared_ptr<T> ptr = std::make_shared<T> (key_);
        attributes_.push_back(ptr);
        ++key_;
        return ptr.get();
    }
    void init();
    template<typename T>
    T *attribute(int key) {
        return static_cast<T*>(instance_attributes_[key]);
    }

    Attributes() = default;
    virtual ~Attributes();
    Attributes(const Attributes &other) = delete;
    Attributes& operator=(const Attributes &other) = delete;

  private:
    static int key_;
    static std::vector<std::shared_ptr<Attribute> > attributes_;
    std::vector<Attribute*> instance_attributes_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
