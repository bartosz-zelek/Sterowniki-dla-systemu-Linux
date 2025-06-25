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

#include <simics/base/transaction.h>
#include <simics/model-iface/transaction.h>
#include <simics/systemc/tlm2simics/transaction.h>
#include <simics/systemc/tlm2simics/non_blocking_tlm_extension.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/iface/simics_adapter.h>
#include <systemc-interfaces.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {
inline transaction_flags_t operator| (transaction_flags_t f1,
                                      transaction_flags_t f2) {
    return static_cast<transaction_flags_t>(static_cast<int>(f1) |
                                            static_cast<int>(f2));
}

// Function registered in ATOM_completion and get called when Simics
// side calls SIM_complete_transaction
exception_type_t completion(conf_object_t *obj, transaction_t *t,
                            exception_type_t ex) {
    auto *bw_if = static_cast<const sc_tlm_bw_transport_interface_t *>(
            SIM_c_get_interface(obj, "sc_tlm_bw_transport"));
    attr_value_t attr = SIM_make_attr_nil();

    /* We know converting to attr and back is bad, but this use-case is
       currently used by external simulators that already have other
       performance problems so this is an acceptable cost. The proper
       solution is to add a new sc_transaction_completion interface with
       a completion method that we can use instead, without converting the
       payload into an attribute */
    if (SIM_transaction_is_read(t)) {
        attr = SIM_alloc_attr_dict(1);
        std::vector<unsigned char> data_(SIM_transaction_size(t));
        buffer_t buffer_ {&data_.front(), data_.size()};
        SIM_get_transaction_bytes(t, buffer_);
        SIM_attr_dict_set_item(&attr, 0, SIM_make_attr_string("gp.data_ptr"),
                               SIM_make_attr_data(buffer_.len, buffer_.data));
    }

    bw_if->nb_transport_bw(obj, attr, tlm::BEGIN_RESP, 0);
    delete[] t->atoms;
    delete t;
    return ex;
}
}  // namespace

namespace simics {
namespace systemc {
namespace tlm2simics {

Transaction::Transaction()
    : InterfaceProvider("transaction"), DmiTransactionHandler(this) {
    add_target_update_listener(&update_target_);
}

Transaction::~Transaction() {
    remove_target_update_listener(&update_target_);
}

unsigned int Transaction::transaction(ConfObjectRef &simics_obj,
                                      tlm::tlm_generic_payload *trans,
                                      bool inquiry) {
    if (!trans->is_read() && !trans->is_write()) {
        SIM_LOG_SPEC_VIOLATION(1, simics_obj, Log_TLM,
                               "GP must be read or write transaction");
        trans->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    NonBlockingTlmExtension *non_blocking = nullptr;
    trans->get_extension(non_blocking);

    transaction_flags_t flags = trans->is_write() ? Sim_Transaction_Write
        : static_cast<transaction_flags_t>(0);
    if (inquiry)
        flags = flags | Sim_Transaction_Inquiry;

    uint64 addr = trans->get_address();
    uint8 *data = static_cast<uint8 *>(trans->get_data_ptr());
    uint32 size = trans->get_data_length();

    if (non_blocking && target_socket_proxy_obj_ == nullptr) {
        std::string target_socket_name =
            simics_obj.name() + "." + \
            gasket()->gasket_name() + ".target_socket";
        target_socket_proxy_obj_ = SIM_get_object(target_socket_name.c_str());
        if (target_socket_proxy_obj_ == nullptr) {
            SIM_LOG_ERROR(simics_obj, Log_TLM,
                          "Cann't find the target socket proxy object");
        }
    }

    std::vector<atom_t> atoms {
        ATOM_flags(flags),
        ATOM_data(data),
        ATOM_size(size),
    };
    // Deferrable transaction is only supported in NB transport
    if (non_blocking) {
        atoms.push_back(ATOM_initiator(target_socket_proxy_obj_));
        atoms.push_back(ATOM_completion(completion));
    }
    add_custom_atoms(trans, &atoms);
    atoms.push_back(ATOM_LIST_END);

    transaction_t t { atoms.data() };
    transaction_t *tp = &t;
    if (non_blocking) {
        // The transaction and atoms are allocated on the heap to ensure
        // that the transaction remains valid until completion.
        auto *newAtoms = new atom_t[atoms.size()];
        std::move(atoms.begin(), atoms.end(), newAtoms);
        tp = new transaction_t {newAtoms};
    }

    exception_type_t ex = SIM_issue_transaction(update_target_.map_target(),
                                                tp, addr);
    if (ex == Sim_PE_Deferred) {
        SIM_LOG_INFO(3, simics_obj, Log_TLM, "Transaction is deferred");
        SIM_monitor_transaction(tp, ex);

        trans->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
        return 0;
    }

    if (non_blocking) {
        delete[] tp->atoms;
        delete tp;
    }

    if (ex != Sim_PE_No_Exception) {
        SIM_LOG_ERROR(simics_obj, Log_TLM,
                      "%s memory address 0x%llx, size 0x%x",
                      trans->is_read() ? "reading" : "writing", addr, size);

        trans->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);

        return 0;
    }

    trans->set_response_status(tlm::TLM_OK_RESPONSE);
    return size;
}

void Transaction::set_gasket(GasketInterface::Ptr gasketInterface) {
    gasketInterface->register_nb_transport_fw();
    // coverity[copy_instead_of_move:SUPPRESS] Ptr is a pointer
    DmiTransactionHandler::set_gasket(gasketInterface);
}

tlm::tlm_response_status Transaction::simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    transaction(simics_obj, trans, false);
    return trans->get_response_status();
}

unsigned int Transaction::debug_transaction(ConfObjectRef &simics_obj,  // NOLINT
                                            tlm::tlm_generic_payload *trans) {
    return transaction(simics_obj, trans, true);
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
