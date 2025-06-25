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

#include <simics/systemc/tlm2simics/i3c_slave.h>
#include <simics/devs/i3c.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

I3cSlave::~I3cSlave() {
    delete receiver_;
}

void I3cSlave::start(uint8_t address) {
    get_interface<i3c_slave_interface_t>()->start(
            target().object(), address);
}

void I3cSlave::write(uint8_t value) {
    get_interface<i3c_slave_interface_t>()->write(
            target().object(), value);
}

void I3cSlave::sdr_write(types::bytes_t data) {
    ::bytes_t bytes;
    bytes.data = data.data;
    bytes.len = data.len;
    get_interface<i3c_slave_interface_t>()->sdr_write(
            target().object(), bytes);
}

void I3cSlave::read() {
    get_interface<i3c_slave_interface_t>()->read(target().object());
}

void I3cSlave::daa_read() {
    get_interface<i3c_slave_interface_t>()->daa_read(target().object());
}

void I3cSlave::stop() {
    get_interface<i3c_slave_interface_t>()->stop(target().object());
}

void I3cSlave::ibi_start() {
    get_interface<i3c_slave_interface_t>()->ibi_start(target().object());
}

void I3cSlave::ibi_acknowledge(types::i3c_ack_t ack) {
    get_interface<i3c_slave_interface_t>()->ibi_acknowledge(
            target().object(), ack == types::I3C_ack ? ::I3C_ack : ::I3C_noack);
}

iface::ReceiverInterface *I3cSlave::receiver() {
    return receiver_;
}

tlm::tlm_response_status I3cSlave::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected I3cSlaveExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
