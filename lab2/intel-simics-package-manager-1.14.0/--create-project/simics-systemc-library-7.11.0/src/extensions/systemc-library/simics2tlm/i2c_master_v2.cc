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

#include <simics/systemc/simics2tlm/i2c_master_v2.h>
#include <simics/systemc/adapter_log_groups.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void I2cMasterV2::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

void I2cMasterV2::acknowledge(types::i2c_ack_t ack) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cMasterV2::acknowledge, ack=%s",
                 ack == types::I2C_ack ? "I2C_ack" : "I2C_noack");
    extension_.acknowledge(ack);
}

void I2cMasterV2::read_response(uint8_t value) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cMasterV2::read_response, value=0x%x", value);
    extension_.read_response(value);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

