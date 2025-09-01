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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_DMI_TRANSACTION_HANDLER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_DMI_TRANSACTION_HANDLER_H

#include <simics/model-iface/direct-memory.h>

#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Base class for transaction handlers that support DMI.
 */
class DmiTransactionHandler
    : public TransactionHandler,
      public InterfaceProvider::TargetUpdateListener {
  public:
    DmiTransactionHandler(InterfaceProvider *interface_provider,
                          iface::ReceiverInterface *ignore_receiver = NULL,
                          InterfaceProvider *dmi_interface_provider = NULL);
    virtual ~DmiTransactionHandler();

    // TransactionHandler
    bool get_direct_mem_ptr(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload &trans,
                            tlm::tlm_dmi& dmi_data) override;
    void update_dmi_allowed(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload *trans) override;

    // TargetUpdateListener
    void update_target(ConfObjectRef old_target,
                       ConfObjectRef new_target) override;

  protected:
    SimicsTargetLock<const direct_memory_interface_t> direct_memory(
        ConfObjectRef target);
    SimicsTargetLock<const direct_memory_lookup_interface_t>
        direct_memory_lookup();

  private:
    bool hasDirectMemoryUpdateInterface(ConfObjectRef simics_obj);

    InterfaceProvider direct_memory_provider_;
    InterfaceProvider direct_memory_lookup_provider_;
    bool has_direct_memory_update_iface_checked_;
    bool has_direct_memory_update_iface_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
