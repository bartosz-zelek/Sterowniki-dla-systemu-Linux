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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_MII_MANAGEMENT_H
#define SIMICS_SYSTEMC_SIMICS2TLM_MII_MANAGEMENT_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/mii_management_extension.h>
#include <simics/systemc/iface/mii_management_interface.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics C++ MiiManagementInterface and
 * translates it into a TLM transaction with protocol specific
 * MiiManagementExtension.
 */
class MiiManagement : public iface::MiiManagementInterface,
                      public GasketOwner {
  public:
    // GasketOwner
    void gasketUpdated() override;

    // MiiManagementInterface
    int serial_access(int data_in, int clock) override;
    uint16 read_register(int phy, int reg) override;
    void write_register(int phy, int reg, uint16 value) override;

  private:
    ExtensionSender sender_;
    iface::MiiManagementExtension extension_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
