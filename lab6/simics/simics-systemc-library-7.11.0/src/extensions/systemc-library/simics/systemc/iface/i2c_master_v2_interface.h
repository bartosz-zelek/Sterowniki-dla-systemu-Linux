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

#ifndef SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_I2C_MASTER_V2_INTERFACE_H

#include <stdint.h>
#include <simics/types/i2c_ack.h>

namespace simics {
namespace systemc {
namespace iface {

/** Simics i2c_master_v2 interface. */
class I2cMasterV2Interface {
  public:
    virtual void acknowledge(types::i2c_ack_t ack) = 0;
    virtual void read_response(uint8_t value) = 0;
    virtual ~I2cMasterV2Interface() {}
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
