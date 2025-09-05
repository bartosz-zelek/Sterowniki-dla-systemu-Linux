// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_DEVICE_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_DEVICE_H



#include <simics/systemc/iface/pci_device_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectPciDevice : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("pci_device.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "bus_reset") {
            ExtensionSetter<TPAYLOAD, iface::PciDeviceExtension>(gp)
                ->bus_reset();
            return true;
        } else if (key == "system_error") {
            ExtensionSetter<TPAYLOAD, iface::PciDeviceExtension>(gp)
                ->system_error();
            return true;
        } else if (key == "interrupt_raised") {
            int pin = 0;
            if (!parser->value(&pin))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciDeviceExtension>(gp)
                ->interrupt_raised(pin);
            return true;
        } else if (key == "interrupt_lowered") {
            int pin = 0;
            if (!parser->value(&pin))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciDeviceExtension>(gp)
                ->interrupt_lowered(pin);
            return true;
        }

        return false;
    }
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
