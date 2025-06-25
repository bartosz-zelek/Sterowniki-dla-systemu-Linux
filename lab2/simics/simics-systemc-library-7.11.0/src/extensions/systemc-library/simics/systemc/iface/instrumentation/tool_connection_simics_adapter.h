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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TOOL_CONNECTION_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TOOL_CONNECTION_SIMICS_ADAPTER_H

#include <systemc-provider-interfaces.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

template<typename TBase, typename TInterface =
    systemc::instrumentation::ToolConnectionInterface>
class ToolConnectionSimicsAdapter
    : public SimicsAdapter<sc_tool_connection_interface_t> {
  public:
    ToolConnectionSimicsAdapter()
        : SimicsAdapter<sc_tool_connection_interface_t>(
            SC_TOOL_CONNECTION_INTERFACE, init_iface()) {
    }

  protected:
    static attr_value_t tool(conf_object_t *obj) {
        return SIM_make_attr_object(adapter<TBase, TInterface>(obj)->tool());
    }
    static attr_value_t controller(conf_object_t *obj) {
        return SIM_make_attr_object(
            adapter<TBase, TInterface>(obj)->controller());
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_tool_connection_interface_t init_iface() {
        sc_tool_connection_interface_t iface = {};
        iface.tool = tool;
        iface.controller = controller;
        return iface;
    }
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
