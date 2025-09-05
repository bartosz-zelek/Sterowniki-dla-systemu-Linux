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

#ifndef SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_PCIE_DEVICE_SIMICS_ADAPTER_H

#include <simics/devs/pci.h>
#include <simics/systemc/iface/pcie_device_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics pcie_device interface. */
template<typename TBase, typename TInterface = PcieDeviceInterface>
class PcieDeviceSimicsAdapter : public SimicsAdapter<pcie_device_interface_t> {
  public:
    PcieDeviceSimicsAdapter()
        : SimicsAdapter<pcie_device_interface_t>(
            PCIE_DEVICE_INTERFACE, init_iface()) {
    }

  protected:
    static void connected(conf_object_t *obj, conf_object_t *port_obj,
                          uint16 device_id) {
        adapter<TBase, TInterface>(obj)->connected(port_obj, device_id);
    }
    static void disconnected(conf_object_t *obj, conf_object_t *port_obj,
                             uint16 device_id) {
        adapter<TBase, TInterface>(obj)->disconnected(port_obj, device_id);
    }
    static void hot_reset(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->hot_reset();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    pcie_device_interface_t init_iface() {
        pcie_device_interface_t iface = {};
        iface.connected = connected;
        iface.disconnected = disconnected;
        iface.hot_reset = hot_reset;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
