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

#include <simics/systemc/awareness/attributes.h>

#include <vector>

namespace simics {
namespace systemc {
namespace awareness {

void Attributes::init() {
    for (auto i = attributes_.begin();
        i != attributes_.end(); ++i) {
        instance_attributes_.push_back((*i)->create());
    }
}

Attributes::~Attributes() {
    for (auto i = instance_attributes_.begin();
        i != instance_attributes_.end(); ++i) {
        delete *i;
    }
}

int Attributes::key_ = 0;
std::vector<std::shared_ptr<Attribute> > Attributes::attributes_;

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
