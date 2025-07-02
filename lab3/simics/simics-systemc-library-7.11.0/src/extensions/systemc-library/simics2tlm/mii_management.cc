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

#include <simics/systemc/simics2tlm/mii_management.h>
#include <simics/systemc/adapter_log_groups.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void MiiManagement::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

int MiiManagement::serial_access(int data_in, int clock) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "MiiManagement::serial_access, data_in=%d clock=%d",
                 data_in, clock);
    return extension_.serial_access(data_in, clock);
}

uint16_t MiiManagement::read_register(int phy, int reg) {
    uint16_t value = extension_.read_register(phy, reg);
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "MiiManagement::read_register, phy=%d, reg=0x%x, value=0x%x",
                 phy, reg, value);
    return value;
}

void MiiManagement::write_register(int phy, int reg, uint16_t value) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "MiiManagement::write_register, phy=%d, reg=0x%x, value=0x%x",
                 phy, reg, value);
    extension_.write_register(phy, reg, value);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

