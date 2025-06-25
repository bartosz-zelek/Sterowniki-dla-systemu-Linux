// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/conf-object.h>
#include <simics/systemc/awareness/proxy_factory_base.h>

namespace simics {
namespace systemc {
namespace awareness {

void ProxyFactoryBase::set_log_object(ConfObjectRef log_object) {
    log_object_ = log_object;
}

ConfObjectRef ProxyFactoryBase::log_object_;

}  // namespace awareness
}  // namespace systemc
}  // namespace simics
