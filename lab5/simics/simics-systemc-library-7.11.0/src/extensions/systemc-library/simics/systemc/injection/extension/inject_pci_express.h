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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_EXPRESS_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_EXPRESS_H

#if defined SIMICS_5_API || defined SIMICS_6_API

#include <simics/systemc/iface/pci_express_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectPciExpress : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("pci_express.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "send_message") {
            AttrDictParser p = parser->init(attr);
            std::vector<uint8_t> payload;
            if (!p.lookUp("payload", &payload))
                return false;

            int type = 0;
            if (!p.lookUp("type", &type))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciExpressExtension>(gp)
                ->send_message(type, payload);
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
#endif
