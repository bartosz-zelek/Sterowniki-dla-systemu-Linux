// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I3C_SLAVE_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I3C_SLAVE_H

#include <simics/systemc/iface/i3c_slave_interface.h>

class MockI3cSlave : public simics::systemc::iface::I3cSlaveInterface {
  public:
    MockI3cSlave()
        : start_(0), start_address_(0), write_(0), write_value_(0),
          sdr_write_(0), read_(0), daa_read_(0), stop_(0),
          ibi_start_(0), ibi_acknowledge_(0),
          ibi_acknowledge_ack_(simics::types::I3C_noack) {
    }

    virtual void start(uint8_t address) {
        ++start_;
        start_address_ = address;
    }
    virtual void write(uint8_t value) {
        ++write_;
        write_value_ = value;
    }
    virtual void sdr_write(simics::types::bytes_t data) {
        ++sdr_write_;
        sdr_write_data_ = data;
    }
    virtual void read() {
        ++read_;
    }
    virtual void daa_read() {
        ++daa_read_;
    }
    virtual void stop() {
        ++stop_;
    }
    virtual void ibi_start() {
        ++ibi_start_;
    }
    virtual void ibi_acknowledge(simics::types::i3c_ack_t ack) {
        ++ibi_acknowledge_;
        ibi_acknowledge_ack_ = ack;
    }

    int start_;
    int start_address_;
    int write_;
    int write_value_;
    int sdr_write_;
    simics::types::bytes_t sdr_write_data_;
    int read_;
    int daa_read_;
    int stop_;
    int ibi_start_;
    int ibi_acknowledge_;
    simics::types::i3c_ack_t ibi_acknowledge_ack_;
};

#endif
