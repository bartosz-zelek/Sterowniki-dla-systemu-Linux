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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_I3C_SLAVE_H
#define SIMICS_SYSTEMC_TLM2SIMICS_I3C_SLAVE_H

#include <simics/systemc/iface/i3c_slave_extension.h>
#include <simics/systemc/iface/i3c_slave_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Protocol specific transaction handler for Simics i3c_slave interface.
 */
class I3cSlave : public InterfaceProvider,
                 public TransactionHandler,
                 public iface::I3cSlaveInterface {
  public:
    I3cSlave() : InterfaceProvider("i3c_slave"),
                 TransactionHandler(this,
                     iface::I3cSlaveExtension::createIgnoreReceiver()),
                 receiver_(iface::I3cSlaveExtension::createReceiver(this)) {}
    virtual ~I3cSlave();

    // I3cSlaveInterface
    void start(uint8_t address) override;
    void write(uint8_t value) override;
    void sdr_write(types::bytes_t data) override;
    void read() override;
    void daa_read() override;
    void stop() override;
    void ibi_start() override;
    void ibi_acknowledge(types::i3c_ack_t ack) override;
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
