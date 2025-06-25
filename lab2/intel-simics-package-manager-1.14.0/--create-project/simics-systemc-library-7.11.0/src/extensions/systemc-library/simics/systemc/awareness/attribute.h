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

#ifndef SIMICS_SYSTEMC_AWARENESS_ATTRIBUTE_H
#define SIMICS_SYSTEMC_AWARENESS_ATTRIBUTE_H

#include <simics/base/attr-value.h>
#include <simics/base/conf-object.h>

namespace simics {
namespace systemc {
namespace awareness {

class Attribute {
  public:
    explicit Attribute(int key)
        : key_(key) {
    }
    int key() const {
        return key_;
    }
    virtual Attribute *create() = 0;
    virtual attr_value_t get() const = 0;
    virtual set_error_t set(attr_value_t *val) = 0;
    virtual ~Attribute() {}

  private:
    int key_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
