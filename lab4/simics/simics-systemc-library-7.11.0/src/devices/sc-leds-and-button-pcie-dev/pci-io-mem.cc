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

#include "pci-io-mem.h"

#include <string>
#include <list>
#include <sstream>

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

void reportGoodAccess(const std::string &message,
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
    SC_REPORT_INFO_VERB(cls_name, p.str().c_str(), sc_core::SC_DEBUG);
}

}  // empty namespace

void PciIoMem::readBytes(sc_dt::uint64 address, unsigned int len,
                         unsigned char* data, bool reg) const {
    std::ostringstream p;
    p << "Good read access --"
      << " o: 0x" << std::hex << address
      << " s: 0x" << len;

    const uint8_t* bytes = NULL;
    if (reg)
        bytes = registers_.byte;
    else
        bytes = mem_bytes;

    for (unsigned int offset = 0; offset < len; ++offset)
        *(data + offset) = bytes[address + offset];

    SC_REPORT_INFO_VERB(cls_name, p.str().c_str(), sc_core::SC_DEBUG);
}

void PciIoMem::writeBytes(sc_dt::uint64 address, unsigned int len,
                          unsigned char* data, bool reg) {
    std::ostringstream p;
    p << "Good write access --"
      << " o: 0x" << std::hex << address
      << " s: 0x" << len;

    uint8_t* bytes = NULL;
    if (reg)
        bytes = registers_.byte;
    else
        bytes = mem_bytes;

    for (unsigned int offset = 0; offset < len; ++offset)
        bytes[address + offset] = *(data + offset);

    SC_REPORT_INFO_VERB(cls_name, p.str().c_str(), sc_core::SC_DEBUG);
}

void PciIoMem::set_vendor_id(int id, uint16_t v) {
    unsigned char data[2];
    data[1] = v >> 8;
    data[0] = v & 0xff;
    writeBytes(0, 2, data, true);
}
uint16_t PciIoMem::vendor_id(int id) const {
    unsigned char data[2];
    readBytes(0, 2, data, true);
    return data[1] << 8 | data[0];
}
void PciIoMem::set_device_id(int id, uint16_t v) {
    unsigned char data[2];
    data[1] = v >> 8;
    data[0] = v & 0xff;
    writeBytes(2, 2, data, true);
}
uint16_t PciIoMem::device_id(int id) const {
    unsigned char data[2];
    readBytes(2, 2, data, true);
    return data[1] << 8 | data[0];
}
void PciIoMem::set_command(int id, uint16_t v) {
    unsigned char data[2];
    data[1] = v >> 8;
    data[0] = v & 0xff;
    if (sc_core::sc_get_status() != sc_core::SC_ELABORATION)
        system_onoff_led_out.write(v & 0x2);
    writeBytes(4, 2, data, true);
}
uint16_t PciIoMem::command(int id) const {
    unsigned char data[2];
    readBytes(4, 2, data, true);
    return data[1] << 8 | data[0];
}
void PciIoMem::set_status(int id, uint16_t v) {
    unsigned char data[2];
    data[1] = v >> 8;
    data[0] = v & 0xff;
    writeBytes(6, 2, data, true);
}
uint16_t PciIoMem::status(int id) const {
    unsigned char data[2];
    readBytes(6, 2, data, true);
    return data[1] << 8 | data[0];
}
void PciIoMem::set_base_address(int id, int bar, uint32_t v) {
    registers_.base_address[bar] = v;
}
uint32_t PciIoMem::base_address(int id, int bar) const {
    return registers_.base_address[bar];
}
void PciIoMem::set_version(int id, uint32_t v) {
    version_ = v;
}
uint32_t PciIoMem::version(int id) const {
    return version_;
}
void PciIoMem::set_system_onoff_led(bool val) {
    system_onoff_led_out.write(val);
}
bool PciIoMem::system_onoff_led() const {
    return system_onoff_led_out.read();
}
void PciIoMem::set_system_onoff_led_count(uint32_t val) {
    system_onoff_led_count_ = val;
}
uint32_t PciIoMem::system_onoff_led_count() const {
    return system_onoff_led_count_;
}
void PciIoMem::set_pin_out_vals(std::list<bool> state) {
    for (int i = 0; i < OUTPUT_PIN_COUNT; ++i) {
        pin_out[i]->write(state.front());
        state.pop_front();
    }
}
std::list<bool> PciIoMem::pin_out_vals() const {
    std::list<bool> l;
    for (int i = 0; i < OUTPUT_PIN_COUNT; ++i) {
        l.push_back(pin_out[i]->read());
    }
    return l;
}

void PciIoMem::initializeRegisters() {
    registers_.byte[0] = 0x86;
    registers_.byte[1] = 0x80;  // Intel
    registers_.byte[2] = 0x61;
    registers_.byte[3] = 0x0d;  // Simics simulated training device
    registers_.byte[6] = 0x10;
    registers_.byte[8] = 0x01;
    registers_.byte[10] = 0x80;
    registers_.byte[11] = 0x09;
    registers_.byte[0x34] = 0x70;  // capabilities_ptr
    registers_.byte[0x70] = 0x11;
    registers_.byte[0x71] = 0x80;
    registers_.byte[0x72] = 0x1;
    registers_.byte[0x74] = 0x1;
    registers_.byte[0x80] = 0x10;
    registers_.byte[0x82] = 0x2;
    registers_.byte[0x84] = 0x40;
    registers_.byte[0x85] = 0x82;
    registers_.byte[0x92] = 0x41;
    for (int j = 0; j < 6; ++j) {
        registers_.base_address[j] = 0;
    }
}

// Helper to check all transactions
bool PciIoMem::checkTransaction(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                sc_dt::uint64 offset, unsigned int size,
                                bool config) {
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess("Operation has non-zero byte enable pointer",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return false;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess("Operation has unsupported streaming_width",
                        id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return false;
    }
    if (config)
        return true;
    if (offset % 4) {
        reportBadAccess(
            "Operation has invalid address, must be 4-byte aligned",
            id, offset, size);
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return false;
    }
    if (size != 4) {
        reportBadAccess(
            "Operation has invalid size, only 4-byte access supported",
            id, offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return false;
    }

    return true;
}

void PciIoMem::readWriteConfig(int id, tlm::tlm_generic_payload &trans) {  // NOLINT
    // The id identifies which initiator socket the transaction originates
    // from, which in turn can be used to derive to which PCI function the
    // transaction is targeting. This device currently only supports one
    // function, the mandatory F0 (id=0)
    if (id != 0) {
        SC_REPORT_ERROR(cls_name,
                        "Too many initiator sockets bound, unknown socket ID");
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    }

    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    if (!checkTransaction(id, trans, offset, size, true)) {
        return;
    }

    // Read or write registers
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    if (trans.is_read()) {
        int idx = 0;
        switch (offset) {
        case 0x10:  // BAR0 is 32-bit Mem, 12 bits size
            // fall through
        case 0x14:  // BAR1 is 32-bit Mem
            // fall through
        case 0x18:  // BAR2 is 32-bit Mem
            // fall through
        case 0x1c:  // BAR3-5 is not used
            // fall through
        case 0x20:
            // fall through
        case 0x24:
            idx = (offset - 0x10) / 4;
            *data = registers_.base_address[idx];
            reportGoodAccess("Read BAR (MEM)",
                             id, offset, size, *data);
            return;
        case 0x30:  // Resevered, expansion_rom_base
            *data = 0;
            reportGoodAccess("Read EXPANSION_ROM_BASE",
                             id, offset, size, *data);
            return;
        default:
            readBytes(offset, size, trans.get_data_ptr(), true);
            return;
        }
    } else if (trans.is_write()) {
        if (offset == 0x00 || offset == 0x08) {
            reportBadAccess("Write to read-only register not supported",
                            id, offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
        int mask = 0;
        int idx = 0;
        switch (offset) {
        case 0x4:  // command
            system_onoff_led_out.write(*data & 0x2);
            writeBytes(offset, size, trans.get_data_ptr(), true);
            return;
        case 0x10:  // BAR0, 32-bit Mem, 12 bits size
            mask = ~((1 << 12) - 1);
            registers_.base_address[0] &= ~mask;
            registers_.base_address[0] |= *data & mask;
            reportGoodAccess("Write BAR0 (MEM)", id,
                             offset, size, *data);
            return;
        case 0x14:  // BAR1, 32-bit Mem
            // fall through
        case 0x18:  // BAR2, 32-bit Mem
            idx = (offset - 0x10) / 4;
            mask = ~((1 << 8) - 1);
            registers_.base_address[idx] &= ~mask;
            registers_.base_address[idx] |= *data & mask;
            // fall through
        case 0x1c:
            // fall through
        case 0x20:
            // fall through
        case 0x24:
            reportGoodAccess("Write BAR", id, offset, size, *data);
            return;
        case 0x30:
            reportGoodAccess("Write EXPANSION_ROM_BASE",
                             id, offset, size, *data);
            return;
        default:
            writeBytes(offset, size, trans.get_data_ptr(), true);
            return;
        }
    } else {
        reportBadAccess("Operation neither a read nor a write",
                        id, offset, size);
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}

void PciIoMem::readWriteMem(int id, tlm::tlm_generic_payload &trans,
                            bool dbg) {
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    if (!checkTransaction(id, trans, offset, size, dbg)) {
        return;
    }

    // Read or write registers, full 32-bit word
    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    if (id == 0) {
        if (trans.is_read()) {
            switch (offset) {
            case 0x0:  // pin_out[0]
                // fall through
            case 0x4:  // pin_out[1]
                // fall through
            case 0x8:  // pin_out[2]
                // fall through
            case 0xc:  // pin_out[3]
                *data = (pin_out[offset / 4]->read()) ? 1 : 0;
                break;
            case 0x20:  // system_onoff_led
                *data = (system_onoff_led_out.read()) ? 1 : 0;
                break;
            case 0x24:  // system_onoff_led_count
                *data = system_onoff_led_count_;
                break;
            default:  // accept any access to any other region
                readBytes(offset, size, trans.get_data_ptr(), false);
                break;
            }
            reportGoodAccess("Read", id, offset, size, *data);
            return;
        } else if (trans.is_write()) {
            switch (offset) {
            case 0x0:  // pin_out[0]
                // fall through
            case 0x4:  // pin_out[1]
                // fall through
            case 0x8:  // pin_out[2]
                // fall through
            case 0xc:  // pin_out[3]
                pin_out[offset / 4]->write(*data);
                reportGoodAccess("Write MEM pinout",
                                 id, offset, size, *data);
                break;
            case 0x20:  // system_onoff_led
                system_onoff_led_out.write(*data);
                reportGoodAccess("Write MEM ONOFF",
                                 id, offset, size, *data);
                break;
            case 0x24:  // system_onoff_led_count
                system_onoff_led_count_ = *data;
                reportGoodAccess("Write MEM ONOFF COUNT",
                                 id, offset, size, *data);
                break;
            default:
                writeBytes(offset, size, trans.get_data_ptr(), false);
                break;
            }
            return;
        } else {
            reportBadAccess("Operation neither a read nor a write",
                            id, offset, size);
        }
    } else if (id == 1) {
        if (trans.is_read()) {
            switch (offset) {
            case 0x0:
                *data = version_;
                reportGoodAccess("Read version", id,
                                 offset, size, *data);
                return;
            default:
                // ignore all other access
                *data = 0;
                return;
            }
        } else if (trans.is_write()) {
            switch (offset) {
            case 0x00:
                version_ = *data;
                reportGoodAccess("Write version", id,
                                 offset, size, *data);
                return;
            default:
                // ignore all other access
                reportGoodAccess("Write MEM (discarded)",
                                 id, offset, size, *data);
                return;
            }
        } else {
            reportBadAccess("Operation neither a read nor a write",
                            id, offset, size);
        }
    }

    trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
}

/*
 * For configuration space access, the IoMemory gasket/interface is used with
 * the normal TLM_READ_COMMAND/TLM_WRITE_COMMAND to indicate read/write
 * operation.
 */
void PciIoMem::config_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                                  sc_core::sc_time &t) {
    readWriteConfig(id, trans);
}

unsigned int PciIoMem::config_transport_dbg(int id,
                                            tlm::tlm_generic_payload &trans) {  // NOLINT
    readWriteConfig(id, trans);
    return trans.get_data_length();
}

// IO controls the outbound LED pins, it contains pin control registers
void PciIoMem::io_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                               sc_core::sc_time &t) {
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned size = trans.get_data_length();
    uint32_t *data = reinterpret_cast<uint32_t *>(trans.get_data_ptr());

    if (!checkTransaction(id, trans, offset, size, false)) {
        return;
    }

    trans.set_response_status(tlm::TLM_OK_RESPONSE);
    // accept any access to any region
    reportGoodAccess("Write/Read IO (discarded)",
                     id, offset, size, *data);
}

/*
 * For normal MMIO access, the IoMemory gasket/interface is used with the
 * normal TLM_READ_COMMAND/TLM_WRITE_COMMAND to indicate read/write operation.
 */
void PciIoMem::mem_b_transport(int id, tlm::tlm_generic_payload &trans,  // NOLINT
                               sc_core::sc_time &t) {
    readWriteMem(id, trans, false);
}

unsigned int PciIoMem::mem_transport_dbg(int id,
                                         tlm::tlm_generic_payload &trans) {  // NOLINT
    readWriteMem(id, trans, true);
    return trans.get_data_length();
}
