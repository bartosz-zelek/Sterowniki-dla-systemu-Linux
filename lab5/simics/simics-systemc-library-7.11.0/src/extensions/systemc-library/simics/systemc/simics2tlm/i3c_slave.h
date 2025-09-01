// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I3C_SLAVE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I3C_SLAVE_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/i3c_slave_extension.h>
#include <simics/systemc/iface/i3c_slave_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics i3c_slave interface and translates it
 * into a TLM transaction.
 */
class I3cSlave : public iface::I3cSlaveInterface,
                 public GasketOwner {
  public:
    virtual void gasketUpdated();

    // I3cSlaveInterface
    void start(uint8_t address) override;
    void write(uint8_t value) override;
    void sdr_write(types::bytes_t data) override;
    void read() override;
    void daa_read() override;
    void stop() override;
    void ibi_start() override;
    void ibi_acknowledge(types::i3c_ack_t ack) override;

  private:
    ExtensionSender sender_;
    iface::I3cSlaveExtension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
