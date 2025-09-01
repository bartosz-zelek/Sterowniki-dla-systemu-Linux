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

#ifndef SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_INTERFACE_H
#define SIMICS_SYSTEMC_IFACE_MII_MANAGEMENT_INTERFACE_H

#include <stdint.h>

namespace simics {
namespace systemc {
namespace iface {

class MiiManagementInterface {
  public:
    virtual int serial_access(int data_in, int clock) = 0;
    virtual uint16_t read_register(int phy, int reg) = 0;
    virtual void write_register(int phy, int reg, uint16_t value) = 0;

    virtual ~MiiManagementInterface() = default;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
