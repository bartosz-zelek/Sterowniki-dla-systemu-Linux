// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/tlm2simics/error_transaction_handler.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

ErrorTransactionHandler::ErrorTransactionHandler(
        InterfaceProvider *interface_provider,
        TransactionHandlerInterface *transaction_handler,
        iface::ReceiverInterface *receiver) :
    interface_provider_(interface_provider),
    transaction_handler_(transaction_handler),
    receiver_(receiver) {
}

bool ErrorTransactionHandler::get_direct_mem_ptr(ConfObjectRef &simics_obj,  // NOLINT
        tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi_data) {
    if (interface_provider_->has_interface())
        return transaction_handler_->get_direct_mem_ptr(simics_obj, trans,
                                                        dmi_data);
    log_error(&simics_obj, "TLM get_direct_mem_ptr abort.");
    return false;
}

tlm::tlm_response_status ErrorTransactionHandler::simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (interface_provider_->has_interface()) {
        return transaction_handler_->simics_transaction(simics_obj, trans);
    }
    error(&simics_obj, "TLM b_transport abort.", trans);
    return trans->get_response_status();
}

unsigned int ErrorTransactionHandler::debug_transaction(ConfObjectRef &simics_obj,  // NOLINT
        tlm::tlm_generic_payload *trans) {
    if (interface_provider_->has_interface())
        return transaction_handler_->debug_transaction(simics_obj, trans);
    error(&simics_obj, "TLM transport_dbg abort.", trans);
    return 0;
}

void ErrorTransactionHandler::update_dmi_allowed(ConfObjectRef &simics_obj,  // NOLINT
        tlm::tlm_generic_payload *trans) {
    transaction_handler_->update_dmi_allowed(simics_obj, trans);
}

bool ErrorTransactionHandler::byte_enable_supported(ConfObjectRef &simics_obj,  // NOLINT
        tlm::tlm_generic_payload *trans) {
    if (transaction_handler_->byte_enable_supported(simics_obj, trans))
        return true;

    if (receiver_)
        (void) receiver_->handle(trans);

    SIM_LOG_INFO(4, simics_obj, Log_TLM,
                 "Gasket does not support TLM byte enable.");
    trans->set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
    return false;
}

ErrorTransactionHandler::~ErrorTransactionHandler() {
    delete receiver_;
}

void ErrorTransactionHandler::error(ConfObjectRef *simics_obj,
                                    const char *message,
                                    tlm::tlm_generic_payload *trans) {
    if (receiver_)
        (void) receiver_->handle(trans);

    log_error(simics_obj, message);
}

void ErrorTransactionHandler::log_error(ConfObjectRef *simics_obj,
                                        const char *message) {
    if (interface_provider_->target().object() == NULL) {
        if (interface_provider_->optional())
            return;
        SIM_LOG_ERROR(*simics_obj, Log_TLM,
            "%s The %s interface provider is not configured (no object).",
            message, interface_provider_->get_interface_name());
    } else {
        SIM_LOG_ERROR(*simics_obj, Log_TLM,
            "%s The object %s does not implement the %s interface.",
            message, SIM_object_name(interface_provider_->target().object()),
            interface_provider_->get_interface_name());
    }
}

const GasketOwner *ErrorTransactionHandler::gasket_owner() const {
    return NULL;
}

const InterfaceProvider *ErrorTransactionHandler::interface_provider() const {
    return NULL;
}

iface::ReceiverInterface *ErrorTransactionHandler::receiver() {
    return transaction_handler_->receiver();
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
