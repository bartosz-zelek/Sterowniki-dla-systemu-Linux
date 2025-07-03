// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/simics2tlm/pcie_device.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void PcieDevice::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

void PcieDevice::connected(conf_object_t *port_obj, uint16_t device_id) {
    SIM_LOG_INFO(2, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::connected, device_id = 0x%x", device_id);
    extension_.connected(port_obj, device_id);
}

void PcieDevice::disconnected(conf_object_t *port_obj, uint16_t device_id) {
    SIM_LOG_INFO(2, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::disconnected, device_id = 0x%x", device_id);
    extension_.disconnected(port_obj, device_id);
}

void PcieDevice::hot_reset() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "PciDevice::hot_reset");
    extension_.hot_reset();
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
