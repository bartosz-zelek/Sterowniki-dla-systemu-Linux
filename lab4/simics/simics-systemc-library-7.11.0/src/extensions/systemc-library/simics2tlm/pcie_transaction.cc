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

#include <simics/devs/pci.h>
#include <simics/systemc/simics2tlm/pcie_transaction.h>
#include <simics/types/pcie_type.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void PcieTransaction::update_transaction(
        const transaction_t *simics_transaction,
        tlm::tlm_generic_payload *tlm_transaction) {
    pcie_ext.reset();

    pcie_ext.type = static_cast<types::pcie_type_t>(
        ATOM_get_transaction_pcie_type(simics_transaction));

    if (ATOM_transaction_pcie_msg_type(simics_transaction)) {
        pcie_ext.msg_type_set = true;
        pcie_ext.msg_type = static_cast<types::pcie_message_type_t>(
            ATOM_get_transaction_pcie_msg_type(simics_transaction));
    }

    if (ATOM_transaction_pcie_device_id(simics_transaction)) {
        pcie_ext.device_id_set = true;
        pcie_ext.device_id = ATOM_get_transaction_pcie_device_id(
            simics_transaction);
    }

    if (ATOM_transaction_pcie_pasid(simics_transaction)) {
        pcie_ext.pasid_set = true;
        pcie_ext.pasid = ATOM_get_transaction_pcie_pasid(
            simics_transaction);
    }

    if (ATOM_transaction_pcie_requester_id(simics_transaction)) {
        pcie_ext.requester_id_set = true;
        pcie_ext.requester_id = ATOM_get_transaction_pcie_requester_id(
            simics_transaction);
    }

    if (ATOM_transaction_pcie_ide_secured(simics_transaction)) {
        pcie_ext.ide_secured_set = true;
        auto simics_ide_secured = ATOM_get_transaction_pcie_ide_secured(
                simics_transaction);
        pcie_ext.ide_secured = {
            simics_ide_secured->t,
            simics_ide_secured->k,
            simics_ide_secured->m,
            simics_ide_secured->p,
            simics_ide_secured->sub_stream,
            simics_ide_secured->stream_id,
            simics_ide_secured->pr_sent_counter
        };
    }

    tlm_transaction->set_extension(PcieTlmExtension::ID, pcie_ext.clone());
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
