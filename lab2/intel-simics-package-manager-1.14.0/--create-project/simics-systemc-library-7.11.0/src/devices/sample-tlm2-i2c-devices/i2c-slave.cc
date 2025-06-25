/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "i2c-slave.h"

#include <sstream>
#include <string>
#include <simics/types/i2c_ack.h>

//------------------------------------------------------------
// Generic SystemC output function prefixed with time
//------------------------------------------------------------
namespace {
const char* TAG = "/intel/sample-tlm2-i2c-devices";

void reportBadAccess(const sc_core::sc_object &object,
                     const std::string &message,
                     sc_dt::uint64 offset,
                     unsigned int size) {
    std::stringstream p;
    p << "BAD ACCESS! (" << message << ") --"
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size;
    SC_REPORT_WARNING(TAG, p.str().c_str());
}

void reportGoodAccess(const sc_core::sc_object &object,
                      const std::string &message,
                      sc_dt::uint64 offset,
                      unsigned int size,
                      int value) {
    std::stringstream p;
    p << "Good access (" << message << ") --"
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size
      << " v: 0x" << value;
    SC_REPORT_INFO_VERB(TAG, p.str().c_str(), sc_core::SC_DEBUG);
}

}  // empty namespace

void I2cSlave::set_register(int v) {
    register_ = v;
}

int I2cSlave::get_register() const {
    return register_;
}

void I2cSlave::set_address(int v) {
    i2c_address_ = v & 0x7f;
}

int I2cSlave::get_address() const {
    return i2c_address_;
}

void I2cSlave::io_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                              sc_core::sc_time &t) {
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has bad streaming_width",
                        offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }
    if (size != 4) {
        reportBadAccess(*this, "Operation not four bytes in size",
                        offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    int32_t *data = reinterpret_cast<int32_t *>(trans.get_data_ptr());
    if (trans.is_read()) {
        // Read data from a register, full word.
        switch (offset) {
        case 0x00:
            *data = register_;
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            reportGoodAccess(*this, "Read", offset, size, *data);
            break;
        default:
            reportBadAccess(*this, "Read from unknown offset", offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            break;
        }
    } else if (trans.is_write()) {
        // Write data to a register, full word.
        switch (offset) {
        case 0x00:
            register_ = *data;
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            reportGoodAccess(*this, "Write", offset, size, *data);
            break;
        default:
            reportBadAccess(*this, "Write to unknown offset", offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            break;
        }
    } else {
        reportBadAccess(*this, "Operation not read or write", offset, size);
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
    }
}

void I2cSlave::i2c_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                               sc_core::sc_time &t) {
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();

    if (cmd != tlm::TLM_IGNORE_COMMAND) {
        reportBadAccess(*this, "Operation not a TLM_IGNORE_COMMAND",
                        offset, size);
        trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
        return;
    }
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has bad streaming_width",
                        offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }
    if (size != 0) {
        reportBadAccess(*this, "Operation has invalid byte size",
                        offset, size);
        trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }

    trans.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    dispatcher_.handle(&trans);
    return;
}

void I2cSlave::start(uint8_t address) {
    SC_REPORT_INFO(::TAG, "I2C device operation: start");
    simics::types::i2c_ack_t ack = simics::types::I2C_noack;
    if (address / 2 == i2c_address_)
        ack = simics::types::I2C_ack;
    extension_.acknowledge(ack);
}

void I2cSlave::read() {
    SC_REPORT_INFO(::TAG, "I2C device operation: read");
    extension_.read_response(register_ & 0xff);
}

void I2cSlave::write(uint8_t value) {
    SC_REPORT_INFO(::TAG, "I2C device operation: write");
    register_ = value;
    extension_.acknowledge(simics::types::I2C_ack);
}

void I2cSlave::stop() {
    SC_REPORT_INFO(::TAG, "I2C device operation: stop");
    // no ACK required for stop, just log
}

std::vector<uint8_t> I2cSlave::addresses() {
    SC_REPORT_INFO(::TAG, "I2C device operation: addresses");
    std::vector<uint8_t> addresses;
    addresses.push_back(i2c_address_ * 2);
    addresses.push_back(i2c_address_ * 2 + 1);
    return addresses;
}
