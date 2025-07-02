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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_INTERFACE_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Simics pci_express interface. */
class PciExpressInterface {
  public:
    virtual int send_message(int type, const std::vector<uint8_t> &payload) = 0;
    virtual ~PciExpressInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
#endif
