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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_I3C_MASTER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_I3C_MASTER_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/i3c_master_extension.h>
#include <simics/systemc/iface/i3c_master_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics i3c_master interface and translates it
 * into a TLM transaction.
 */
class I3cMaster : public iface::I3cMasterInterface,
                  public GasketOwner {
  public:
    virtual void gasketUpdated();

    void acknowledge(types::i3c_ack_t ack);
    void read_response(uint8_t value, bool more);
    void daa_response(uint64_t id, uint8_t bcr, uint8_t dcr);
    void ibi_request();
    void ibi_address(uint8_t address);

  private:
    ExtensionSender sender_;
    iface::I3cMasterExtension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
