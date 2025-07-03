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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_DEVICE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCI_DEVICE_INTERFACE_H

#include <stdint.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics pci_device interface. */
class PciDeviceInterface {
  public:
    virtual void bus_reset() = 0;
    virtual void system_error() = 0;
    virtual void interrupt_raised(int pin) = 0;
    virtual void interrupt_lowered(int pin) = 0;
    virtual ~PciDeviceInterface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
