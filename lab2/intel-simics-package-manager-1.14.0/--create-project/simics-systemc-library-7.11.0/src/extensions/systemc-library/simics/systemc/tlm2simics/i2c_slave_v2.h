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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_I2C_SLAVE_V2_H
#define SIMICS_SYSTEMC_TLM2SIMICS_I2C_SLAVE_V2_H

#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <simics/systemc/interface_provider.h>
#include <simics/systemc/tlm2simics/transaction_handler.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Protocol specific transaction handler for Simics i2c_slave_v2 interface.
 */
class I2cSlaveV2 : public InterfaceProvider,
                   public TransactionHandler,
                   public iface::I2cSlaveV2Interface {
  public:
    I2cSlaveV2() : InterfaceProvider("i2c_slave_v2"),
                   TransactionHandler(this,
                       iface::I2cSlaveV2Extension::createIgnoreReceiver()),
                   receiver_(
                       iface::I2cSlaveV2Extension::createReceiver(this)) {}
    virtual ~I2cSlaveV2();

    // I2cSlaveV2Interface
    void start(uint8_t address) override;
    void read() override;
    void write(uint8_t value) override;
    void stop() override;
    std::vector<uint8_t> addresses() override;
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
