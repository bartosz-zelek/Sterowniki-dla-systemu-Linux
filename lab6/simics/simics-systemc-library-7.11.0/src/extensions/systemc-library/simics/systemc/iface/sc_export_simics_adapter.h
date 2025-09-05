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

#ifndef SIMICS_SYSTEMC_IFACE_SC_EXPORT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_EXPORT_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_export_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScExportInterface>
class ScExportSimicsAdapter : public SimicsAdapter<sc_export_interface_t> {
  public:
    ScExportSimicsAdapter() : SimicsAdapter<sc_export_interface_t>(
        SC_EXPORT_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t export_to_proxy(conf_object_t *obj) {
        return SIM_make_attr_object(
            adapter<TBase, TInterface>(obj)->export_to_proxy());
    }
    static const char *if_typename(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->if_typename();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_export_interface_t init_iface() {
        sc_export_interface_t iface = {};
        iface.export_to_proxy = export_to_proxy;
        iface.if_typename = if_typename;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
