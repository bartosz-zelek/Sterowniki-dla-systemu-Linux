// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_I3C_MASTER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_I3C_MASTER_H

#include <simics/systemc/iface/i3c_master_extension.h>
#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Protocol specific transaction handler for Simics i3c_master interface.
 */
class I3cMaster : public InterfaceProvider,
                  public TransactionHandler,
                  public iface::I3cMasterInterface {
  public:
    I3cMaster() : InterfaceProvider("i3c_master"),
                  TransactionHandler(this,
                      iface::I3cMasterExtension::createIgnoreReceiver()),
                  receiver_(iface::I3cMasterExtension::createReceiver(this)) {}
    virtual ~I3cMaster();

    // I3cMasterInterface
    void acknowledge(types::i3c_ack_t ack) override;
    void read_response(uint8_t value, bool more) override;
    void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) override;
    void ibi_request() override;
    void ibi_address(uint8_t address) override;

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
