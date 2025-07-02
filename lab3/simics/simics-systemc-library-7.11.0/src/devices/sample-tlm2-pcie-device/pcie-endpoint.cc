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

#include "pcie-endpoint.h"

#include <simics/systemc/utility.h>  // reset_payload
#include <simics/systemc/pcie_tlm_extension.h>  // PcieTlmExtension

#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

namespace {
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
    SC_REPORT_WARNING("/intel/sample-tlm2-pcie-device/spec-viol",
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
    SC_REPORT_INFO_VERB("/intel/sample-tlm2-pcie-device",
                        p.str().c_str(), sc_core::SC_DEBUG);
}

}  // empty namespace

void PcieEndpoint::set_vendor_id(int f, uint16_t v) {
    registers_[f].vendor_id = v;
}
uint16_t PcieEndpoint::vendor_id(int f) const {
    return registers_[f].vendor_id;
}
void PcieEndpoint::set_device_id(int f, uint16_t v) {
    registers_[f].device_id = v;
}
uint16_t PcieEndpoint::device_id(int f) const {
    return registers_[f].device_id;
}
void PcieEndpoint::set_command(int f, uint16_t v) {
    registers_[f].command = v;
}
uint16_t PcieEndpoint::command(int f) const {
    return registers_[f].command;
}
void PcieEndpoint::set_status(int f, uint16_t v) {
    registers_[f].status = v | 0x10;
}
uint16_t PcieEndpoint::status(int f) const {
    return registers_[f].status;
}
void PcieEndpoint::set_base_address(int f, int bar, uint32_t v) {
    if (bar > 6) {
        SC_REPORT_ERROR("/intel/sample-tlm2-pcie-device",
                        ("Invalid BAR id: " + std::to_string(bar)).c_str());
        return;
    }
    if (bar == 6) {  // Expansion ROM BAR
        // only set the address part (upper 16 bits and the enable bit)
        v &= ~0xfffe;
        registers_[f].expansion_rom_base_address = v;
        return;
    }
    if (bar != 1) {  // BAR1 is the higher 32 bits of 64 bits memory BAR
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
    }
    registers_[f].base_address[bar] = v;
}
uint32_t PcieEndpoint::base_address(int f, int bar) const {
    if (bar == 6) {
        return registers_[f].expansion_rom_base_address;
    }
    // IO, Mem and Mem64 bits are setup by getBarInfo()
    return registers_[f].base_address[bar];
}
void PcieEndpoint::set_version(int f, uint32_t v) {
    registers_[f].version = v;
}
uint32_t PcieEndpoint::version(int f) const {
    return registers_[f].version;
}

void PcieEndpoint::dmaTrigger(const std::list<physical_address_t> &l) {
    // extract trigger data from test
    assert(l.size() == 3);
    std::list<physical_address_t> args = l;
    physical_address_t src = args.front();
    args.pop_front();
    physical_address_t dst = args.front();
    args.pop_front();
    size_t size = args.front();
    args.pop_front();
    std::vector<uint8_t> data(size, 0);
    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    simics::systemc::PcieTlmExtension ext;
    ext.type = simics::types::PCIE_Type_Mem;

    // read data from pcie map target
    simics::systemc::utility::reset_payload(&pcie_map_payload_);
    pcie_map_payload_.set_read();
    pcie_map_payload_.set_address(src);
    pcie_map_payload_.set_data_ptr(&data.front());
    pcie_map_payload_.set_data_length(size);
    pcie_map_payload_.set_extension(&ext);
    pcie_map_socket->b_transport(pcie_map_payload_, t);

    // write data to pcie map target
    simics::systemc::utility::reset_payload(&pcie_map_payload_);
    pcie_map_payload_.set_write();
    pcie_map_payload_.set_address(dst);
    pcie_map_payload_.set_data_ptr(&data.front());
    pcie_map_payload_.set_data_length(size);
    pcie_map_payload_.set_extension(&ext);
    pcie_map_socket->b_transport(pcie_map_payload_, t);
}

void PcieEndpoint::readWriteConfig(int id, tlm::tlm_generic_payload &trans) {  // NOLINT
    // The id identifies which initiator socket the transaction originates
    // from, which in turn can be used to derive to which PCI function the
    // transaction is targeting. This device currently supports two functions,
    // the mandatory F0 (id=0) and also F5 (id=1)
    if (id > 1) {
        SC_REPORT_ERROR("/intel/sample-tlm2-pcie-device",
                        "Too many initiator sockets bound, unknown socket ID");
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    }

    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    // Check assumptions
    if (!checkTransaction(id, trans, offset, size)) {
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
        case 0x06:
            if (size == 2) {
                uint16_t *data16 = reinterpret_cast<uint16_t *>(
                        trans.get_data_ptr());
                *data16 = registers_[id].status;
                reportGoodAccess(*this, "Read device | status",
                                 id, offset, size, *data16);
            }
            return;
        case 0xe:
            if (size == 1) {
                uint8_t *data8 = reinterpret_cast<uint8_t *>(
                        trans.get_data_ptr());
                *data8 = 0x80;  // MF Endpoint header type
                reportGoodAccess(*this, "Read device | header type",
                                 id, offset, size, *data8);
            }
            return;
        case 0x10:  // BAR0 is 64-bit Mem (low)
            *data = registers_[id].base_address[0];
            reportGoodAccess(*this, "Read BAR0 (MEM64 lo)", id, offset, size,
                             *data);
            if (size == 4) {
                return;
            } else {
                // fall through to next BAR, with ptr shifted to high bytes
                offset += 4;
                data += 4;
            }
        case 0x14:  // BAR1 is also 64-bit Mem (high)
            *data = registers_[id].base_address[1];
            reportGoodAccess(*this, "Read BAR1 (MEM64 hi)", id, offset, size,
                             *data);
            return;
        case 0x30:  // Expansion ROM BAR
            *data = registers_[id].expansion_rom_base_address;
            reportGoodAccess(*this, "Read expansion ROM BAR", id, offset,
                             size, *data);
            return;
        case 0x34:  // Capabilities Pointer
            *data = registers_[id].capabilities_pointer;
            reportGoodAccess(*this, "Read capabilities pointer", id, offset,
                             size, *data);
            return;
        case 0x40:
            if (size == 2) {
                // PCIe Capability Structure
                *data = 0x10;
                reportGoodAccess(*this,
                                 "Read PCI Express Capability List Register",
                                 id, offset, size, *data);
            }
            return;
        case 0x42:
            if (size == 2) {
                // PCI Express Endpoint
                *data = 0x2;
                reportGoodAccess(*this,
                                 "Read PCI Express Capabilities Register",
                                 id, offset, size, *data);
            }
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
        case 0x10:  // BAR0, 64-bit Mem (low), 16 bits size
            mask = ~((1 << 16) - 1);
            registers_[id].base_address[0] &= ~mask;
            registers_[id].base_address[0] |= *data & mask;
            reportGoodAccess(*this, "Write BAR0 (MEM64 lo)",
                             id, offset, size, *data);
            if (size == 4) {
                return;
            } else {
                // fall through to next BAR, with ptr shifted to high bytes
                offset += 4;
                data += 1;
            }
        case 0x14:  // BAR1, 64-bit Mem (high)
            registers_[id].base_address[1] = *data;
            reportGoodAccess(*this, "Write BAR1 (MEM64 hi)",
                             id, offset, size, *data);
            return;
        case 0x30:  // Expansion ROM BAR
            mask = ~0xfffe;
            registers_[id].expansion_rom_base_address &= ~mask;
            registers_[id].expansion_rom_base_address |= *data & mask;
            reportGoodAccess(*this, "Write expansion ROM BAR",
                             id, offset, size, *data);
            return;
        case 0x34:
            // Capabilities Pointer is RO
            reportGoodAccess(*this,
                             "Write to Read-Only Capabilities Pointer",
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
void PcieEndpoint::
config_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                   sc_core::sc_time &t) {  // NOLINT
    readWriteConfig(id, trans);
}

unsigned int PcieEndpoint::
config_transport_dbg(int id, tlm::tlm_generic_payload &trans) {  // NOLINT
    readWriteConfig(id, trans);
    return trans.get_data_length();
}

// Helper to check 32-bit transactions to IO or MEM
bool PcieEndpoint::checkTransaction(int id,
                                    tlm::tlm_generic_payload &trans,  // NOLINT
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
    return true;
}

void PcieEndpoint::initRegisters() {
    // F0 & F5 configuration of basic registers
    // NOTE: part of the model, not the adapter. These values are however
    // checkpointed by the attributes of the adapter
    for (int i = 0; i < 2; ++i) {
        registers_[i].vendor_id = 0x104c;  // same for all functions
        registers_[i].device_id = 0xac10;
        // capabilities list bit must be hardwired to 1b
        registers_[i].status = 0x10;
        registers_[i].command = 0;
        for (int j = 0; j < 6; ++j) {
            // io, mem, mem64 bits are properties of the model too,
            // => set on read, ignored on write
            registers_[i].base_address[j] = 0;
        }
        // BAR0 is hardwired as Prefetchable 64 bits MEM
        registers_[i].base_address[0] &= ~7;
        registers_[i].base_address[0] |= 0xc;

        registers_[i].expansion_rom_base_address = 0;
        registers_[i].capabilities_pointer = 0x40;
        registers_[i].version = 0x4711 + i;
    }
}

std::vector<sclif::PcieBaseAddressRegisterQueryInterface::PCIeBar>
PcieEndpoint::getBarInfo() {
    std::vector<sclif::PcieBaseAddressRegisterQueryInterface::PCIeBar> info;

    // NOTE: any changes to the config must *also* be reflected in the
    // implementation of readWriteConfig() below. In particular, changing the
    // size here does not impact the probing of the BARs

    for (int i = 0; i < 2; ++i) {
        PcieBaseAddressRegisterQueryInterface::PCIeBar bar0 = {};
        bar0.function = i * 5;
        bar0.offset = 0x10;
        bar0.is_memory = true;
        bar0.is_64bit = true;
        bar0.size_bits = 16;  // 64KB
        bar0.target_socket = &bar0_target_socket;
        info.push_back(bar0);

        PcieBaseAddressRegisterQueryInterface::PCIeBar bar6 = {};
        bar6.function = i * 5;
        bar6.offset = 0x30;
        bar6.is_memory = true;
        bar6.is_64bit = false;
        bar6.size_bits = 16;  // 64KB
        bar6.target_socket = &expansion_rom_bar_target_socket;
        info.push_back(bar6);
    }

    return info;
}

void PcieEndpoint::hotReset() {
    SC_REPORT_INFO_VERB("/intel/sample-tlm2-pcie-device",
                        "Hot reset", sc_core::SC_DEBUG);
    initRegisters();
}

void PcieEndpoint::warmReset() {
    SC_REPORT_INFO_VERB("/intel/sample-tlm2-pcie-device",
                        "Warm reset", sc_core::SC_DEBUG);
    initRegisters();
}

/*
 * For normal MMIO access, the IoMemory gasket/interface is used with the
 * normal TLM_READ_COMMAND/TLM_WRITE_COMMAND to indicate read/write operation.
 */
void PcieEndpoint::mem_b_transport(int bar_id, int id,
                                   tlm::tlm_generic_payload &trans,  // NOLINT
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
            reportGoodAccess(*this,
                             "Read MEM version for BAR" + \
                             std::to_string(bar_id),
                             id, offset, size, *data);
            return;
        default:
            // accept any access to any region
            *data = 0;
            reportGoodAccess(*this, "Read MEM for BAR" + std::to_string(bar_id),
                             id, offset, size, *data);
            return;
        }
    } else if (trans.is_write()) {
        switch (offset) {
        case 0x00:
            registers_[id].version = *data;
            reportGoodAccess(
                *this,
                "Write MEM version for BAR" + std::to_string(bar_id),
                id, offset, size, *data);
            return;
        default:
            // accept any access to any region
            reportGoodAccess(
                *this,
                "Write MEM (discarded) for BAR" + std::to_string(bar_id),
                id, offset, size, *data);
            return;
        }
    } else {
        reportBadAccess(*this, "Operation neither a read nor a write",
                        id, offset, size);
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}

unsigned int PcieEndpoint::
mem_transport_dbg(int bar_id, int id,
                  tlm::tlm_generic_payload &trans) {  // NOLINT: SystemC API
    sc_core::sc_time t = sc_core::SC_ZERO_TIME;
    // mem_b_transport does not call wait()
    mem_b_transport(bar_id, id, trans, t);
    if (trans.get_response_status() == tlm::TLM_OK_RESPONSE) {
        return trans.get_data_length();
    } else {
        return 0;
    }
}

void PcieEndpoint::msg_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                                   sc_core::sc_time &t) {  // NOLINT
    SC_REPORT_INFO_VERB("/intel/sample-tlm2-pcie-device",
                        "Unimplemented: PCIe message transaction",
                        sc_core::SC_DEBUG);

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
}
