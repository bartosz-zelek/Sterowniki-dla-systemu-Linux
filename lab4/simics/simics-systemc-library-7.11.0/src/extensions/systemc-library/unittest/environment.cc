// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include "environment.h"

#include <simics/systemc/adapter.h>

#include <systemc>

std::map<std::string, const interface_t *> interfaces;

void EnvironmentBase::register_interface(std::string name,
                                     const interface_t *iface) {
    interfaces[name] = iface;
}

EnvironmentBase::EnvironmentBase() {
    // suppress the warnings that we know are safe during unittesting
    sc_core::sc_report_handler::set_actions(
        sc_core::SC_ID_NO_SC_START_ACTIVITY_, sc_core::SC_DO_NOTHING);
    sc_core::sc_report_handler::set_actions(
        sc_core::SC_ID_INSTANCE_EXISTS_, sc_core::SC_DO_NOTHING);
}

EnvironmentBase::~EnvironmentBase() {
    interfaces.clear();
}

std::map<std::string, const interface_t *> EnvironmentBase::interfaces;

const interface_t *SIM_c_get_interface(const conf_object_t *NOTNULL obj,
                                       const char *NOTNULL name) {
    if (EnvironmentBase::interfaces.find(name) !=
            EnvironmentBase::interfaces.end()) {
        return EnvironmentBase::interfaces[name];
    }

    return NULL;
}

Environment::Environment() {}

simics::systemc::iface::SimulationInterface &
simics::systemc::AdapterBase::simulation_from_conf_obj(
        conf_object_t *obj) {
    return *static_cast<simics::systemc::iface::SimulationInterface *>(
        obj->instance_data);
}
