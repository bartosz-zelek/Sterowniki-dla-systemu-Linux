// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_INTERFACE_H

#include <stdint.h>
#include <simics/base/conf-object.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics pcie_device interface. */
class PcieDeviceInterface {
  public:
    virtual void connected(conf_object_t *port_obj, uint16_t device_id) = 0;
    virtual void disconnected(conf_object_t *port_obj, uint16_t device_id) = 0;
    virtual void hot_reset() = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
