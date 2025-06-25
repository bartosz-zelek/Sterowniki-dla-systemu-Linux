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

#ifndef SIMICS_SYSTEMC_IFACE_SC_OBJECT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_OBJECT_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_object_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScObjectInterface>
class ScObjectSimicsAdapter : public SimicsAdapter<sc_object_interface_t> {
  public:
    ScObjectSimicsAdapter()
        : SimicsAdapter<sc_object_interface_t>(
            SC_OBJECT_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t sc_print(conf_object_t *obj) {
        std::vector<std::string> lines =
            adapter<TBase, TInterface>(obj)->sc_print();

        return vectorToAttr(lines);
    }
    static attr_value_t sc_dump(conf_object_t *obj) {
        std::vector<std::string> lines
            = adapter<TBase, TInterface>(obj)->sc_dump();

        return vectorToAttr(lines);
    }
    static attr_value_t sc_kind(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->sc_kind();
    }
    static const char *sc_name(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->sc_name();
    }

  private:
    static attr_value_t vectorToAttr(const std::vector<std::string> &lines) {
        attr_value_t list = SIM_alloc_attr_list(lines.size());
        for (unsigned i = 0; i < lines.size(); ++i)
            SIM_attr_list_set_item(&list, i,
                                   SIM_make_attr_string(lines[i].c_str()));

        return list;
    }
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_object_interface_t init_iface() {
        sc_object_interface_t iface = {};
        iface.sc_print = sc_print;
        iface.sc_dump = sc_dump;
        iface.sc_kind = sc_kind;
        iface.sc_name = sc_name;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
