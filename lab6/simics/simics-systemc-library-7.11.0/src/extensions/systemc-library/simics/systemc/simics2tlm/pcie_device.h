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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PCIE_DEVICE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PCIE_DEVICE_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/pcie_device_extension.h>
#include <simics/systemc/iface/pcie_device_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics pcie_device interface and translates
 * it into a TLM transaction.
 */
class PcieDevice : public simics::systemc::iface::PcieDeviceInterface,
                   public GasketOwner {
  public:
    virtual void gasketUpdated();
    // Methods translate Simics calls
    void connected(conf_object_t *port_obj, uint16_t device_id) override;
    void disconnected(conf_object_t *port_obj, uint16_t device_id) override;
    void hot_reset() override;
  private:
    ExtensionSender sender_;
    iface::PcieDeviceExtension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
