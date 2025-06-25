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

#ifndef SIMICS_SYSTEMC_IFACE_I3C_MASTER_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_I3C_MASTER_INTERFACE_H

#include <stdint.h>
#include <simics/types/i3c_ack.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics i3c_master interface. */
class I3cMasterInterface {
  public:
    virtual ~I3cMasterInterface() {}
    virtual void acknowledge(types::i3c_ack_t ack) = 0;
    virtual void read_response(uint8_t value, bool more) = 0;
    virtual void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) = 0;
    virtual void ibi_request() = 0;
    virtual void ibi_address(uint8_t address) = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
