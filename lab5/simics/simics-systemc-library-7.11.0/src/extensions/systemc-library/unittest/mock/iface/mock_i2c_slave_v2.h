// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I2C_SLAVE_V2_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I2C_SLAVE_V2_H

#include <simics/systemc/iface/i2c_slave_v2_interface.h>

#include <vector>

class MockI2cSlaveV2 : public simics::systemc::iface::I2cSlaveV2Interface {
  public:
    MockI2cSlaveV2()
        : start_(0), start_address_(0), read_(0),
          write_(0), write_value_(0), stop_(0), addresses_(0) {
    }

    virtual void start(uint8_t address) {
        ++start_;
        start_address_ = address;
    }
    virtual void read() {
        ++read_;
    }
    virtual void write(uint8_t value) {
        ++write_;
        write_value_ = value;
    }
    virtual void stop() {
        ++stop_;
    }
    virtual std::vector<uint8_t> addresses() {
        ++addresses_;
        return addresses_return_value_;
    }

    int start_;
    int start_address_;
    int read_;
    int write_;
    int write_value_;
    int stop_;
    int addresses_;
    std::vector<uint8_t> addresses_return_value_;
};

#endif
