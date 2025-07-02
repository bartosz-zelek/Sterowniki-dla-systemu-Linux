// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_MII_MANAGEMENT_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_MII_MANAGEMENT_H

#include <simics/systemc/iface/mii_management_interface.h>

class MockMiiManagement
    : public simics::systemc::iface::MiiManagementInterface {
  public:
    MockMiiManagement()
        : read_register_(0),
          read_register_phy_(0),
          read_register_reg_(0),
          read_register_return_(0),
          write_register_(0),
          write_register_phy_(0),
          write_register_reg_(0),
          write_register_value_(0),
          serial_access_(0),
          serial_access_data_in_(0),
          serial_access_clock_(0),
          serial_access_return_(0) {
    }

    uint16_t read_register(int phy, int reg) override {
        ++read_register_;
        read_register_phy_ = phy;
        read_register_reg_ = reg;
        return read_register_return_;
    }
    void write_register(int phy, int reg, uint16_t value) override {
        ++write_register_;
        write_register_phy_ = phy;
        write_register_reg_ = reg;
        write_register_value_ = value;
    }
    int serial_access(int data_in, int clock) override {
        ++serial_access_;
        serial_access_data_in_ = data_in;
        serial_access_clock_ = clock;
        return serial_access_return_;
    }

    int read_register_;
    int read_register_phy_;
    int read_register_reg_;
    uint16_t read_register_return_;
    int write_register_;
    int write_register_phy_;
    int write_register_reg_;
    uint16_t write_register_value_;
    int serial_access_;
    int serial_access_data_in_;
    int serial_access_clock_;
    int serial_access_return_;
};

#endif
