// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_SC_VECTOR_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_VECTOR_INTERFACE_H

#include <simics/base/types.h>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScVectorInterface {
  public:
    virtual std::vector<conf_object_t *> get_elements() = 0;
    virtual ~ScVectorInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
