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

#ifndef SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONNECTION_H
#define SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONNECTION_H

#include <simics/cc-api.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <simics/systemc/iface/instrumentation/connection_interface.h>
#include <simics/systemc/simulation_interface_proxy.h>

#include <map>
#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace instrumentation {

/** \internal */
class ToolConnection : public ConfObject,
                       public ToolConnectionInterface,
                       public iface::instrumentation::ConnectionInterface,
                       public SimulationInterfaceProxy {
  public:
    explicit ToolConnection(ConfObjectRef o);
    virtual ~ToolConnection();
    virtual void set_functions(std::vector<std::string> *functions);
    virtual const std::vector<std::string> &functions() const;
    virtual bool enabled() const;
    virtual void set_tool(ConfObjectRef tool);
    virtual ConfObjectRef tool() const;
    virtual void set_controller(ConfObjectRef controller);
    virtual ConfObjectRef controller() const;
    virtual bool enable();
    virtual bool disable();
    static void initialize(const std::string &module_name);
    static std::string connection_class_name();

  private:
    ConfObjectRef tool_;
    ConfObjectRef controller_;
    std::map<std::string, const interface_t *> interfaces_;
    std::vector<std::string> functions_;
    bool enabled_;
    static std::string connection_class_name_;
};

}  // namespace instrumentation
}  // namespace systemc
}  // namespace simics

#endif
