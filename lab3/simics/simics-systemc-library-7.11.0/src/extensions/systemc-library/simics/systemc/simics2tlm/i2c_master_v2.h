// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I2C_MASTER_V2_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I2C_MASTER_V2_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/i2c_master_v2_extension.h>
#include <simics/systemc/iface/i2c_master_v2_interface.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics i2c_master_v2 interface and translates it
 * into a TLM transaction.
 */
class I2cMasterV2 : public iface::I2cMasterV2Interface,
                    public GasketOwner {
  public:
    virtual void gasketUpdated();

    void acknowledge(types::i2c_ack_t ack);
    void read_response(uint8_t value);

  private:
    ExtensionSender sender_;
    iface::I2cMasterV2Extension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
