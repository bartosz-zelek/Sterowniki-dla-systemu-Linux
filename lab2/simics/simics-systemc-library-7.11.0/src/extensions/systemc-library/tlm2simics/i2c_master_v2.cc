// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/tlm2simics/i2c_master_v2.h>
#include <simics/types/i2c_ack.h>
#include <simics/devs/i2c.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

I2cMasterV2::~I2cMasterV2() {
    delete receiver_;
}

void I2cMasterV2::acknowledge(types::i2c_ack_t ack) {
    get_interface<i2c_master_v2_interface_t>()->acknowledge(
            target().object(), ack == types::I2C_ack ? ::I2C_ack : ::I2C_noack);
}

void I2cMasterV2::read_response(uint8_t value) {
    get_interface<i2c_master_v2_interface_t>()->read_response(
        target().object(), value);
}

iface::ReceiverInterface *I2cMasterV2::receiver() {
    return receiver_;
}

tlm::tlm_response_status I2cMasterV2::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected I2cMasterV2Extension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
