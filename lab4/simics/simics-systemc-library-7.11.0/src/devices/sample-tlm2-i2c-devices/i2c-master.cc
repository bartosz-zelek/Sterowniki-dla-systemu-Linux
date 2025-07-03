/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "i2c-master.h"
#include <sstream>

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

}  // empty namespace

void I2cMaster::set_address(uint8_t v) {
    address_ = v;
    extension_.start(address_);
}

uint8_t I2cMaster::get_address() const {
    return address_;
}

void I2cMaster::set_data_rd(uint8_t v) {
    data_rd_ = v;
}

uint8_t I2cMaster::get_data_rd() const {
    return data_rd_;
}

void I2cMaster::set_data_wr(uint8_t v) {
    data_wr_ = v;
}

uint8_t I2cMaster::get_data_wr() const {
    return data_wr_;
}

void I2cMaster::set_ack(simics::types::i2c_ack_t v) {
    ack_ = v;
}

simics::types::i2c_ack_t I2cMaster::get_ack() const {
    return ack_;
}

void I2cMaster::i2c_b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
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

void I2cMaster::acknowledge(simics::types::i2c_ack_t ack) {
    SC_REPORT_INFO(::TAG, "I2C master operation: acknowledge");
    ack_ = ack;
    if (ack == simics::types::I2C_ack) {
        if (data_wr_ != 0)
            extension_.write(data_wr_);
        else
            extension_.read();
    } else
        extension_.stop();
}

void I2cMaster::read_response(uint8_t value) {
    SC_REPORT_INFO(::TAG, "I2C master operation: read_response");
    data_rd_ = value;
    extension_.stop();
}
