// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

TransactionHandler::TransactionHandler(
        InterfaceProvider *interface_provider,
        iface::ReceiverInterface *ignore_receiver,
        InterfaceProvider *error_interface_provider)
    : interface_provider_(interface_provider),
      error_transaction_handler_(error_interface_provider ?
          error_interface_provider : interface_provider,
          this, ignore_receiver) {
    // Don't call interface_provider_ from here. In general the object will not
    // be fully constructed, so it is only safe to store a pointer to it.
}

bool TransactionHandler::get_direct_mem_ptr(ConfObjectRef &simics_obj,
                                            tlm::tlm_generic_payload &trans,
                                            tlm::tlm_dmi& dmi_data) {
        return false;
}

unsigned int TransactionHandler::debug_transaction(
        ConfObjectRef &simics_obj, tlm::tlm_generic_payload *trans) {
    // No debug support
    return 0;
}

void TransactionHandler::update_dmi_allowed(ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    trans->set_dmi_allowed(false);
}

bool TransactionHandler::byte_enable_supported(ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    return false;
}

void TransactionHandler::set_gasket(GasketInterface::Ptr gasketInterface) {
    GasketOwner::set_gasket(gasketInterface);
    gasketInterface->set_transaction_handler(&error_transaction_handler_);
}

const GasketOwner *TransactionHandler::gasket_owner() const {
    return this;
}

const InterfaceProvider *TransactionHandler::interface_provider() const {
    return interface_provider_;
}

TransactionHandler::~TransactionHandler() {
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
