// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_ETHERNET_COMMON_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_ETHERNET_COMMON_H

#include <simics/systemc/iface/ethernet_common_interface.h>

class MockEthDevice : public simics::systemc::iface::EthernetCommonInterface {
  public:
    MockEthDevice()
        : frame_(0), frame_frame_(NULL), frame_crc_ok_(0) {
    }
    virtual void frame(const simics::types::frags_t *frame, int crc_ok) {
        ++frame_;
        frame_frame_ = frame;
        frame_crc_ok_ = crc_ok;
    }

    int frame_;
    const simics::types::frags_t *frame_frame_;
    int frame_crc_ok_;
};

#endif
