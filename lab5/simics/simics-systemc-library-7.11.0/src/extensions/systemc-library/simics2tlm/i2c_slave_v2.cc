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

#include <simics/systemc/simics2tlm/i2c_slave_v2.h>
#include <simics/systemc/adapter_log_groups.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

void I2cSlaveV2::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

void I2cSlaveV2::start(uint8_t address) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cSlaveV2::start, address=0x%x", address);
    extension_.start(address);
}

void I2cSlaveV2::read() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cSlaveV2::read");
    extension_.read();
}

void I2cSlaveV2::write(uint8_t data) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cSlaveV2::write, data=0x%x", data);
    extension_.write(data);
}

void I2cSlaveV2::stop() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cSlaveV2::stop");
    extension_.stop();
}

std::vector<uint8_t> I2cSlaveV2::addresses() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I2cSlaveV2::addresses");
    return extension_.addresses();
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
