// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_EXECUTE_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_EXECUTE_SIMICS_ADAPTER_H

#include <simics/model-iface/execute.h>
#include <simics/systemc/iface/execute_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase, typename TInterface = ExecuteInterface>
class ExecuteSimicsAdapter : public SimicsAdapter<execute_interface_t> {
  public:
    ExecuteSimicsAdapter()
        : SimicsAdapter<execute_interface_t>(EXECUTE_INTERFACE, init_iface()) {
    }

  protected:
    static void run(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->run();
    }
    static void stop(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->stop();
    }
    static void switch_in(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->switch_in();
    }
    static void switch_out(conf_object_t *obj) {
        adapterWithoutLocking<TBase, TInterface>(obj)->switch_out();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    execute_interface_t init_iface() {
        execute_interface_t iface = {};
        iface.run = run;
        iface.stop = stop;
        iface.switch_in = switch_in;
        iface.switch_out = switch_out;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
