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

#include <simics/systemc/tools/sc_tool.h>
#include <simics/systemc/tools/sc_protocol_checker_tool.h>

#include <string>

namespace simics {
namespace systemc {
namespace tools {

void ScProtocolCheckerTool::initialize(const std::string &module_name) {
    ScTool::register_class<ScProtocolCheckerTool>(
        module_name + "_sc_protocol_checker_tool",
        "validates TLM transactions",
        "SystemC Tool for validation of TLM transactions");
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
