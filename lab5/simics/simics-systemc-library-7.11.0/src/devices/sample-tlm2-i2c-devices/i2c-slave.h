/*                                                              -*- C++ -*-

  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_I2C_DEVICES_I2C_SLAVE_H
#define SAMPLE_TLM2_I2C_DEVICES_I2C_SLAVE_H

// SystemC and TLM2 includes
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/i2c_master_v2_extension.h>
#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>

#include <stdint.h>
#include <vector>

namespace sclif = simics::systemc::iface;

class I2cSlave : public sc_core::sc_module,
                 public sclif::I2cSlaveV2Interface {
  public:
    SC_CTOR(I2cSlave)
        : io_target_socket("io_target_socket"),
          i2c_target_socket("i2c_target_socket"),
          i2c_master_initiator_socket("i2c_master_initiator_socket"),
          register_(0xdeadbeef),
          i2c_address_(0x12) {
        io_target_socket.register_b_transport(
            this, &I2cSlave::io_b_transport);
        i2c_target_socket.register_b_transport(
            this, &I2cSlave::i2c_b_transport);

        dispatcher_.subscribe(sclif::I2cSlaveV2Extension::createReceiver(this));
        sender_.init(&i2c_master_initiator_socket);
        extension_.init(&sender_);
    }

    void set_register(int v);
    int get_register() const;
    void set_address(int v);
    int get_address() const;

    // Incoming sockets
    tlm_utils::simple_target_socket<I2cSlave> io_target_socket;
    tlm_utils::simple_target_socket<I2cSlave> i2c_target_socket;
    // Outgoing socket
    tlm_utils::simple_initiator_socket<I2cSlave> i2c_master_initiator_socket;
    
  private:
    void i2c_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                         sc_core::sc_time &t);
    void io_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                        sc_core::sc_time &t);
    
    // I2cSlaveV2Interface
    void start(uint8_t address);
    void read();
    void write(uint8_t value);
    void stop();
    std::vector<uint8_t> addresses();

    sclif::ExtensionDispatcher dispatcher_;
    sclif::ExtensionSender<
        tlm_utils::simple_initiator_socket<I2cSlave> > sender_;
    sclif::I2cMasterV2Extension extension_;
    
    int register_;
    uint8_t i2c_address_;
};

#endif
