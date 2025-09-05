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

#ifndef SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_PCI_EXPRESS_SIMICS_ADAPTER_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/devs/pci.h>
#include <simics/systemc/iface/pci_express_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics pci_express interface. */
template<typename TBase, typename TInterface = PciExpressInterface>
class PciExpressSimicsAdapter : public SimicsAdapter<pci_express_interface_t> {
  public:
    PciExpressSimicsAdapter()
        : SimicsAdapter<pci_express_interface_t>(
            PCI_EXPRESS_INTERFACE, init_iface()) {
    }

  protected:
    static int send_message(conf_object_t *obj, conf_object_t *src,
                            pcie_message_type_t type, byte_string_t payload) {
        // Need to convert between Simics byte_string_t and C++ type
        std::vector<uint8_t> p(payload.str, payload.str+payload.len);
        return adapter<TBase, TInterface>(obj)->send_message(type, p);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    pci_express_interface_t init_iface() {
        pci_express_interface_t iface = {};
        iface.send_message = send_message;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
#endif
