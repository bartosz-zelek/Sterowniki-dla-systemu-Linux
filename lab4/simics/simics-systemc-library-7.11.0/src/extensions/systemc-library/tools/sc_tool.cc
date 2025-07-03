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

#include <simics/simulator-api.h>
#include <simics/systemc/instrumentation/tool_connection.h>
#include <simics/systemc/instrumentation/tool_connection_interface.h>
#include <simics/systemc/instrumentation/tool_controller.h>
#include <simics/systemc/internals.h>

#include <simics/systemc/tools/sc_tool.h>

#include <string>
#include <vector>

using simics::systemc::instrumentation::ToolConnection;
using simics::systemc::instrumentation::ToolConnectionInterface;
using simics::systemc::instrumentation::ToolController;

namespace simics {
namespace systemc {
namespace tools {

conf_object_t *ScTool::connect(conf_object_t *controller, attr_value_t args) {
    std::string cls_name = ToolConnection::connection_class_name();
    conf_class_t *cls = SIM_get_class(cls_name.c_str());
    if (SIM_clear_exception() != SimExc_No_Exception) {
        FATAL_ERROR("ScTool::connect: %s", SIM_last_error());
    }

    attr_value_t attrs = SIM_make_attr_list(0);
    std::string conn_name
            = obj().name() + "." + VT_generate_object_name();
    conf_object_t *conn = SIM_create_object(cls, conn_name.c_str(), attrs);
    // Use (void) to avoid coverity warning "unchecked_return_value"
    (void)SIM_clear_exception();
    SIM_attr_free(&attrs);
    if (!conn)
        return NULL;

    ToolConnectionInterface *tci = dynamic_cast<ToolConnectionInterface *>(
        static_cast<ConfObject *>(SIM_object_data(conn)));
    if (!tci)
        return NULL;

    ToolController *tc = dynamic_cast<ToolController *>(
        static_cast<ConfObject *>(SIM_object_data(controller)));
    if (!tc)
        return NULL;

    std::vector<std::string> functions;
    unsigned len = SIM_attr_list_size(args);
    for (unsigned n = 0; n < len; ++n)
        functions.push_back(SIM_attr_string(SIM_attr_list_item(args, n)));

    tci->set_functions(&functions);
    tci->set_tool(obj());
    tci->set_controller(controller);
    tc->insert(tci, -1);
    return conn;
}

void ScTool::disconnect(conf_object_t *conn) {
     SIM_delete_object(conn);
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
