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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_UPSTREAM_OPERATION_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCI_UPSTREAM_OPERATION_INTERFACE_H

#include <simics/types/addr_space.h>
#include <simics/types/pci_bus_exception_type.h>
#include <stdint.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics SystemC pci_upstream_operation interface. */
class PciUpstreamOperationInterface {
  public:
    // The address, buffer, size and op-type (read/write) must also be encoded
    // in the TLM2 GP that is the carrier of the PciUpstreamOperation extension
    // where these interface methods are encoded. A return value of 0 indicates
    // that the GP op-type is inconsistent with the extension method
    // invocation.
    virtual types::pci_bus_exception_type_t read(
            uint16_t rid,
            types::addr_space_t space) = 0;
    virtual types::pci_bus_exception_type_t write(
            uint16_t rid,
            types::addr_space_t space) = 0;
    virtual ~PciUpstreamOperationInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
