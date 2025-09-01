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

#ifndef SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_BUS_H
#define SIMICS_SYSTEMC_INJECTION_EXTENSION_INJECT_PCI_BUS_H



#include <simics/systemc/iface/pci_bus_extension.h>
#include <simics/systemc/injection/extension/extension_setter.h>
#include <simics/systemc/injection/extension/map_info_parser.h>
#include <simics/systemc/injection/inject_base.h>

#include <string>

namespace simics {
namespace systemc {
namespace injection {
namespace extension {

/** \internal */
template <typename TPAYLOAD>
class InjectPciBus : public InjectBase<TPAYLOAD> {
  public:
    ATTR_DICT_PARSER_NAMESPACE("pci_bus.")

    virtual bool setValue(AttrDictParser *parser, const std::string &key,
                          attr_value_t *attr, TPAYLOAD *gp) {
        if (key == "raise_interrupt") {
            int64_t v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->raise_interrupt(v);
        } else if (key == "lower_interrupt") {
            int64_t v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->lower_interrupt(v);
        } else if (key == "interrupt_acknowledge") {
            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->interrupt_acknowledge();
        } else if (key == "set_bus_number") {
            int64_t v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->set_bus_number(v);
        } else if (key == "set_sub_bus_number") {
            int64_t v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->set_sub_bus_number(v);
        } else if (key == "bus_reset") {
            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->bus_reset();
        } else if (key == "special_cycle") {
            uint32_t v = 0;
            if (!parser->value(&v))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->special_cycle(v);
        } else if (key == "system_error") {
            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->system_error();
        } else if (key == "get_bus_address") {
            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->get_bus_address();
        } else if (key == "add_map") {
            AttrDictParser p = parser->init(attr);
            types::addr_space_t space;
            if (!addrSpaceLookUp(&p, &space, "space"))
                return false;

            attr_value_t info;
            if (!p.lookUp("info", &info))
                return false;

            types::map_info_t map_info;
            if (!value(parser, &info, &map_info))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->add_map(space, map_info);
        } else if (key == "remove_map") {
            AttrDictParser p = parser->init(attr);
            types::addr_space_t space;
            if (!addrSpaceLookUp(&p, &space, "space"))
                return false;

            int32_t function = 0;
            if (!p.lookUp("function", &function))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->remove_map(space, function);
        } else if (key == "add_default") {
            AttrDictParser p = parser->init(attr);
            types::addr_space_t space;
            if (!addrSpaceLookUp(&p, &space, "space"))
                return false;

            attr_value_t info;
            if (!p.lookUp("info", &info))
                return false;

            types::map_info_t map_info;
            if (!value(parser, &info, &map_info))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->add_default(space, map_info);
        } else if (key == "remove_default") {
            types::addr_space_t v;
            if (!addrSpaceValue(parser, &v, key))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->remove_default(v);
        } else if (key == "set_device_status") {
            AttrDictParser p = parser->init(attr);
            int32_t device = 0;
            int32_t function = 0;
            int32_t enabled = 0;
            if (!p.lookUp("device", &device))
                return false;

            if (!p.lookUp("function", &function))
                return false;

            if (!p.lookUp("enabled", &enabled))
                return false;

            ExtensionSetter<TPAYLOAD, iface::PciBusExtension>(gp)
                ->set_device_status(device, function, enabled);
        } else {
            return false;
        }

        return true;
    }
    bool addrSpaceValue(AttrDictParser *parser, simics::types::addr_space_t *v,
                        std::string key) {
        uint64_t space = 0;
        if (!parser->value(&space))
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
    bool value(AttrDictParser *parser, const attr_value_t *attr,
               types::map_info_t *map_info) {
        AttrDictParser p = parser->init(attr);
        MapInfoParser info;
        if (!p.parse(&info))
            return false;

        *map_info = info.map_info_;
        return true;
    }
};

}  // namespace extension
}  // namespace injection
}  // namespace systemc
}  // namespace simics

#endif
