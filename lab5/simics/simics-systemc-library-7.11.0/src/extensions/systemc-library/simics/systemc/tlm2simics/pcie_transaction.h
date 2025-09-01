// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef TLM2SIMICS_PCIE_TRANSACTION_H
#define TLM2SIMICS_PCIE_TRANSACTION_H

#include <tlm>

#include <simics/systemc/tlm2simics/transaction.h>
#include <simics/systemc/pcie_tlm_extension.h>

#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

class PcieTransaction : public Transaction {
  public:
    using Transaction::Transaction;

  private:
    void add_custom_atoms(const tlm::tlm_generic_payload *tlm_transaction,
                          std::vector<atom_t> *atoms) override;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
