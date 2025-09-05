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
#include <simics/cc-api.h>
#include <simics/c++/model-iface/transaction.h>

class PcieMapHelper : public simics::ConfObject,
                      public simics::iface::TransactionInterface {
  public:
    using ConfObject::ConfObject;

    static void init_class(simics::ConfClass *cls);
    exception_type_t issue(transaction_t *t, uint64 addr) override;
    void set_pcie_type(pcie_type_t pcie_type);

    pcie_type_t pcie_type_ {PCIE_Type_Not_Set};
    int function_id_ {0};
    simics::Connect<simics::iface::TransactionInterface> forward_target_;
};

void PcieMapHelper::init_class(simics::ConfClass *cls) {
    cls->add(simics::iface::TransactionInterface::Info());
    cls->add(simics::Attribute("pcie_type", "i",
                               "The PCIe type used when forwarding the "
                               "transaction to the forward target",
                               ATTR_GETTER(PcieMapHelper, pcie_type_),
                               ATTR_SETTER(PcieMapHelper, set_pcie_type),
                               Sim_Attr_Required));
    cls->add(simics::Attribute("forward_target", "o",
                               "The object used to forward the transaction",
                               ATTR_CLS_VAR(PcieMapHelper, forward_target_),
                               Sim_Attr_Required));
    cls->add(simics::Attribute("function_id", "i",
                               "The function ID used when forwarding the "
                               "transaction to the forward target",
                               ATTR_CLS_VAR(PcieMapHelper, function_id_)));
}

exception_type_t PcieMapHelper::issue(transaction_t *t, uint64 addr) {
    atom_t atoms[3] {
        ATOM_pcie_type(pcie_type_),
        ATOM_list_end(0)
    };

    if (pcie_type_ == PCIE_Type_Cfg && !ATOM_transaction_pcie_device_id(t)) {
        // Attach the function_id if the transaction does not already have
        // pcie_device_id set and it is of type CFG
        atoms[1] = ATOM_pcie_device_id(function_id_);
        atoms[2] = ATOM_list_end(0);
    }

    transaction_t new_t = { .atoms = atoms, .prev = t };
    return forward_target_.iface().issue(&new_t, addr);
}

void PcieMapHelper::set_pcie_type(pcie_type_t pcie_type) {
    if (pcie_type == PCIE_Type_Not_Set) {
        throw std::invalid_argument("Valid PCIe type is required");
    }
    pcie_type_ = pcie_type;
}

extern "C" void init_local() {
    auto cls = simics::make_class<PcieMapHelper>(
            "pcie_map_helper_cpp",
            "PCIe map helper C++ device",
            "PCIe map helper which is mapped at a PCIe memory space. When "
            "accessed, the transaction is forwarded to the forward target "
            "with PCIe type ATOM set");
}
