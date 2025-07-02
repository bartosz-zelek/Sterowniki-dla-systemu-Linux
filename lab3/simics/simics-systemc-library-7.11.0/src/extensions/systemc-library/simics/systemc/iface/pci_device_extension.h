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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_DEVICE_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCI_DEVICE_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pci_device_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pci_device interface. See base class for details. */
class PciDeviceExtension : public Extension<PciDeviceExtension,
                                            PciDeviceInterface> {
  public:
    virtual void call(PciDeviceInterface *device) {
        switch (method_.value<Method>()) {
        case BUS_RESET:
            device->bus_reset();
            break;
        case SYSTEM_ERROR:
            device->system_error();
            break;
        case INTERRUPT_RAISED:
            device->interrupt_raised(method_input_[0].value<int>());
            break;
        case INTERRUPT_LOWERED:
            device->interrupt_lowered(method_input_[0].value<int>());
            break;
        }
    }

    virtual void bus_reset() {
        method_ = BUS_RESET;
        send();
    }
    virtual void system_error() {
        method_ = SYSTEM_ERROR;
        send();
    }
    virtual void interrupt_raised(int pin) {
        method_input_.push_back(pin);
        method_ = INTERRUPT_RAISED;
        send();
    }
    virtual void interrupt_lowered(int pin) {
        method_input_.push_back(pin);
        method_ = INTERRUPT_LOWERED;
        send();
    }

  private:
    enum Method {
        BUS_RESET,
        SYSTEM_ERROR,
        INTERRUPT_RAISED,
        INTERRUPT_LOWERED
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
