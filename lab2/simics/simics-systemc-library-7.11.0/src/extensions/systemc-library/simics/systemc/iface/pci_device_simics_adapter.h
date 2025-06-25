// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_PCI_DEVICE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_PCI_DEVICE_SIMICS_ADAPTER_H

#include <simics/devs/pci.h>
#include <simics/systemc/iface/pci_device_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

#define DEPRECATED_FUNC(f) _deprecated_ ## f

/** Adapter for Simics pci_device interface. */
template<typename TBase, typename TInterface = PciDeviceInterface>
class PciDeviceSimicsAdapter : public SimicsAdapter<pci_device_interface_t> {
  public:
    PciDeviceSimicsAdapter()
        : SimicsAdapter<pci_device_interface_t>(
            PCI_DEVICE_INTERFACE, init_iface()) {
    }

  protected:
    static void bus_reset(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->bus_reset();
    }
    static void system_error(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->system_error();
    }
    static void interrupt_raised(conf_object_t *obj, int pin) {
        adapter<TBase, TInterface>(obj)->interrupt_raised(pin);
    }
    static void interrupt_lowered(conf_object_t *obj, int pin) {
        adapter<TBase, TInterface>(obj)->interrupt_lowered(pin);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    pci_device_interface_t init_iface() {
        pci_device_interface_t iface = {};
        iface.bus_reset = bus_reset;
        iface.DEPRECATED_FUNC(interrupt_acknowledge) = NULL;
        iface.DEPRECATED_FUNC(special_cycle) = NULL;
        iface.system_error = system_error;
        iface.interrupt_raised = interrupt_raised;
        iface.interrupt_lowered = interrupt_lowered;
        return iface;
    }
};

#undef DEPRECATED_FUNC

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
