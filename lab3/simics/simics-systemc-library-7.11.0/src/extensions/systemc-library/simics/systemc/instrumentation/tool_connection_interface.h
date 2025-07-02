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

#ifndef SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONNECTION_INTERFACE_H
#define SIMICS_SYSTEMC_INSTRUMENTATION_TOOL_CONNECTION_INTERFACE_H

#include <simics/device-api.h>

#include <string>
#include <vector>

namespace simics {
namespace systemc {
namespace instrumentation {

/** \internal */
class ToolConnectionInterface {
  public:
    virtual ~ToolConnectionInterface() {}
    virtual void set_functions(std::vector<std::string> *functions) = 0;
    virtual const std::vector<std::string> &functions() const = 0;
    virtual bool enabled() const = 0;
    virtual void set_tool(ConfObjectRef tool) = 0;
    virtual ConfObjectRef tool() const = 0;
    virtual void set_controller(ConfObjectRef controller) = 0;
    virtual ConfObjectRef controller() const = 0;
    template<class TInterface>
    TInterface *get_interface() {
        // Check if ToolConnection implemented the interface
        if (!tool())
            return dynamic_cast<TInterface *>(this);

        simics::ConfObject *obj = static_cast<simics::ConfObject *>(
                SIM_object_data(tool()));
        return dynamic_cast<TInterface *>(obj);
    }
};

}  // namespace instrumentation
}  // namespace systemc
}  // namespace simics

#endif
