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

#ifndef SIMICS_SYSTEMC_IFACE_SC_OBJECT_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_OBJECT_INTERFACE_H

#include <simics/base/attr-value.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScObjectInterface {
  public:
    virtual std::vector<std::string> sc_print() const = 0;
    virtual std::vector<std::string> sc_dump() const = 0;
    virtual attr_value_t sc_kind() const = 0;
    virtual const char *sc_name() const = 0;
    virtual ~ScObjectInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
