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

#ifndef SIMICS_SYSTEMC_IFACE_SC_PORT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_PORT_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_port_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScPortInterface>
class ScPortSimicsAdapter : public SimicsAdapter<sc_port_interface_t> {
  public:
    ScPortSimicsAdapter() : SimicsAdapter<sc_port_interface_t>(
        SC_PORT_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t port_to_proxies(conf_object_t *obj) {
        return stl2simics(
            adapter<TBase, TInterface>(obj)->port_to_proxies());
    }
    static const char *if_typename(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->if_typename();
    }
    static int max_number_of_proxies(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->max_number_of_proxies();
    }

  private:
    static attr_value_t stl2simics(std::vector<conf_object_t *> proxies) {
        attr_value_t list = SIM_alloc_attr_list(proxies.size());
        for (unsigned i = 0; i < proxies.size(); ++i)
            SIM_attr_list_set_item(&list, i,
                                   SIM_make_attr_object(proxies[i]));

        return list;
    }
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_port_interface_t init_iface() {
        sc_port_interface_t iface = {};
        iface.port_to_proxies = port_to_proxies;
        iface.if_typename = if_typename;
        iface.max_number_of_proxies = max_number_of_proxies;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
