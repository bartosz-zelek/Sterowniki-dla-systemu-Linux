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

#include "pci-device.h"

#include <simics/systemc/utility.h>

#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

// Needed because of a bug in sc_report.h that does not qualify the call to
// sc_report_handler for the SC_REPORT_INFO_VERB macro. This has been fixed in
// master on github.
using sc_core::sc_report_handler;
using sc_core::SC_INFO;

namespace {
const int bar0_size_bits = 7;
const int bar2_size_bits = 16;
const int bar5_size_bits = 8;

int id2function(int id) {
    int f[] = { 0, 5 };
    return f[id];
}

void reportBadAccess(const sc_core::sc_object &object,
                     const std::string &message,
                     int id,
                     sc_dt::uint64 offset,
                     unsigned int size) {
    std::ostringstream p;
    p << "BAD ACCESS! (" << message << ") --"
      << " f: " << id2function(id)
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size;
    SC_REPORT_WARNING("/intel/sample-pci-device/spec-viol",
                      p.str().c_str());
}

void reportGoodAccess(const sc_core::sc_object &object,
                      const std::string &message,
                      int id,
                      sc_dt::uint64 offset,
                      unsigned int size,
                      int value) {
    std::ostringstream p;
    p << "Good access (" << message << ") --"
      << " f: " << id2function(id)
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size
      << " v: 0x" << value;
    SC_REPORT_INFO_VERB("/intel/sample-pci-device",
                        p.str().c_str(), sc_core::SC_DEBUG);
}

}  // empty namespace

void PciDevice::set_vendor_id(int f, uint16_t v) {
    registers_[f].vendor_id = v;
}
uint16_t PciDevice::vendor_id(int f) const {
    return registers_[f].vendor_id;
}
void PciDevice::set_device_id(int f, uint16_t v) {
    registers_[f].device_id = v;
}
uint16_t PciDevice::device_id(int f) const {
    return registers_[f].device_id;
}
void PciDevice::set_command(int f, uint16_t v) {
    registers_[f].command = v;
}
uint16_t PciDevice::command(int f) const {
    return registers_[f].command;
}
void PciDevice::set_status(int f, uint16_t v) {
    registers_[f].status = v;
}
uint16_t PciDevice::status(int f) const {
    return registers_[f].status;
}
void PciDevice::set_base_address(int f, int bar, uint32_t v) {
    // IO, Mem and Mem64 bits are ignored; see getBarInfo()
    if (registers_[f].base_address[bar] & 1) {
        // IO, clear only 2 bits
        v &= ~3;
        v |= registers_[f].base_address[bar] & 3;
    } else {
        // Mem, clear 4 bits
        v &= ~7;
        v |= registers_[f].base_address[bar] & 7;
    }
    registers_[f].base_address[bar] = v;
}
uint32_t PciDevice::base_address(int f, int bar) const {
    // IO, Mem and Mem64 bits are setup by getBarInfo()
    return registers_[f].base_address[bar];
}
void PciDevice::set_version(int f, uint32_t v) {
    registers_[f].version = v;
}
uint32_t PciDevice::version(int f) const {
    return registers_[f].version;
}

void PciDevice::dmaTrigger(const std::list<physical_address_t> &l) {
    // extract trigger data from test
    assert(l.size() == 3);
    std::list<physical_address_t> args = l;
    physical_address_t src = args.front(); args.pop_front();
    physical_address_t dst = args.front(); args.pop_front();
    size_t size = args.front(); args.pop_front();
    std::vector<uint8_t> data(size, 0);

    // read data from pci-bus
    simics::systemc::utility::reset_payload(&pci_bus_payload_);
    pci_bus_payload_.set_read();
    pci_bus_payload_.set_address(src);
    pci_bus_payload_.set_data_ptr(&data.front());
    pci_bus_payload_.set_data_length(size);
    pci_upstream_operation_extension_.read(
        0,  // NOTE: RID/BDF should be captured from the config access
        simics::types::Sim_Addr_Space_Memory);

    // write data to pci-bus
    simics::systemc::utility::reset_payload(&pci_bus_payload_);
    pci_bus_payload_.set_write();
    pci_bus_payload_.set_address(dst);
    pci_bus_payload_.set_data_ptr(&data.front());
    pci_bus_payload_.set_data_length(size);
    pci_upstream_operation_extension_.write(
        0,  // NOTE: RID/BDF should be captured from the config access
        simics::types::Sim_Addr_Space_Memory);
}

using simics::systemc::iface::BaseAddressRegisterQueryInterface;
BaseAddressRegisterQueryInterface::BarInfo PciDevice::getBarInfo() {
    BaseAddressRegisterQueryInterface::BarInfo info;

    // NOTE: any changes to the config must *also* be reflected in the
    // implementation of readWriteConfig() below. In particular, changing the
    // size here does not impact the probing of the BARs

    for (int i = 0; i < 2; ++i) {
        registers_[i].base_address[0] &= ~3;
        registers_[i].base_address[0] |= 1;  // IO
        BaseAddressRegisterQueryInterface::BaseAddressRegister bar0 = {};
        bar0.function = i * 5;
        bar0.offset = 0x10;
        bar0.is_memory = false;
        bar0.is_64bit = false;
        bar0.size_bits = bar0_size_bits;
        bar0.mapping_id = 0 + i * 3;
        info.push_back(bar0);

        registers_[i].base_address[2] &= ~7;
        registers_[i].base_address[2] |= 4;  // 64-bits MEM
        BaseAddressRegisterQueryInterface::BaseAddressRegister bar2 = {};
        bar2.function = i * 5;
        bar2.offset = 0x18;
        bar2.is_memory = true;
        bar2.is_64bit = true;
        bar2.size_bits = bar2_size_bits;
        bar2.mapping_id = 1 + i * 3;
        info.push_back(bar2);

        registers_[i].base_address[5] &= ~7;  // 32-bits MEM
        BaseAddressRegisterQueryInterface::BaseAddressRegister bar5 = {};
        bar5.function = i * 5;
        bar5.offset = 0x24;
        bar5.is_memory = true;
        bar5.is_64bit = false;
        bar5.size_bits = bar5_size_bits;
        bar5.mapping_id = 2 + i * 3;
        info.push_back(bar5);
    }

    return info;
}

BaseAddressRegisterQueryInterface::BarSockets PciDevice::getBarTargetSockets() {
    BaseAddressRegisterQueryInterface::BarSockets sockets;
    BaseAddressRegisterQueryInterface::BarInfo info = getBarInfo();

    for (BaseAddressRegisterQueryInterface::BarInfo::const_iterator it
             = info.begin(); it != info.end(); ++it) {
        if ((*it).is_memory) {
            if ((*it).is_64bit) {
                sockets.push_back(std::make_pair((*it), &mem64_target_socket));
            } else {
                sockets.push_back(std::make_pair((*it), &mem_target_socket));
            }
        } else {
            sockets.push_back(std::make_pair((*it), &io_target_socket));
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
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has unsupported streaming_width",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    // This b_transport only handles PCI commands, so we should issue an error
    // if this socket was hooked up to the wrong interface
    if (trans.get_command() != tlm::TLM_IGNORE_COMMAND) {
        reportBadAccess(*this, "Operation not a TLM_IGNORE_COMMAND",
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

void PciDevice::readWriteConfig(int id, tlm::tlm_generic_payload &trans) {  // NOLINT
    // The id identifies which initiator socket the transaction originates
    // from, which in turn can be used to derive to which PCI function the
    // transaction is targeting. This device currently supports two functions,
    // the mandatory F0 (id=0) and also F5 (id=1)
    if (id > 1) {
        SC_REPORT_ERROR("/intel/sample-pci-device",
                        "Too many initiator sockets bound, unknown socket ID");
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    }

    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    // Check assumptions
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has unsupported streaming_width",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }
    // NOTE: we should also check that address is DWORD/4-byte aligned, but
    //       our Viper/X58 platform seems to address single bytes which would
    //       make this implementation a bit more complicated should we support
    //       the full set of configuration registers

    // Read or write registers (assume 4-byte aligned address)
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    if (trans.is_read()) {
        switch (offset) {
        case 0x00:
            if (size == 2) {
                uint16_t *data16 = reinterpret_cast<uint16_t *>(
                        trans.get_data_ptr());
                *data16 = registers_[id].vendor_id;
                reportGoodAccess(*this, "Read device | vendor ID",
                                 id, offset, size, *data16);
            } else {
                *data = (registers_[id].device_id << 16)
                    | registers_[id].vendor_id;
                reportGoodAccess(*this, "Read device | vendor ID",
                                 id, offset, size, *data);
            }
            return;
        case 0x04:
            if (size == 2) {
                uint16_t *data16 = reinterpret_cast<uint16_t *>(
                        trans.get_data_ptr());
                *data16 = registers_[id].command;
                reportGoodAccess(*this, "Read device | command",
                                 id, offset, size, *data16);
            } else {
                *data = (registers_[id].status << 16)
                    | registers_[id].command;
                reportGoodAccess(*this, "Read status | command",
                                 id, offset, size, *data);
            }
            return;
        case 0x10:  // BAR0 is 32-bit IO, BAR1 is not used
            *data = registers_[id].base_address[0];
            reportGoodAccess(*this, "Read BAR0 (IO)", id, offset, size, *data);
            return;
        case 0x18:  // BAR2 is 64-bit Mem (low)
            *data = registers_[id].base_address[2];
            reportGoodAccess(*this, "Read BAR2 (MEM64 lo)", id, offset, size,
                             *data);
            if (size == 4) {
                return;
            } else {
                // fall through to next BAR, with ptr shifted to high bytes
                offset += 4;
                data += 4;
            }
        case 0x1c:  // BAR3 is also 64-bit Mem (high)
            *data = registers_[id].base_address[3];
            reportGoodAccess(*this, "Read BAR2 (MEM64 hi)", id, offset, size,
                             *data);
            return;
        case 0x24:  // BAR5 is 32-bit Mem, BAR4 is not used,
            *data = registers_[id].base_address[5];
            reportGoodAccess(*this, "Read BAR5 (MEM)", id, offset, size, *data);
            return;
        default:
            // according to spec, return 0 for unimplemented/reserved registers
            *data = 0;
            reportGoodAccess(*this, "Read from unimplemented/reserved register",
                             id, offset, size, *data);
            return;
        }
    } else if (trans.is_write()) {
        if (offset == 0x00 || offset == 0x08) {
            reportBadAccess(*this, "Write to read-only register not supported",
                            id, offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }

        int mask = 0;
        switch (offset) {
        case 0x04:  // command & status
            if (size == 2) {
                uint16_t *data16 = reinterpret_cast<uint16_t *>(
                        trans.get_data_ptr());
                registers_[id].command = *data16;
                reportGoodAccess(*this, "Write command (status ignored)",
                                 id, offset, size, *data16);
            } else {
                registers_[id].command = (*data) & 0xffff;  // ignore status
                reportGoodAccess(*this, "Write command (status ignored)",
                                 id, offset, size, *data);
            }
            return;
        case 0x10:  // BAR0, 32-bit IO, 7 bits size
            mask = ~((1 << bar0_size_bits) - 1);
            registers_[id].base_address[0] &= ~mask;
            registers_[id].base_address[0] |= *data & mask;
            reportGoodAccess(*this, "Write BAR0 (IO)", id, offset, size, *data);
            return;
        case 0x18:  // BAR2, 64-bit Mem (low), 16 bits size
            mask = ~((1 << bar2_size_bits) - 1);
            registers_[id].base_address[2] &= ~mask;
            registers_[id].base_address[2] |= *data & mask;
            reportGoodAccess(*this, "Write BAR2 (MEM64 lo)",
                             id, offset, size, *data);
            if (size == 4) {
                return;
            } else {
                // fall through to next BAR, with ptr shifted to high bytes
                offset += 4;
                data += 1;
            }
        case 0x1c:  // BAR3, 64-bit Mem (high)
            registers_[id].base_address[3] = *data;
            reportGoodAccess(*this, "Write BAR2 (MEM64 hi)",
                             id, offset, size, *data);
            return;
        case 0x24:  // BAR5, 32-bit Mem, 8 bits size
            mask = ~((1 << bar5_size_bits) - 1);
            registers_[id].base_address[5] &= ~mask;
            registers_[id].base_address[5] |= *data & mask;
            reportGoodAccess(*this, "Write BAR5 (MEM)",
                             id, offset, size, *data);
            return;
        default:
            // note: our sample device simply ignores access to unimplemented
            // registers, but this is probably a bad idea for a compliant
            // device
            reportGoodAccess(*this, "Write to unimplemented register discarded",
                             id, offset, size, *data);
            return;
        }
    } else {
        reportBadAccess(*this, "Operation neither a read nor a write",
                        id, offset, size);
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}

/*
 * For configuration space access, the IoMemory gasket/interface is used with
 * the normal TLM_READ_COMMAND/TLM_WRITE_COMMAND to indicate read/write
 * operation.
 */
void PciDevice::config_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                   sc_core::sc_time &t) {
    readWriteConfig(id, trans);
}

unsigned int PciDevice::config_transport_dbg(int id,
                                             tlm::tlm_generic_payload &trans) {  // NOLINT
    readWriteConfig(id, trans);
    return trans.get_data_length();
}

// Helper to check 32-bit transactions to IO or MEM
bool PciDevice::checkTransaction(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                 sc_dt::uint64 offset, unsigned int size) {
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return false;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has unsupported streaming_width",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return false;
    }
    if (offset % 4) {
        reportBadAccess(
            *this, "Operation has invalid address, must be 4-byte aligned",
            id, offset, size);
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return false;
    }
    if (size != 4) {
        reportBadAccess(
            *this, "Operation has invalid size, only 4-byte access supported",
            id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return false;
    }

    return true;
}

// IO does not have anything mapped (this is just a sample device)
void PciDevice::io_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                sc_core::sc_time &t) {
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    if (!checkTransaction(id, trans, offset, size)) {
        return;
    }

    // Read or write registers, full 32-bit word
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    if (trans.is_read()) {
        // accept any access to any region
        *data = 0;
        reportGoodAccess(*this, "Read", id, offset, size, *data);
        return;
    } else if (trans.is_write()) {
        // accept any access to any region
        reportGoodAccess(*this, "Write (discarded)",
                         id, offset, size, *data);
        return;
    } else {
        reportBadAccess(*this, "Operation neither a read nor a write",
                        id, offset, size);
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}


/*
 * For normal MMIO access, the IoMemory gasket/interface is used with the
 * normal TLM_READ_COMMAND/TLM_WRITE_COMMAND to indicate read/write operation.
 */
void PciDevice::mem_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                sc_core::sc_time &t) {
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    if (!checkTransaction(id, trans, offset, size)) {
        return;
    }

    // Read or write registers, full 32-bit word
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    if (trans.is_read()) {
        switch (offset) {
        case 0x0:
            *data = registers_[id].version;
            reportGoodAccess(*this, "Read version", id, offset, size, *data);
            return;
        default:
            // accept any access to any region
            *data = 0;
            reportGoodAccess(*this, "Read", id, offset, size, *data);
            return;
        }
    } else if (trans.is_write()) {
        switch (offset) {
        case 0x00:
            registers_[id].version = *data;
            reportGoodAccess(*this, "Write version", id, offset, size, *data);
            return;
        default:
            // accept any access to any region
            reportGoodAccess(*this, "Write (discarded)",
                             id, offset, size, *data);
            return;
        }
    } else {
        reportBadAccess(*this, "Operation neither a read nor a write",
                        id, offset, size);
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}

int PciDevice::send_message(int type, const std::vector<uint8_t> &payload) {
    SC_REPORT_INFO("/intel/sample-pci-device",
                   "PCI device operation: send_message");
    return 0;  // 0 means OK and I don't think it's used for anything by anyone
}

void PciDevice::bus_reset() {
    SC_REPORT_INFO("/intel/sample-pci-device",
                   "PCI device operation: bus_reset");
}

void PciDevice::system_error() {
    SC_REPORT_INFO("/intel/sample-pci-device",
                   "PCI device operation: system_error");
}

void PciDevice::interrupt_raised(int pin) {
    std::ostringstream os;
    os << "PCI device operation: interrupt_raised pin = " << pin;
    SC_REPORT_INFO("/intel/sample-pci-device", os.str().c_str());
}

void PciDevice::interrupt_lowered(int pin) {
    std::ostringstream os;
    os << "PCI device operation: interrupt_lowered pin = " << pin;
    SC_REPORT_INFO("/intel/sample-pci-device", os.str().c_str());
}
