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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I2C_SLAVE_V2_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I2C_SLAVE_V2_GASKET_ADAPTER_H

#include <simics/systemc/context.h>

#include <simics/systemc/iface/i2c_slave_v2_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for I2cSlaveV2 gasket.
 */
class I2cSlaveV2GasketAdapter
    : public iface::I2cSlaveV2Interface,
      public GasketAdapter<iface::I2cSlaveV2Interface> {
  public:
    I2cSlaveV2GasketAdapter(I2cSlaveV2Interface *i2cslavev2,
                            iface::SimulationInterface *simulation)
        : i2cslavev2_(i2cslavev2), simulation_(simulation) {
    }
    void start(uint8_t address) override {
        Context context(simulation_);
        i2cslavev2_->start(address);
    }
    void read() override {
        Context context(simulation_);
        i2cslavev2_->read();
    }
    void write(uint8_t value) override {
        Context context(simulation_);
        i2cslavev2_->write(value);
    }
    void stop() override {
        Context context(simulation_);
        i2cslavev2_->stop();
    }
    std::vector<uint8_t> addresses() override {
        Context context(simulation_);
        return i2cslavev2_->addresses();
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(i2cslavev2_);
    }

  private:
    I2cSlaveV2Interface *i2cslavev2_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
