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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_SERIAL_DEVICE_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_SERIAL_DEVICE_H

#include <simics/systemc/iface/serial_device_interface.h>

class MockSerialDevice : public simics::systemc::iface::SerialDeviceInterface {
  public:
    MockSerialDevice()
        : write_(0), write_value_(0),
          write_return_(0), receive_ready_(0) {
    }
    virtual int write(int value) {
        ++write_;
        write_value_ = value;
        return write_return_;
    }
    virtual void receive_ready() {
        ++receive_ready_;
    }

    int write_;
    int write_value_;
    int write_return_;
    int receive_ready_;
};

#endif
