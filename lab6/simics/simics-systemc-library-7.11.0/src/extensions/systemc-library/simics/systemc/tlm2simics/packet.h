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

#ifndef TLM2SIMICS_PACKET_H
#define TLM2SIMICS_PACKET_H

#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/** Protocol specific transaction handler for Simics packet interface.
 */
class Packet : public InterfaceProvider,
                    public TransactionHandler {
  public:
    Packet() : InterfaceProvider("packet"), TransactionHandler(this) {}
    // TransactionHandler
    iface::ReceiverInterface *receiver() override;

  private:
    unsigned int transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                             tlm::tlm_generic_payload *trans, bool inquiry);
    tlm::tlm_response_status simics_transaction(
            simics::ConfObjectRef &simics_obj,
            tlm::tlm_generic_payload *trans) override;
    unsigned int debug_transaction(simics::ConfObjectRef &simics_obj,
                                   tlm::tlm_generic_payload *trans) override;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
