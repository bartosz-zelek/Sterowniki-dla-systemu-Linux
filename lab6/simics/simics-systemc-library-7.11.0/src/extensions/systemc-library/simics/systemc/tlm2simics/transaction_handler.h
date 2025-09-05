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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_TRANSACTION_HANDLER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_TRANSACTION_HANDLER_H

#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/registry.h>
#include <simics/systemc/tlm2simics/error_transaction_handler.h>
#include <simics/systemc/tlm2simics/gasket_owner.h>
#include <simics/systemc/tlm2simics/transaction_handler_interface.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Base class for transaction handlers without support for DMI.
 * Most protocol specific transaction handlers are derived from this class.
 */
class TransactionHandler
    : public GasketOwner,
      public virtual Registrant<TransactionHandlerInterface> {
  public:
    TransactionHandler(InterfaceProvider *interface_provider,
                       iface::ReceiverInterface *ignore_receiver = NULL,
                       InterfaceProvider *error_interface_provider = NULL);
    virtual ~TransactionHandler();

    // GasketOwner
    void set_gasket(GasketInterface::Ptr gasketInterface) override;

    // TransactionHandlerInterface
    bool get_direct_mem_ptr(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload &trans,
                            tlm::tlm_dmi& dmi_data) override;
    unsigned int debug_transaction(ConfObjectRef &simics_obj,
                                   tlm::tlm_generic_payload *trans) override;
    void update_dmi_allowed(ConfObjectRef &simics_obj,
                            tlm::tlm_generic_payload *trans) override;
    bool byte_enable_supported(ConfObjectRef &simics_obj,
                               tlm::tlm_generic_payload *trans) override;
    const GasketOwner *gasket_owner() const override;
    const InterfaceProvider *interface_provider() const override;

  protected:
    InterfaceProvider *interface_provider_;
    ErrorTransactionHandler error_transaction_handler_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
