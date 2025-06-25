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

#ifndef SIMICS_SYSTEMC_IFACE_SC_PROCESS_PROFILER_CONTROL_SIMICS_ADAPTER_H
#define SIMICS_SYSTEMC_IFACE_SC_PROCESS_PROFILER_CONTROL_SIMICS_ADAPTER_H

#include <simics/systemc/iface/sc_process_profiler_control_interface.h>
#include <simics/systemc/iface/simics_adapter.h>

#include <systemc-interfaces.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace iface {

/** \internal */
template<typename TBase,
         typename TInterface = ScProcessProfilerControlInterface>
class ScProcessProfilerControlSimicsAdapter
    : public SimicsAdapter<sc_process_profiler_control_interface_t> {
  public:
    ScProcessProfilerControlSimicsAdapter()
        : SimicsAdapter<sc_process_profiler_control_interface_t>(
            SC_PROCESS_PROFILER_CONTROL_INTERFACE,
            init_iface()) {
    }

  protected:
    static bool is_enabled(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->is_enabled();
    }
    static void set_enabled(conf_object_t *obj, bool enable) {
        return adapter<TBase, TInterface>(obj)->set_enabled(enable);
    }
    static ::uint64 total_time(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->total_time();
    }
    static ::uint64 total_number_of_calls(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->total_number_of_calls();
    }
    static void clear_data(conf_object_t *obj) {
        return adapter<TBase, TInterface>(obj)->clear_data();
    }

  private:
    std::vector<std::string> description(conf_object_t *obj,
                                         DescriptionType type) {
        return descriptionBase<TBase, TInterface>(obj, type);
    }
    sc_process_profiler_control_interface_t init_iface() {
        sc_process_profiler_control_interface_t iface = {};
        iface.is_enabled = is_enabled;
        iface.set_enabled = set_enabled;
        iface.total_time = total_time;
        iface.total_number_of_calls = total_number_of_calls;
        iface.clear_data = clear_data;
        return iface;
    }
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
