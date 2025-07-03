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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PCIE_DEVICE_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PCIE_DEVICE_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/pcie_device_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for PcieDevice gasket.
 */
class PcieDeviceGasketAdapter
    : public iface::PcieDeviceInterface,
      public GasketAdapter<iface::PcieDeviceInterface> {
  public:
    PcieDeviceGasketAdapter(PcieDeviceInterface *pcie_device,
                            iface::SimulationInterface *simulation)
        : pcie_device_(pcie_device), simulation_(simulation) {
    }
    void connected(conf_object_t *port_obj, uint16_t device_id) override {
        Context context(simulation_);
        pcie_device_->connected(port_obj, device_id);
    }
    void disconnected(conf_object_t *port_obj, uint16_t device_id) override {
        Context context(simulation_);
        pcie_device_->disconnected(port_obj, device_id);
    }
    void hot_reset() override {
        Context context(simulation_);
        pcie_device_->hot_reset();
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(pcie_device_);
    }

  private:
    PcieDeviceInterface *pcie_device_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
