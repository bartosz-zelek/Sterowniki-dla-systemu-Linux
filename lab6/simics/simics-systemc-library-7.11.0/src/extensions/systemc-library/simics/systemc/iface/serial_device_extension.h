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

#ifndef SIMICS_SYSTEMC_IFACE_SERIAL_DEVICE_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_SERIAL_DEVICE_EXTENSION_H

#include <simics/systemc/iface/extension.h>
#include <simics/systemc/iface/serial_device_interface.h>

namespace simics {
namespace systemc {
namespace iface {

//:: pre SerialDeviceExtension {{
class SerialDeviceExtension : public Extension<SerialDeviceExtension,
                                               SerialDeviceInterface> {
  public:
    virtual void call(SerialDeviceInterface *device) {
        switch (method_.value<Method>()) {
        case WRITE:
            method_return_ = device->write(method_input_[0].value<int>());
            break;
        case RECEIVE_READY:
            device->receive_ready();
            break;
        }
    }

    virtual int write(int value) {
        method_input_.push_back(value);
        method_ = WRITE;
        send();
        return method_return_.value<int>();
    }
    virtual void receive_ready() {
        method_ = RECEIVE_READY;
        send();
    }

  private:
    enum Method {
        WRITE,
        RECEIVE_READY
    };
};
// }}

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
