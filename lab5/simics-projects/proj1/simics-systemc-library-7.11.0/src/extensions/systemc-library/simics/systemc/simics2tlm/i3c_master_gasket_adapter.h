// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I3C_MASTER_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I3C_MASTER_GASKET_ADAPTER_H

#include <simics/systemc/context.h>

#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for I3cMaster gasket.
 */
class I3cMasterGasketAdapter
    : public iface::I3cMasterInterface,
      public GasketAdapter<iface::I3cMasterInterface> {
  public:
    I3cMasterGasketAdapter(I3cMasterInterface *i3cmaster,
                           iface::SimulationInterface *simulation)
        : i3cmaster_(i3cmaster), simulation_(simulation) {
    }
    void acknowledge(types::i3c_ack_t ack) override {
        Context context(simulation_);
        i3cmaster_->acknowledge(ack);
    }
    void read_response(uint8_t value, bool more) override {
        Context context(simulation_);
        i3cmaster_->read_response(value, more);
    }
    void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) override {
        Context context(simulation_);
        i3cmaster_->daa_response(id, bcr, dcr);
    }
    void ibi_request() override {
        Context context(simulation_);
        i3cmaster_->ibi_request();
    }
    void ibi_address(uint8_t address) override {
        Context context(simulation_);
        i3cmaster_->ibi_address(address);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(i3cmaster_);
    }

  private:
    I3cMasterInterface *i3cmaster_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
