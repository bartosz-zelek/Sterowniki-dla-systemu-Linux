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

#include <simics/systemc/tlm2simics/i2c_slave_v2.h>
#include <simics/devs/i2c.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

I2cSlaveV2::~I2cSlaveV2() {
    delete receiver_;
}

void I2cSlaveV2::start(uint8_t address) {
    get_interface<i2c_slave_v2_interface_t>()->start(
        target().object(), address);
}
void I2cSlaveV2::read() {
    get_interface<i2c_slave_v2_interface_t>()->read(target().object());
}
void I2cSlaveV2::write(uint8_t value) {
    get_interface<i2c_slave_v2_interface_t>()->write(
        target().object(), value);
}
void I2cSlaveV2::stop() {
    get_interface<i2c_slave_v2_interface_t>()->stop(target().object());
}
std::vector<uint8_t> I2cSlaveV2::addresses() {
    std::vector<uint8_t> ret;
    attr_value_t val = get_interface<i2c_slave_v2_interface_t>()->addresses(
        target().object());
    for (unsigned i = 0; i < SIM_attr_list_size(val); ++i)
        ret.push_back(static_cast<uint8_t>(
                          SIM_attr_integer(SIM_attr_list_item(val, i))));
    return ret;
}

iface::ReceiverInterface *I2cSlaveV2::receiver() {
    return receiver_;
}

tlm::tlm_response_status I2cSlaveV2::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected I2cSlaveV2Extension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
