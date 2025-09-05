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

#include <simics/systemc/tlm2simics/pcie_transaction.h>
#include <simics/devs/pci.h>

#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

void PcieTransaction::add_custom_atoms(
        const tlm::tlm_generic_payload *tlm_transaction,
        std::vector<atom_t> *atoms) {
    PcieTlmExtension *pcie_atom_ext = nullptr;
    tlm_transaction->get_extension<PcieTlmExtension>(pcie_atom_ext);
    if (pcie_atom_ext) {
        atoms->push_back(
            ATOM_pcie_type(static_cast<::pcie_type_t>(
                               pcie_atom_ext->type)));
        if (pcie_atom_ext->msg_type_set == true) {
            atoms->push_back(
                ATOM_pcie_msg_type(static_cast<::pcie_message_type_t>(
                                       pcie_atom_ext->msg_type)));
        }
        if (pcie_atom_ext->device_id_set == true) {
            atoms->push_back(ATOM_pcie_device_id(pcie_atom_ext->device_id));
        }
        if (pcie_atom_ext->pasid_set == true) {
            atoms->push_back(ATOM_pcie_pasid(pcie_atom_ext->pasid));
        }
        if (pcie_atom_ext->requester_id_set == true) {
            atoms->push_back(ATOM_pcie_requester_id(
                                 pcie_atom_ext->requester_id));
        }
    }
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
