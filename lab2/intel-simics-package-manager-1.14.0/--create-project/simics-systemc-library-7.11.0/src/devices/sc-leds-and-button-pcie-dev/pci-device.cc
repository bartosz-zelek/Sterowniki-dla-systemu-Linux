/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "pci-device.h"

#include <simics/systemc/utility.h>

#include <sstream>
#include <string>
#include <vector>
#include <utility>

// Work around bug in the SC kernel not using correct namespace
using sc_core::sc_report_handler;
using sc_core::SC_INFO;

namespace {
const char* cls_name = "/intel/sc_leds_and_button_pcie_device";

int id2function(int id) {
    int f[] = { 0, 5 };
    return f[id];
}

void reportBadAccess(const std::string &message,
                     int id,
                     sc_dt::uint64 offset,
                     unsigned int size) {
    std::ostringstream p;
    p << "BAD ACCESS! (" << message << ") --"
      << " f: " << id2function(id)
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size;
    SC_REPORT_WARNING("/intel/sc_leds_and_button_pcie_device/spec-viol",
                      p.str().c_str());
}

}  // empty namespace

void PciDevice::dma() {
    dma_src_ = registers_.base_address[0] + 0x30;
    physical_address_t dst = dma_src_ + 0x100;
    size_t size = 4;
    while (dma_src_ < registers_.base_address[0] + 0x130) {
        std::vector<uint8_t> data(size, 0);

        // read data from pci-bus
        simics::systemc::utility::reset_payload(&pci_bus_payload_);
        pci_bus_payload_.set_read();
        pci_bus_payload_.set_address(dma_src_);
        pci_bus_payload_.set_data_ptr(&data.front());
        pci_bus_payload_.set_data_length(size);
        pci_upstream_operation_extension_.read(
            0,  // TODO(ah): RID/BDF should be captured from the config access
            simics::types::Sim_Addr_Space_Memory);

        // write data to pci-bus
        simics::systemc::utility::reset_payload(&pci_bus_payload_);
        pci_bus_payload_.set_write();
        pci_bus_payload_.set_address(dst);
        pci_bus_payload_.set_data_ptr(&data.front());
        pci_bus_payload_.set_data_length(size);
        pci_upstream_operation_extension_.write(
            0,  // TODO(ah): RID/BDF should be captured from the config access
            simics::types::Sim_Addr_Space_Memory);

        dma_src_ += size;
        dst += size;
        SC_WAITN(sc_core::sc_time(1, sc_core::SC_NS));
    }
}

using sclif::BaseAddressRegisterQueryInterface;
BaseAddressRegisterQueryInterface::BarInfo PciDevice::getBarInfo() {
    BaseAddressRegisterQueryInterface::BarInfo info;
    // BAR #0: maps the registers for the device itself
    BaseAddressRegisterQueryInterface::BaseAddressRegister bar0 = {};
    bar0.function = 0;
    bar0.offset = 0x10;
    bar0.is_memory = true;
    bar0.is_64bit = false;
    bar0.size_bits = 12;
    bar0.mapping_id = 0;
    info.push_back(bar0);

    // BAR #1: Map the PCI MSI-X table
    BaseAddressRegisterQueryInterface::BaseAddressRegister bar1 = {};
    bar1.function = 0;
    bar1.offset = 0x14;
    bar1.is_memory = true;
    bar1.is_64bit = false;
    bar1.size_bits = 8;
    bar1.mapping_id = 1;
    info.push_back(bar1);

    // BAR #2: Map the PCI MSI-X PBA
    BaseAddressRegisterQueryInterface::BaseAddressRegister bar2 = {};
    bar2.function = 0;
    bar2.offset = 0x18;
    bar2.is_memory = true;
    bar2.is_64bit = false;
    bar2.size_bits = 8;
    bar2.mapping_id = 2;
    info.push_back(bar2);

    return info;
}

BaseAddressRegisterQueryInterface::BarSockets PciDevice::getBarTargetSockets() {
    BaseAddressRegisterQueryInterface::BarSockets sockets;
    BaseAddressRegisterQueryInterface::BarInfo info = getBarInfo();

    for (BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = info.begin(); it != info.end(); ++it) {
        if ((*it).is_memory) {
            sockets.push_back(std::make_pair((*it), &mem_target_socket));
        }
    }
    return sockets;
}

void PciDevice::pci_b_transport(tlm::tlm_generic_payload &trans,
                                sc_core::sc_time &t) {
    // pci_device interface is handled by F0
    int id = 0;

    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();

    // Check assumptions
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess("Operation has non-zero byte enable pointer",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess("Operation has unsupported streaming_width",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    // This b_transport only handles PCI commands, so we should issue an error
    // if this socket was hooked up to the wrong interface
    if (trans.get_command() != tlm::TLM_IGNORE_COMMAND) {
        reportBadAccess("Operation not a TLM_IGNORE_COMMAND",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    }

    // NOTE: in this sample device, we ignore the functionality of the PCI
    // interface and only log the actions.
    dispatcher_.handle(&trans);
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    return;
}

void PciDevice::bus_reset() {
    SC_REPORT_INFO(cls_name,
                   "PCI device operation: bus_reset");
}

void PciDevice::system_error() {
    SC_REPORT_INFO(cls_name,
                   "PCI device operation: system_error");
}

void PciDevice::interrupt_raised(int pin) {
    std::ostringstream os;
    os << "PCI device operation: interrupt_raised pin = " << pin;
    SC_REPORT_INFO(cls_name, os.str().c_str());
}

void PciDevice::interrupt_lowered(int pin) {
    std::ostringstream os;
    os << "PCI device operation: interrupt_lowered pin = " << pin;
    SC_REPORT_INFO(cls_name, os.str().c_str());
}

int PciDevice::send_message(int type, const std::vector<uint8_t> &payload) {
    SC_REPORT_INFO(cls_name,
                   "PCI express operation: send_message");
    switch (type) {
    case 0x44:  // PCIE_HP_Power_Indicator_Off
        SC_REPORT_INFO(cls_name,
                       "power indicator set to OFF");
        blink_onoff_e.cancel();
        system_onoff_led_out.write(false);
        system_onoff_led_count_ += 1;
        break;
    case 0x45:  // PCIE_HP_Power_Indicator_On
        SC_REPORT_INFO(cls_name,
                       "power indicator set to ON");
        blink_onoff_e.cancel();
        system_onoff_led_out.write(true);
        system_onoff_led_count_ += 1;
        break;
    case 0x47:  // PCIE_HP_Power_Indicator_Blink
        SC_REPORT_INFO(cls_name,
                       "power indicator set to BLINK");
        blink_onoff_e.notify();
        break;
    default:
        break;
    }
    return 0;
}

void PciDevice::blinkOnoff() {
    SC_REPORT_INFO(cls_name, "systemc_onoff_led blinks");
    system_onoff_led_out.write(!system_onoff_led_out.read());
    system_onoff_led_count_ += 1;
    blink_onoff_e.notify(5, sc_core::SC_US);
}
