// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_EXECUTE_CONTROL_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_EXECUTE_CONTROL_SIMICS_ADAPTER_H

#include <simics/model-iface/execute.h>
#include <simics/systemc/iface/execute_control_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for concurrency group interface. */
template<typename TBase, typename TInterface = ExecuteControlInterface>
class ExecuteControlSimicsAdapter
    : public SimicsAdapter<execute_control_interface_t> {
  public:
    ExecuteControlSimicsAdapter()
        : SimicsAdapter<execute_control_interface_t>(EXECUTE_CONTROL_INTERFACE,
                                                     init_iface()) {}

  protected:
    static void message_pending(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->message_pending();
    }
    static void yield_request(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->yield_request();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    execute_control_interface_t init_iface() {
        execute_control_interface_t iface = {};
        iface.message_pending = message_pending;
        iface.yield_request = yield_request;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
