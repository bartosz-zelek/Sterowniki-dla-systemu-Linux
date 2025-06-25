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

#include <simics/systemc/simics2tlm/serial_device.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void SerialDevice::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

int SerialDevice::write(int value) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "SerialDevice::write, value=%d", value);
    return extension_.write(value);
}

void SerialDevice::receive_ready() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "SerialDevice::receive_ready");
    extension_.receive_ready();
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
