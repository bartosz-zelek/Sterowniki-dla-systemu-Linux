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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PCI_DEVICE_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PCI_DEVICE_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/pci_device_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for PciDevice gasket.
 */
class PciDeviceGasketAdapter
    : public iface::PciDeviceInterface,
      public GasketAdapter<iface::PciDeviceInterface> {
  public:
    PciDeviceGasketAdapter(PciDeviceInterface *pci_device,
                           iface::SimulationInterface *simulation)
        : pci_device_(pci_device), simulation_(simulation) {
    }
    void bus_reset() override {
        Context context(simulation_);
        pci_device_->bus_reset();
    }
    void system_error() override {
        Context context(simulation_);
        pci_device_->system_error();
    }
    void interrupt_raised(int pin) override {
        Context context(simulation_);
        pci_device_->interrupt_raised(pin);
    }
    void interrupt_lowered(int pin) override {
        Context context(simulation_);
        pci_device_->interrupt_lowered(pin);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(pci_device_);
    }

  private:
    PciDeviceInterface *pci_device_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
