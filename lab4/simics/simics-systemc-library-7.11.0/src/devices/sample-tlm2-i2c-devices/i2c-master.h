/*                                                              -*- C++ -*-

  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_I2C_DEVICES_I2C_MASTER_H
#define SAMPLE_TLM2_I2C_DEVICES_I2C_MASTER_H

// SystemC and TLM2 includes
#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/iface/extension_dispatcher.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/systemc/iface/i2c_master_v2_extension.h>

#include <simics/systemc/iface/extension_sender.h>
#include <simics/systemc/iface/i2c_slave_v2_extension.h>

namespace sclif = simics::systemc::iface;

class I2cMaster : public sc_core::sc_module,
                  public sclif::I2cMasterV2Interface {
  public:
    SC_CTOR(I2cMaster)
        : i2c_target_socket("i2c_target_socket"),
          i2c_initiator_socket("i2c_initiator_socket"),
          address_(0),
          data_rd_(0),
          data_wr_(0),
          ack_(simics::types::I2C_noack) {
        i2c_target_socket.register_b_transport(
            this, &I2cMaster::i2c_b_transport);

        dispatcher_.subscribe(sclif::I2cMasterV2Extension::createReceiver(this));

        sender_.init(&i2c_initiator_socket);
        extension_.init(&sender_);
    }

    void set_address(uint8_t v);
    uint8_t get_address() const;
    void set_data_rd(uint8_t v);
    uint8_t get_data_rd() const;
    void set_data_wr(uint8_t v);
    uint8_t get_data_wr() const;
    void set_ack(simics::types::i2c_ack_t v);
    simics::types::i2c_ack_t get_ack() const;

    // Incoming sockets
    tlm_utils::simple_target_socket<I2cMaster> i2c_target_socket;
    // Outgoing socket
    tlm_utils::simple_initiator_socket<I2cMaster> i2c_initiator_socket;

  private:
    void i2c_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                         sc_core::sc_time &t);

    // I2cMasterV2Interface
    void acknowledge(simics::types::i2c_ack_t ack);
    void read_response(uint8_t value);

    sclif::ExtensionDispatcher dispatcher_;
    sclif::ExtensionSender<
        tlm_utils::simple_initiator_socket<I2cMaster> > sender_;
    sclif::I2cSlaveV2Extension extension_;

    // 7 bits I2C address and 1 bit for Read/Write
    uint8_t address_;
    uint8_t data_rd_;
    uint8_t data_wr_;
    simics::types::i2c_ack_t ack_;
};

#endif
