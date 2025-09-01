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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TOOL_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_TOOL_SIMICS_ADAPTER_H

#include <simics/base/attr-value.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/systemc/iface/instrumentation/tool_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

template<typename TBase, typename TInterface = ToolInterface>
class ToolSimicsAdapter
    : public SimicsAdapter<instrumentation_tool_interface_t> {
  public:
    ToolSimicsAdapter()
        : SimicsAdapter<instrumentation_tool_interface_t>(
            INSTRUMENTATION_TOOL_INTERFACE, init_iface()) {
    }

  protected:
    static conf_object_t *connect(conf_object_t *obj, conf_object_t *provider,
                                  attr_value_t args) {
        return adapter<TBase, TInterface>(obj, provider)->connect(provider,
                                                                  args);
    }
    static void disconnect(conf_object_t *obj, conf_object_t *conn) {
        adapter<TBase, TInterface>(obj, conn)->disconnect(conn);
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    instrumentation_tool_interface_t init_iface() {
        instrumentation_tool_interface_t iface = {};
        iface.connect = connect;
        iface.disconnect = disconnect;
        return iface;
    }
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
