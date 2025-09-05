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

#include <simics/systemc/tlm2simics/i3c_master.h>
#include <simics/devs/i3c.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

I3cMaster::~I3cMaster() {
    delete receiver_;
}

void I3cMaster::acknowledge(types::i3c_ack_t ack) {
    get_interface<i3c_master_interface_t>()->acknowledge(
            target().object(), ack == types::I3C_ack ? ::I3C_ack : ::I3C_noack);
}

void I3cMaster::read_response(uint8_t value, bool more) {
    get_interface<i3c_master_interface_t>()->read_response(
            target().object(), value, more);
}

void I3cMaster::daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) {
    get_interface<i3c_master_interface_t>()->daa_response(
            target().object(), id, bcr, dcr);
}

void I3cMaster::ibi_request() {
    get_interface<i3c_master_interface_t>()->ibi_request(
            target().object());
}

void I3cMaster::ibi_address(uint8_t address) {
    get_interface<i3c_master_interface_t>()->ibi_address(
            target().object(), address);
}


iface::ReceiverInterface *I3cMaster::receiver() {
    return receiver_;
}

tlm::tlm_response_status I3cMaster::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected I3cMasterExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
