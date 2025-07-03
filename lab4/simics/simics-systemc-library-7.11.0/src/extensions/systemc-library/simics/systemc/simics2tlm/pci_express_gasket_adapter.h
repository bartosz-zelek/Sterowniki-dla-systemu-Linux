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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PCI_EXPRESS_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PCI_EXPRESS_GASKET_ADAPTER_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/systemc/context.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for PciDevice gasket.
 */
class PciExpressGasketAdapter
    : public iface::PciExpressInterface,
      public GasketAdapter<iface::PciExpressInterface> {
  public:
    PciExpressGasketAdapter(PciExpressInterface *pci_express,
                            iface::SimulationInterface *simulation)
        : pci_express_(pci_express), simulation_(simulation) {
    }
    int send_message(int type, const std::vector<uint8_t> &payload) override {
        Context context(simulation_);
        return pci_express_->send_message(type, payload);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(pci_express_);
    }

  private:
    PciExpressInterface *pci_express_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
#endif
