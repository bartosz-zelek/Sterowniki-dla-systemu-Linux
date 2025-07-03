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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_ETHERNET_COMMON_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_ETHERNET_COMMON_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/ethernet_common_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

class EthernetCommonGasketAdapter
    : public iface::EthernetCommonInterface,
      public GasketAdapter<iface::EthernetCommonInterface> {
  public:
    EthernetCommonGasketAdapter(
        EthernetCommonInterface *eth_device,
        iface::SimulationInterface *simulation)
        : eth_device_(eth_device), simulation_(simulation) {
    }
    void frame(const types::frags_t *frame, int crc_ok) override {
        Context context(simulation_);
        eth_device_->frame(frame, crc_ok);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(eth_device_);
    }

  private:
    EthernetCommonInterface *eth_device_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
