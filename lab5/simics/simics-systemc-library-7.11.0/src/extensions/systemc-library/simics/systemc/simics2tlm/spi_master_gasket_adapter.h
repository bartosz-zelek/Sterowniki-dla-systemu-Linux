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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_SPI_MASTER_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_SPI_MASTER_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/iface/spi_master_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for SpiMaster gasket.
 */
class SpiMasterGasketAdapter
    : public iface::SpiMasterInterface,
      public GasketAdapter<iface::SpiMasterInterface> {
  public:
    SpiMasterGasketAdapter(SpiMasterInterface *spimaster,
                           iface::SimulationInterface *simulation)
        : spimaster_(spimaster), simulation_(simulation) {
    }
    virtual void spi_response(const uint8 *data_ptr, size_t data_length) {
        Context context(simulation_);
        spimaster_->spi_response(data_ptr, data_length);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(spimaster_);
    }

  private:
    SpiMasterInterface *spimaster_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
