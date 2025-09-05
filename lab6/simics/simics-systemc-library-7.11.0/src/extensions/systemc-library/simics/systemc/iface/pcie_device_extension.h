// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2024 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/pcie_device_interface.h>

namespace simics {
namespace systemc {
namespace iface {

/** Extension for Simics pcie_device interface. See base class for details. */
class PcieDeviceExtension
    : public Extension<PcieDeviceExtension,
                       simics::systemc::iface::PcieDeviceInterface> {
  public:
    virtual void call(simics::systemc::iface::PcieDeviceInterface *device) {
        switch (method_.value<Method>()) {
        case CONNECTED:
            device->connected(method_input_[0].value<conf_object_t *>(),
                              method_input_[1].value<uint16_t>());
            break;
        case DISCONNECTED:
            device->disconnected(method_input_[0].value<conf_object_t *>(),
                                 method_input_[1].value<uint16_t>());
            break;
        case HOT_RESET:
            device->hot_reset();
            break;
        }
    }

    virtual void connected(conf_object_t *port_obj, uint16_t device_id) {
        method_input_.push_back(port_obj);
        method_input_.push_back(device_id);
        method_ = CONNECTED;
        send();
    }
    virtual void disconnected(conf_object_t *port_obj, uint16_t device_id) {
        method_input_.push_back(port_obj);
        method_input_.push_back(device_id);
        method_ = DISCONNECTED;
        send();
    }
    virtual void hot_reset() {
        method_ = HOT_RESET;
        send();
    }

  private:
    enum Method {
        CONNECTED,
        DISCONNECTED,
        HOT_RESET
    };
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
