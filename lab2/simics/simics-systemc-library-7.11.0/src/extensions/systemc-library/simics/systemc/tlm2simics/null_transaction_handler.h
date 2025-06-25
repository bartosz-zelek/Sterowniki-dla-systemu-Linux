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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_NULL_TRANSACTION_HANDLER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_NULL_TRANSACTION_HANDLER_H

#include <simics/systemc/tlm2simics/transaction_handler_interface.h>
#include <simics/systemc/instance_counter.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Utility class that counts the number of instances. This class is used to
 * validate that all transaction handlers have been assigned before running the
 * simulation.
 */
class NullTransactionHandler
    : public TransactionHandlerInterface,
      public InstanceCounter<NullTransactionHandler> {
  public:
    bool get_direct_mem_ptr(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload &trans,
                            tlm::tlm_dmi& dmi_data) override {
        return false;
    }
    tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override {
        return tlm::TLM_OK_RESPONSE;
    }
    unsigned int debug_transaction(ConfObjectRef &simics_obj,
                                   tlm::tlm_generic_payload *trans) override {
        return 0;
    }
    void update_dmi_allowed(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload *trans) override {}
    bool byte_enable_supported(ConfObjectRef &simics_obj,
                               tlm::tlm_generic_payload *trans) override {
        return false;
    }
    const GasketOwner *gasket_owner() const override {
        return NULL;
    }
    const InterfaceProvider *interface_provider() const override {
        return NULL;
    }
    // TransactionHandler
    iface::ReceiverInterface *receiver() override {
        return NULL;
    }
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
