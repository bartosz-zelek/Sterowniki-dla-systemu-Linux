// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_SC_VERSION_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_VERSION_SIMICS_ADAPTER_H

#include <simics/base/attr-value.h>
#include <simics/systemc/iface/sc_version_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <map>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ScVersionInterface>
class ScVersionSimicsAdapter : public SimicsAdapter<sc_version_interface_t> {
  public:
    ScVersionSimicsAdapter() : SimicsAdapter<sc_version_interface_t>(
            SC_VERSION_INTERFACE, init_iface()) {
    }

  protected:
    static const char *kernel_version(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->kernel_version();
    }
    static const char *library_version(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->library_version();
    }
    static const char *library_kernel_version(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->library_kernel_version();
    }
    static attr_value_t versions(conf_object_t *obj) {
        const std::map<std::string, std::string> *v =
            adapter<TBase, TInterface>(obj)->versions();
        unsigned idx = 0;
        std::map<std::string, std::string>::const_iterator i;
        attr_value_t dict = SIM_alloc_attr_dict(v->size());
        for (i = v->begin(); i != v->end(); ++i, ++idx) {
            SIM_attr_dict_set_item(&dict, idx,
                                   SIM_make_attr_string(i->first.c_str()),
                                   SIM_make_attr_string(i->second.c_str()));
        }
        return dict;
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_version_interface_t init_iface() {
        sc_version_interface_t iface = {};
        iface.kernel_version = kernel_version;
        iface.library_version = library_version;
        iface.library_kernel_version = library_kernel_version;
        iface.versions = versions;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
