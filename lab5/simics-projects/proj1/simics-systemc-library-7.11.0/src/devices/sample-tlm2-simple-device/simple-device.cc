/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "simple-device.h"

#include <iostream>  // NOLINT(readability/streams)
#include <sstream>
#include <string>

namespace {
const char * TAG = "intel/simple-device";

void timedPrintout(const std::string &object, const std::string &message) {
    std::ostringstream log;
    log << "(" << object << ") @" << sc_core::sc_time_stamp() << ": "
        << message;
    SC_REPORT_INFO(TAG, log.str().c_str());
}

void reportBadAccess(const sc_core::sc_object &object,
                     const std::string &message,
                     sc_dt::uint64 offset,
                     unsigned int size) {
    std::stringstream p;
    p << "BAD ACCESS! (" << message << ") --"
      << " o: 0x" << std::hex << offset
      << " s: 0x" << size;
    timedPrintout(object.name(), p.str());
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
    timedPrintout(object.name(), p.str());
}

void readRegister(uint32_t reg, unsigned char *data) {
    data[0] = reg & 0xff;
    data[1] = (reg >> 8) & 0xff;
    data[2] = (reg >> 16) & 0xff;
    data[3] = reg >> 24;
}

void writeRegister(uint32_t *reg, unsigned char *data) {
    *reg = data[0]
         | data[1] << 8
         | data[2] << 16
         | data[3] << 24;
}
}  // empty namespace

void SimpleDevice::set_register1(uint32_t value) {
    register1_ = value;
}

uint32_t SimpleDevice::register1() const {
    return register1_;
}

void SimpleDevice::set_register2(uint32_t value) {
    register2_ = value;
}

uint32_t SimpleDevice::register2() const {
    return register2_;
}

void SimpleDevice::b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                               sc_core::sc_time &t) {
    // Decode the generic payload (GP) struct
    sc_dt::uint64 offset = trans.get_address();
    unsigned int size = trans.get_data_length();
    unsigned char *data = trans.get_data_ptr();

    // Check assumptions
    if (trans.get_byte_enable_ptr()) {
        reportBadAccess(*this, "Operation has non-zero byte enable pointer",
                        offset, size);
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return;
    }
    if (trans.get_streaming_width() != size) {
        reportBadAccess(*this, "Operation has unsupported streaming_width",
                        offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }
    if (size != 4) {
        reportBadAccess(
            *this, "Operation has invalid size, only 4-byte access supported",
            offset, size);
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return;
    }

    // Read or write registers, full 32-bit word
    t += access_delay_;
    if (trans.is_read()) {
        switch (offset) {
        case 0x00:
            readRegister(register1_, data);
            reportGoodAccess(*this, "Read", offset, size, register1_);
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            return;
        case 0x04:
            readRegister(register2_, data);
            reportGoodAccess(*this, "Read", offset, size, register2_);
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            return;
        default:
            reportBadAccess(*this, "Read from unknown offset", offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
    } else if (trans.is_write()) {
        switch (offset) {
        case 0x00:
            writeRegister(&register1_, data);
            reportGoodAccess(*this, "Write", offset, size, register1_);
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            return;
        case 0x04:
            writeRegister(&register2_, data);
            reportGoodAccess(*this, "Write", offset, size, register2_);
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            return;
        default:
            reportBadAccess(*this, "Write to unknown offset", offset, size);
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
    } else {  // neither a read nor a write
        reportBadAccess(*this, "Operation not read or write", offset, size);
        trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
        return;
    }
}
