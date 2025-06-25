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

#ifndef SIMICS_SYSTEMC_TRACE_PROCESS_ALL_DYNAMIC_H
#define SIMICS_SYSTEMC_TRACE_PROCESS_ALL_DYNAMIC_H

#include <simics/cc-api.h>
#include <simics/simulator/sim-get-class.h>
#include <simics/systemc/iface/instrumentation/provider_controller_simics_adapter.h>
#include <simics/systemc/iface/instrumentation/process_action_interface.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/instrumentation/tool_controller_process_action.h>
#include <simics/systemc/internal_processes.h>
#include <simics/systemc/simulation_interface_proxy.h>

namespace simics {
namespace systemc {

/** \internal */
class TraceProcessAllDynamic
    : public simics::ConfObject,
      public instrumentation::ToolControllerProcessAction,
      public EventCallbackInterface,
      public SimulationInterfaceProxy {
  public:
    explicit TraceProcessAllDynamic(simics::ConfObjectRef o)
        : ConfObject(o) {
    }
    void init(SimulationInterface *simulation) {
        setSimulation(simulation);
    }
    virtual void connection_list_updated(ConnectionListState state);
    void event_callback(const char *event_type,
                        const char *class_type,
                        void *object,
                        const sc_core::sc_time &ts) {
        sc_core::sc_process_b *process
             = static_cast<sc_core::sc_process_b*>(object);
        sc_core::sc_process_handle handle(process);
        bool internal = false;
        if (InternalProcesses.find(handle) != InternalProcesses.end())
            internal = true;

        DISPATCH_TOOL_CHAIN_THIS(iface::instrumentation::ProcessActionInterface,
                                 process_state_change,
                                 NULL,
                                 event_type, class_type, object,
                                 const_cast<sc_core::sc_time *>(&ts),
                                 internal);
    }
    static conf_class_t *initClass() {
        conf_class_t *cls = SIM_get_class("sc_method_process_all_dynamic");
        if (SIM_clear_exception() == SimExc_No_Exception) {
            return cls;
        }

        auto new_cls = make_class<TraceProcessAllDynamic>(
            "sc_method_process_all_dynamic",
            "All dynamic SystemC processes",
            "Helper class to attach SystemC Tools on dynamically created "
            "SystemC processes.",
            Sim_Class_Kind_Pseudo);

        static iface::instrumentation::ProviderControllerSimicsAdapter<
            TraceProcessAllDynamic> sc_provider_controller_adapter;
        SIM_register_interface(*new_cls.get(), SC_PROVIDER_CONTROLLER_INTERFACE,
                               sc_provider_controller_adapter.cstruct());
        return *new_cls.get();
    }
};

}  // namespace systemc
}  // namespace simics

#endif
