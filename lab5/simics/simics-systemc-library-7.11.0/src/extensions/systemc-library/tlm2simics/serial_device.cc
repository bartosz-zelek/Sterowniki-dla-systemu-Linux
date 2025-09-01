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

#include <simics/systemc/tlm2simics/serial_device.h>
#include <simics/devs/serial-device.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

int SerialDevice::write(int value) {
    return get_interface<serial_device_interface_t>()->write(target().object(),
                                                             value);
}

void SerialDevice::receive_ready() {
    get_interface<serial_device_interface_t>()->receive_ready(
        target().object());
}

iface::ReceiverInterface *SerialDevice::receiver() {
    return receiver_;
}

SerialDevice::~SerialDevice() {
    delete receiver_;
}

tlm::tlm_response_status SerialDevice::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected SerialDeviceExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
