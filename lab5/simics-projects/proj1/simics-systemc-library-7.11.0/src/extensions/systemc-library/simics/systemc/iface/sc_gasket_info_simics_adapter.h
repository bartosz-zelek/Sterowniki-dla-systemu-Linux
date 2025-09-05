// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_SC_GASKET_INFO_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_GASKET_INFO_SIMICS_ADAPTER_H

#include <simics/base/attr-value.h>

#include <simics/systemc/iface/sc_gasket_info_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScGasketInfoInterface>
class ScGasketInfoSimicsAdapter
    : public SimicsAdapter<sc_gasket_info_interface_t> {
  public:
    ScGasketInfoSimicsAdapter() : SimicsAdapter<sc_gasket_info_interface_t>(
            SC_GASKET_INFO_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t simics2tlm(conf_object_t *obj) {
        return vec2list(adapter<TBase, TInterface>(obj)->simics2tlm());
    }
    static attr_value_t tlm2simics(conf_object_t *obj) {
        return vec2list(adapter<TBase, TInterface>(obj)->tlm2simics());
    }
    static attr_value_t simics2systemc(conf_object_t *obj) {
        return vec2list(adapter<TBase, TInterface>(obj)->simics2systemc());
    }
    static attr_value_t systemc2simics(conf_object_t *obj) {
        return vec2list(adapter<TBase, TInterface>(obj)->systemc2simics());
    }
    static attr_value_t vec2list(
            const std::vector<std::vector<std::string> > *v) {
        attr_value_t l1 = SIM_alloc_attr_list(v->size());
        int lidx1 = 0;
        for (auto i = v->begin(); i != v->end(); ++i) {
            attr_value_t l2 = SIM_alloc_attr_list(i->size());
            int lidx2 = 0;
            SIM_attr_list_set_item(&l1, lidx1++, l2);
            for (auto j = i->begin(); j != i->end(); ++j) {
                SIM_attr_list_set_item(&l2, lidx2++,
                                       SIM_make_attr_string(j->c_str()));
            }
        }
        return l1;
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_gasket_info_interface_t init_iface() {
        sc_gasket_info_interface_t iface = {};
        iface.simics2tlm = simics2tlm;
        iface.tlm2simics = tlm2simics;
        iface.simics2systemc = simics2systemc;
        iface.systemc2simics = systemc2simics;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
