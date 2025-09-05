// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_ETHERNET_COMMON_H
#define SIMICS_SYSTEMC_TLM2SIMICS_ETHERNET_COMMON_H

#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/iface/ethernet_common_extension.h>
#include <simics/systemc/iface/ethernet_common_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Protocol specific transaction handler for Simics ethernet_common interface.
 */
class EthernetCommon : public InterfaceProvider,
                       public TransactionHandler,
                       public iface::EthernetCommonInterface {
  public:
    EthernetCommon()
        : InterfaceProvider("ethernet_common"),
          TransactionHandler(this,
              iface::EthernetCommonExtension::createIgnoreReceiver()),
          receiver_(
              iface::EthernetCommonExtension::createReceiver(this)) {
    }
    virtual ~EthernetCommon();

    // EthernetCommonInterface
    void frame(const types::frags_t *frame, int crc) override;
    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

  private:
    tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override;
    iface::ReceiverInterface *receiver_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
