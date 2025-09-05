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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I2C_MASTER_V2_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I2C_MASTER_V2_H

#include <simics/systemc/iface/i2c_master_v2_interface.h>
#include <simics/types/i2c_ack.h>

class MockI2cMasterV2 : public simics::systemc::iface::I2cMasterV2Interface {
  public:
    MockI2cMasterV2()
        : acknowledge_(0),
          acknowledge_ack_(simics::types::I2C_noack),
          read_response_(0),
          read_response_value_(0) {
    }

    virtual void acknowledge(simics::types::i2c_ack_t ack) {
        ++acknowledge_;
        acknowledge_ack_ = ack;
    }

    virtual void read_response(uint8_t value) {
        ++read_response_;
        read_response_value_ = value;
    }

    int acknowledge_;
    simics::types::i2c_ack_t acknowledge_ack_;
    int read_response_;
    uint8_t read_response_value_;
};

#endif
