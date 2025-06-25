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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I2C_SLAVE_V2_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I2C_SLAVE_V2_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/i2c_slave_v2_extension.h>
#include <simics/systemc/iface/i2c_slave_v2_interface.h>

#include <stdint.h>
#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics i2c_slave_v2 interface and translates it
 * into a TLM transaction.
 */
class I2cSlaveV2 : public iface::I2cSlaveV2Interface,
                   public GasketOwner {
  public:
    virtual void gasketUpdated();

    void start(uint8_t address);
    void read();
    void write(uint8_t data);
    void stop();
    std::vector<uint8_t> addresses();

  private:
    ExtensionSender sender_;
    iface::I2cSlaveV2Extension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
