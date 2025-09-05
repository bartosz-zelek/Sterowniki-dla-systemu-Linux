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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_PCIE_TRANSACTION_H
#define SIMICS_SYSTEMC_SIMICS2TLM_PCIE_TRANSACTION_H

#include <tlm>

#include <simics/systemc/pcie_tlm_extension.h>
#include <simics/systemc/simics2tlm/transaction.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/*
 * Transaction with PCIe extension
 *
 * The various bits of information required to complete an operation (Type,
 * BDF number, etc.) are attached to the transaction_t as atoms.
 *
 * Retrieve these information from atom and convert it to TLM PcieTlmExtension
 */
class PcieTransaction : public Transaction {
    void update_transaction(
            const transaction_t *simics_transaction,
            tlm::tlm_generic_payload *tlm_transaction) override;

    PcieTlmExtension pcie_ext;
};
}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
