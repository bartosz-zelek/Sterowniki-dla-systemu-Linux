// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PACKET_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PACKET_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/packet_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

class PacketGasketAdapter
    : public iface::PacketInterface,
      public GasketAdapter<iface::PacketInterface> {
  public:
    PacketGasketAdapter(PacketInterface *packet,
                        iface::SimulationInterface *simulation)
        : packet_(packet), simulation_(simulation) {
    }
    virtual void transfer(const uint8 *data_ptr, size_t data_length) {
        Context context(simulation_);
        packet_->transfer(data_ptr, data_length);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(packet_);
    }

  private:
    PacketInterface* packet_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
