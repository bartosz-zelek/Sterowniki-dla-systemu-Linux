// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_PCI_EXPRESS_H
#define SIMICS_SYSTEMC_TLM2SIMICS_PCI_EXPRESS_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

class PciExpress : public InterfaceProvider,
                   public TransactionHandler,
                   public iface::PciExpressInterface {
  public:
    PciExpress()
        : InterfaceProvider("pci_express"),
          TransactionHandler(
              this,
              iface::PciExpressExtension::createIgnoreReceiver()),
          receiver_(iface::PciExpressExtension::createReceiver(this)),
          device_(NULL) {
    }
    virtual ~PciExpress();

    int send_message(int type, const std::vector<uint8_t> &payload) override;
    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

  private:
    tlm::tlm_response_status simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) override;
    iface::ReceiverInterface *receiver_;

    conf_object_t *device_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
#endif
