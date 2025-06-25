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

#include <simics/simulator/sim-get-class.h>
#include <simics/base/log.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/iface/instrumentation/provider_controller_interface.h>
#include <simics/systemc/iface/instrumentation/connection_simics_adapter.h>
#include <simics/systemc/iface/instrumentation/tool_connection_simics_adapter.h>
#include <simics/systemc/instrumentation/tool_connection.h>

#include <string>
#include <vector>

namespace scii = simics::systemc::iface::instrumentation;

namespace simics {
namespace systemc {
namespace instrumentation {

std::string ToolConnection::connection_class_name_;  // NOLINT(runtime/string)

ToolConnection::ToolConnection(ConfObjectRef o)
    : ConfObject(o), enabled_(true) {}

ToolConnection::~ToolConnection() {
    scii::ProviderControllerInterface *pc =
        dynamic_cast<scii::ProviderControllerInterface *>(
            static_cast<ConfObject *>(SIM_object_data(controller_)));
    if (pc)
        pc->remove(this);
}

void ToolConnection::set_functions(std::vector<std::string> *functions) {
    functions_ = *functions;
}

const std::vector<std::string> &ToolConnection::functions() const {
    return functions_;
}

bool ToolConnection::enabled() const {
    return enabled_;
}

void ToolConnection::set_tool(ConfObjectRef tool) {
    tool_ = tool;
}

ConfObjectRef ToolConnection::tool() const {
    return tool_;
}

void ToolConnection::set_controller(ConfObjectRef controller) {
    controller_ = controller;
    SimulationInterface *simulation =
        dynamic_cast<SimulationInterface *>(
            static_cast<ConfObject *>(SIM_object_data(controller)));
    if (simulation)
        setSimulation(simulation);
    else
        SIM_LOG_ERROR(obj(), 0,
                      "No SimulationInterface implemented for %s",
                      controller.name().c_str());
}

ConfObjectRef ToolConnection::controller() const {
    return controller_;
}

bool ToolConnection::enable() {
    enabled_ = true;
    return true;
}

bool ToolConnection::disable() {
    enabled_ = false;
    return true;
}

void ToolConnection::initialize(const std::string &module_name) {
    if (!connection_class_name_.empty())
        return;

    connection_class_name_ = module_name + "_tool_connection";

    auto cls = make_class<ToolConnection>(
        connection_class_name_,
        "couples a tool with a provider",
        "SystemC Tool Connection to couple a tool with a provider.",
        Sim_Class_Kind_Pseudo);

    static scii::ConnectionSimicsAdapter<ToolConnection> adapter;
    cls->add(adapter);

    static scii::ToolConnectionSimicsAdapter<ToolConnection>
        tool_connection_adapter;
    cls->add(tool_connection_adapter);
}

std::string ToolConnection::connection_class_name() {
    return connection_class_name_;
}

}  // namespace instrumentation
}  // namespace systemc
}  // namespace simics
