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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_TRANSACTION_GASKET_ADAPTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_TRANSACTION_GASKET_ADAPTER_H

#include <simics/systemc/context.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/iface/transaction_interface.h>
#include <simics/systemc/simics2tlm/gasket_adapter.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Adapter for Transaction gasket.
 */
class TransactionGasketAdapter
    : public iface::TransactionInterface,
      public GasketAdapter<iface::TransactionInterface> {
  public:
    TransactionGasketAdapter(TransactionInterface *transaction,
                             iface::SimulationInterface *simulation)
        : transaction_(transaction), simulation_(simulation) {
    }
    exception_type_t issue(transaction_t *t, uint64 addr) override {
        Context context(simulation_);
        return transaction_->issue(t, addr);
    }
    simics2tlm::GasketOwner *gasket_owner() const override {
        return dynamic_cast<simics2tlm::GasketOwner *>(transaction_);
    }

  private:
    TransactionInterface *transaction_;
    iface::SimulationInterface *simulation_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
