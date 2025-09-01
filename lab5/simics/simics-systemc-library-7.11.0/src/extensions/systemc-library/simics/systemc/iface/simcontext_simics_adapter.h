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

#ifndef SIMICS_SYSTEMC_IFACE_SIMCONTEXT_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SIMCONTEXT_SIMICS_ADAPTER_H

#include <simics/systemc/iface/simcontext_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** Adapter for Simics simulation context interface. */
template<typename TBase, typename TInterface = SimContextInterface>
class SimContextSimicsAdapter
    : public SimicsAdapter<sc_simcontext_interface_t> {
  public:
    SimContextSimicsAdapter()
        : SimicsAdapter<sc_simcontext_interface_t>(
            SC_SIMCONTEXT_INTERFACE, init_iface()) {
    }

  protected:
    static ::uint64 time_stamp(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->time_stamp();
    }
    static ::uint64 delta_count(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->delta_count();
    }
    static ::uint64 time_to_pending_activity(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->time_to_pending_activity();
    }
    static int status(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->status();
    }
    static attr_value_t events(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->events();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_simcontext_interface_t init_iface() {
        sc_simcontext_interface_t iface = {};
        iface.time_stamp = time_stamp;
        iface.delta_count = delta_count;
        iface.time_to_pending_activity = time_to_pending_activity;
        iface.status = status;
        iface.events = events;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
