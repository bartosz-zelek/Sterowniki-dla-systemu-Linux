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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_DEVICE_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_DEVICE_H

#include <simics/systemc/iface/pci_device_interface.h>

class MockPciDevice : public simics::systemc::iface::PciDeviceInterface {
  public:
    MockPciDevice()
        : bus_reset_(0), system_error_(0),
          interrupt_raised_(0), interrupt_raised_pin_(0),
          interrupt_lowered_(0), interrupt_lowered_pin_(0) {
    }
    virtual void bus_reset() {
        ++bus_reset_;
    }
    virtual void system_error() {
        ++system_error_;
    }
    virtual void interrupt_raised(int pin) {
        ++interrupt_raised_;
        interrupt_raised_pin_ = pin;
    }
    virtual void interrupt_lowered(int pin) {
        ++interrupt_lowered_;
        interrupt_lowered_pin_ = pin;
    }

    int bus_reset_;
    int system_error_;
    int interrupt_raised_;
    int interrupt_raised_pin_;
    int interrupt_lowered_;
    int interrupt_lowered_pin_;
};

#endif
