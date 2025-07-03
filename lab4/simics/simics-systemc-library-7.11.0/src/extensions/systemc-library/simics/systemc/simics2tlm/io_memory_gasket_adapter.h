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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_IO_MEMORY_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_IO_MEMORY_GASKET_ADAPTER_H

#include <simics/systemc/context.h>

#include <simics/systemc/iface/io_memory_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for IoMemory gasket.
 */
class IoMemoryGasketAdapter
    : public iface::IoMemoryInterface,
      public GasketAdapter<iface::IoMemoryInterface> {
  public:
    IoMemoryGasketAdapter(IoMemoryInterface *iomemory,
                          iface::SimulationInterface *simulation)
        : iomemory_(iomemory), simulation_(simulation) {
    }

    exception_type_t operation(generic_transaction_t *mem_op,
                               const types::map_info_t &info) override {
        Context context(simulation_);
        exception_type_t ret = iomemory_->operation(mem_op, info);
        return ret;
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(iomemory_);
    }

  private:
    IoMemoryInterface *iomemory_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
