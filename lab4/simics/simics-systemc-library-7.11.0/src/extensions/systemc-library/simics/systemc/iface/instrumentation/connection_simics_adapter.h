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

#ifndef SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_CONNECTION_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_INSTRUMENTATION_CONNECTION_SIMICS_ADAPTER_H

#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/systemc/iface/instrumentation/connection_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {
namespace instrumentation {

template<typename TBase, typename TInterface = ConnectionInterface>
class ConnectionSimicsAdapter
    : public SimicsAdapter<instrumentation_connection_interface_t> {
  public:
    ConnectionSimicsAdapter()
        : SimicsAdapter<instrumentation_connection_interface_t>(
            INSTRUMENTATION_CONNECTION_INTERFACE, init_iface()) {
    }

  protected:
    static void enable(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->enable();
    }
    static void disable(conf_object_t *obj) {
        adapter<TBase, TInterface>(obj)->disable();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                       DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    instrumentation_connection_interface_t init_iface() {
        instrumentation_connection_interface_t iface = {};
        iface.enable = enable;
        iface.disable = disable;
        return iface;
    }
};

}  // namespace instrumentation
}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
