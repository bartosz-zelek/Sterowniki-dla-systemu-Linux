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

#include <simics/simulator/python.h>

#include <simics/systemc/instrumentation/tool_connection.h>
#include <simics/systemc/tools/systemc_tools.h>
#include <simics/systemc/tools/sc_break_tool.h>
#include <simics/systemc/tools/sc_protocol_checker_tool.h>
#include <simics/systemc/tools/sc_trace_tool.h>
#include <simics/systemc/tools/sc_transaction_tracker_tool.h>
#include <simics/systemc/tools/sc_vcd_trace_tool.h>

#include <algorithm>
#include <string>

namespace simics {
namespace systemc {
namespace tools {

void initScTools(const char *module_name) {
    static std::string only_once;
    bool early_exit = false;
    if (only_once.empty())
        only_once = module_name;
    else
        early_exit = true;

    FATAL_ERROR_IF(only_once != module_name,
                   "initScTools was previously called for module %s .",
                   only_once.c_str());

    if (early_exit)
        return;

    std::string underscoreName = module_name;
    std::replace(underscoreName.begin(), underscoreName.end(), '-', '_');

    ScBreakTool::initialize(underscoreName);
    ScProtocolCheckerTool::initialize(underscoreName);
    ScTraceTool::initialize(underscoreName);
    ScTransactionTrackerTool::initialize(underscoreName);
    ScVcdTraceTool::initialize(underscoreName);
    instrumentation::ToolConnection::initialize(underscoreName);
    attr_value_t args = SIM_make_attr_list(1,
                                           SIM_make_attr_string(module_name));
    VT_call_python_module_function("simmod.systemc_tools.simics_start",
                                   "sc_tool_make_tool_commands", &args);
    // Use (void) to avoid coverity warning "unchecked_return_value"
    (void)SIM_clear_exception();
    SIM_attr_free(&args);
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
