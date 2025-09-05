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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I3C_SLAVE_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I3C_SLAVE_GASKET_ADAPTER_H

#include <simics/systemc/context.h>

#include <simics/systemc/iface/i3c_slave_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for I3cSlave gasket.
 */
class I3cSlaveGasketAdapter
    : public iface::I3cSlaveInterface,
      public GasketAdapter<iface::I3cSlaveInterface> {
  public:
    I3cSlaveGasketAdapter(I3cSlaveInterface *i3cslave,
                           iface::SimulationInterface *simulation)
        : i3cslave_(i3cslave), simulation_(simulation) {
    }

    void start(uint8_t address) override {
        Context context(simulation_);
        i3cslave_->start(address);
    }
    void write(uint8_t value) override {
        Context context(simulation_);
        i3cslave_->write(value);
    }
    void sdr_write(types::bytes_t data) override {
        Context context(simulation_);
        i3cslave_->sdr_write(data);
    }
    void read() override {
        Context context(simulation_);
        i3cslave_->read();
    }
    void daa_read() override {
        Context context(simulation_);
        i3cslave_->daa_read();
    }
    void stop() override {
        Context context(simulation_);
        i3cslave_->stop();
    }
    void ibi_start() override {
        Context context(simulation_);
        i3cslave_->ibi_start();
    }
    void ibi_acknowledge(types::i3c_ack_t ack) override {
        Context context(simulation_);
        i3cslave_->ibi_acknowledge(ack);
    }

    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(i3cslave_);
    }

  private:
    I3cSlaveInterface *i3cslave_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
