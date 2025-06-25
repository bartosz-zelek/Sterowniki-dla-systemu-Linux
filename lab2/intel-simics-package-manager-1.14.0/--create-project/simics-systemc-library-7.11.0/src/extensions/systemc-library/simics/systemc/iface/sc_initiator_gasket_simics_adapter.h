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

#ifndef SIMICS_SYSTEMC_IFACE_SC_INITIATOR_GASKET_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_INITIATOR_GASKET_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_initiator_gasket_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase,
         typename TInterface = ScInitiatorGasketInterface>
class ScInitiatorGasketSimicsAdapter
    : public SimicsAdapter<sc_initiator_gasket_interface_t> {
  public:
    ScInitiatorGasketSimicsAdapter()
        : SimicsAdapter<sc_initiator_gasket_interface_t>(
            SC_INITIATOR_GASKET_INTERFACE, init_iface()) {
    }

  protected:
    static void set_dmi(conf_object_t *obj, bool enable) {
        adapter<TBase, TInterface>(obj)->set_dmi(enable);
    }
    static bool is_dmi_enabled(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->is_dmi_enabled();
    }
    static char *print_dmi_table(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->print_dmi_table();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_initiator_gasket_interface_t init_iface() {
        sc_initiator_gasket_interface_t iface = {};
        iface.set_dmi = set_dmi;
        iface.is_dmi_enabled = is_dmi_enabled;
        iface.print_dmi_table = print_dmi_table;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
