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

#ifndef SIMICS_SYSTEMC_IFACE_SERIAL_DEVICE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SERIAL_DEVICE_SIMICS_ADAPTER_H

#include <simics/devs/serial-device.h>
#include <simics/systemc/iface/serial_device_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

template<typename TBase,
         typename TInterface = SerialDeviceInterface>
class SerialDeviceSimicsAdapter
    : public SimicsAdapter<serial_device_interface_t> {
  public:
    SerialDeviceSimicsAdapter()
        : SimicsAdapter<serial_device_interface_t>(
            SERIAL_DEVICE_INTERFACE, init_iface()) {
    }

  protected:
    static int write(conf_object_t *obj, int value) {
        return adapter<TBase, TInterface>(obj)->write(value);
    }
    static void receive_ready(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->receive_ready();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    serial_device_interface_t init_iface() {
        serial_device_interface_t iface = {};
        iface.write = write;
        iface.receive_ready = receive_ready;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
