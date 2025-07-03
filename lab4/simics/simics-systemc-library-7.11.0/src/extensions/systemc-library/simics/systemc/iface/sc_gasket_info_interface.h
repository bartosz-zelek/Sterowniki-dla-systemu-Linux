// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_SC_GASKET_INFO_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_SC_GASKET_INFO_INTERFACE_H

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
class ScGasketInfoInterface {
  public:
    virtual const std::vector<std::vector<std::string> > *simics2tlm() = 0;
    virtual const std::vector<std::vector<std::string> > *tlm2simics() = 0;
    virtual const std::vector<std::vector<std::string> > *simics2systemc() = 0;
    virtual const std::vector<std::vector<std::string> > *systemc2simics() = 0;
    virtual ~ScGasketInfoInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
