// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/simics2tlm/pci_device.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void PciDevice::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

void PciDevice::bus_reset() {
    SIM_LOG_INFO(2, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::bus_reset");
    extension_.bus_reset();
}

void PciDevice::system_error() {
    SIM_LOG_INFO(2, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::system_error");
    extension_.system_error();
}

void PciDevice::interrupt_raised(int pin) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::interrupt_raised, pin=%d", pin);
    extension_.interrupt_raised(pin);
}

void PciDevice::interrupt_lowered(int pin) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::interrupt_lowered, pin=%d", pin);
    extension_.interrupt_lowered(pin);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
