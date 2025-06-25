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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_PROVIDER_CONTROLLER_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_PROVIDER_CONTROLLER_SIMICS_ADAPTER_H

#include <simics/systemc/iface/simics_adapter.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <systemc-provider-interfaces.h>
#include <systemc>

#include <string>
#include <vector>

#include "provider_controller_interface.h"

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

template<typename TBase, typename TInterface = ProviderControllerInterface>
class ProviderControllerSimicsAdapter
    : public SimicsAdapter<sc_provider_controller_interface_t> {
  public:
    ProviderControllerSimicsAdapter()
        : SimicsAdapter<sc_provider_controller_interface_t>(
            SC_PROVIDER_CONTROLLER_INTERFACE, init_iface()) {
    }

  protected:
    static bool insert(conf_object_t *obj, conf_object_t *conn, int pos) {
        return adapter<TBase, TInterface>(obj)->insert(static_cast<
            simics::systemc::instrumentation::ToolConnectionInterface *>(
                SIM_object_data(conn)), pos);
    }
    static void remove(conf_object_t *obj, conf_object_t *conn) {
        adapter<TBase, TInterface>(obj)->remove(static_cast<
            simics::systemc::instrumentation::ToolConnectionInterface *>(
                SIM_object_data(conn)));
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_provider_controller_interface_t init_iface() {
        sc_provider_controller_interface_t iface = {};
        iface.insert = insert;
        iface.remove = remove;
        return iface;
    }
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
