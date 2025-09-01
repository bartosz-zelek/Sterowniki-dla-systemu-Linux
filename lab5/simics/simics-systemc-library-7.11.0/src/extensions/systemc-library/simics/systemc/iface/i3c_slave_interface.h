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

#ifndef SIMICS_SYSTEMC_IFACE_I3C_SLAVE_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_I3C_SLAVE_INTERFACE_H

#include <stdint.h>
#include <simics/types/bytes.h>
#include <simics/types/i3c_ack.h>

namespace simics {
namespace systemc {
namespace iface {

class I3cSlaveInterface {
  public:
    virtual ~I3cSlaveInterface() {}
    virtual void start(uint8_t address) = 0;
    virtual void write(uint8_t value) = 0;
    virtual void sdr_write(types::bytes_t data) = 0;
    virtual void read() = 0;
    virtual void daa_read() = 0;
    virtual void stop() = 0;
    virtual void ibi_start() = 0;
    virtual void ibi_acknowledge(types::i3c_ack_t ack) = 0;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
