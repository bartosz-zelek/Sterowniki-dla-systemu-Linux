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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_EXPRESS_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_PCI_EXPRESS_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/systemc/iface/pci_express_interface.h>

#include <vector>

class MockPciExpress : public simics::systemc::iface::PciExpressInterface {
  public:
    MockPciExpress()
        : send_message_(0), type_(0),
          send_message_return_(0) {
    }
    virtual int send_message(int type, const std::vector<uint8_t> &payload) {
        ++send_message_;
        type_ = type;
        payload_ = payload;
        return send_message_return_;
    }

    int send_message_;
    int type_;
    std::vector<uint8_t> payload_;
    int send_message_return_;
};

#endif
#endif
