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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_SERIAL_DEVICE_H
#define SIMICS_SYSTEMC_SIMICS2TLM_SERIAL_DEVICE_H

#include <simics/systemc/simics2tlm/gasket_owner.h>
#include <simics/systemc/simics2tlm/extension_sender.h>
#include <simics/systemc/iface/serial_device_extension.h>
#include <simics/systemc/iface/serial_device_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Class that implements the Simics C++ SerialDeviceInterface and translates it
 * to a TLM transaction with protocol specific SerialDeviceExtension.
 */
//:: pre simics2tlm_SerialDevice {{
class SerialDevice : public simics::systemc::iface::SerialDeviceInterface,
                     public GasketOwner {
  public:
    virtual void gasketUpdated();
    // SerialDeviceInterface
    int write(int value);
    void receive_ready();
  private:
    ExtensionSender sender_;
    iface::SerialDeviceExtension extension_;
};
// }}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
