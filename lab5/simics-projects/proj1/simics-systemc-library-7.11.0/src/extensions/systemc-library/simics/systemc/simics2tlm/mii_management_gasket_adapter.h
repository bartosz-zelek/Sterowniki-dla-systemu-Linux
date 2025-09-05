// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_MII_MANAGEMENT_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_MII_MANAGEMENT_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/mii_management_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

class MiiManagementGasketAdapter
    : public iface::MiiManagementInterface,
      public GasketAdapter<iface::MiiManagementInterface> {
  public:
    MiiManagementGasketAdapter(
            MiiManagementInterface *mii_management,
            iface::SimulationInterface *simulation)
        : mii_management_(mii_management),
          simulation_(simulation) {
    }

    int serial_access(int data_in, int clock) override {
        Context context(simulation_);
        return mii_management_->serial_access(data_in, clock);
    }
    uint16_t read_register(int phy, int reg) override {
        Context context(simulation_);
        return mii_management_->read_register(phy, reg);
    }
    void write_register(int phy, int reg, uint16_t value) override {
        Context context(simulation_);
        mii_management_->write_register(phy, reg, value);
    }

    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(mii_management_);
    }

  private:
    MiiManagementInterface *mii_management_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
