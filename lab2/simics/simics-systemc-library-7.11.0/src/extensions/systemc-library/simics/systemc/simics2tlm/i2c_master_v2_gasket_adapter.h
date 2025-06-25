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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I2C_MASTER_V2_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I2C_MASTER_V2_GASKET_ADAPTER_H

#include <simics/systemc/context.h>

#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for I2cMasterV2 gasket.
 */
class I2cMasterV2GasketAdapter
    : public iface::I2cMasterV2Interface,
      public GasketAdapter<iface::I2cMasterV2Interface> {
  public:
    I2cMasterV2GasketAdapter(I2cMasterV2Interface *i2cmasterv2,
                             iface::SimulationInterface *simulation)
        : i2cmasterv2_(i2cmasterv2), simulation_(simulation) {
    }
    void acknowledge(types::i2c_ack_t ack) override {
        Context context(simulation_);
        i2cmasterv2_->acknowledge(ack);
    }
    void read_response(uint8_t value) override {
        Context context(simulation_);
        i2cmasterv2_->read_response(value);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(i2cmasterv2_);
    }

  private:
    I2cMasterV2Interface* i2cmasterv2_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
