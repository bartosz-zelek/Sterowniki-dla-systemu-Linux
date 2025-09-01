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

#ifndef SIMICS_SYSTEMC_IFACE_SC_VERSION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_VERSION_INTERFACE_H

#include <map>
#include <string>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScVersionInterface {
  public:
    virtual const char *kernel_version() const = 0;
    virtual const char *library_version() const = 0;
    virtual const char *library_kernel_version() const = 0;
    virtual const std::map<std::string, std::string> *versions() const = 0;
    virtual ~ScVersionInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
