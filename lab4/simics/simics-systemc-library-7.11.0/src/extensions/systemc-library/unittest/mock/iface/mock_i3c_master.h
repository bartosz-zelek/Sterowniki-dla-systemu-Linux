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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I3C_MASTER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_IFACE_MOCK_I3C_MASTER_H

#include <simics/systemc/iface/i3c_master_interface.h>
#include <simics/types/i3c_ack.h>

class MockI3cMaster : public simics::systemc::iface::I3cMasterInterface {
  public:
    MockI3cMaster()
        : acknowledge_(0),
          acknowledge_ack_(simics::types::I3C_noack),
          read_response_(0),
          read_response_value_(0),
          read_response_more_(false),
          daa_response_(0),
          daa_response_id_(0),
          daa_response_bcr_(0),
          daa_response_dcr_(0),
          ibi_request_(0),
          ibi_address_(0),
          ibi_address_address_(0) {
    }

    virtual void acknowledge(simics::types::i3c_ack_t ack) {
        ++acknowledge_;
        acknowledge_ack_ = ack;
    }

    virtual void read_response(uint8_t value, bool more) {
        ++read_response_;
        read_response_value_ = value;
        read_response_more_ = more;
    }

    virtual void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) {
        ++daa_response_;
        daa_response_id_ = id;
        daa_response_bcr_ = bcr;
        daa_response_dcr_ = dcr;
    }

    virtual void ibi_request() {
        ++ibi_request_;
    }

    virtual void ibi_address(uint8_t address) {
        ++ibi_address_;
        ibi_address_address_ = address;
    }

    int acknowledge_;
    simics::types::i3c_ack_t acknowledge_ack_;
    int read_response_;
    uint8_t read_response_value_;
    bool read_response_more_;
    int daa_response_;
    uint64_t daa_response_id_;
    uint8_t daa_response_bcr_;
    uint8_t daa_response_dcr_;
    int ibi_request_;
    int ibi_address_;
    uint8_t ibi_address_address_;
};

#endif
