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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_UPSTREAM_OPERATION_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_UPSTREAM_OPERATION_H



#include <simics/systemc/iface/pci_upstream_operation_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectPciUpstreamOperation : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("pci_upstream_operation.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "read") {
            AttrDictParser p = parser->init(attr);
            uint16_t rid = 0;
            if (!p.lookUp("rid", &rid))
                return false;

            simics::types::addr_space_t space;
            if (!addrSpaceLookUp(&p, &space, "space"))
                return false;

            ExtensionSetter<TPAYLOAD,
                iface::PciUpstreamOperationExtension>(gp)->read(rid, space);
            return true;
        } else if (key == "write") {
            AttrDictParser p = parser->init(attr);
            uint16_t rid = 0;
            if (!p.lookUp("rid", &rid))
                return false;

            simics::types::addr_space_t space;
            if (!addrSpaceLookUp(&p, &space, "space"))
                return false;

            ExtensionSetter<TPAYLOAD,
                iface::PciUpstreamOperationExtension>(gp)->write(rid, space);
            return true;
        }

        return false;
    }
    bool addrSpaceLookUp(AttrDictParser *parser, simics::types::addr_space_t *v,
                         std::string key) {
        uint64_t space = 0;
        if (!parser->lookUp("space", &space))
            return false;

        if (space > types::Sim_Addr_Space_Memory) {
            parser->reportError("%s must be in range [%i - %i]",
                                key.c_str(),
                                types::Sim_Addr_Space_Conf,
                                types::Sim_Addr_Space_Memory);
            return false;
        }

        *v = static_cast<types::addr_space_t>(space);
        return true;
    }
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
