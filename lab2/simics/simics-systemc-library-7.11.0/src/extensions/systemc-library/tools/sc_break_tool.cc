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

#include <simics/systemc/internals.h>
#include <simics/systemc/tools/sc_break_tool.h>

#include <string>

namespace simics {
namespace systemc {
namespace tools {

void ScBreakTool::triggered(scla::ProxyInterface *proxy,
                            const char *event_type,
                            const char *class_type,
                            void *object,
                            sc_core::sc_time *timestamp) {
    proxy->breakSimulation();
    ScTraceTool::triggered(proxy, event_type, class_type, object, timestamp);
}

void ScBreakTool::process_state_change(scla::ProxyInterface *proxy,
                                       const char *event_type,
                                       const char *class_type,
                                       void *object,
                                       sc_core::sc_time *timestamp,
                                       bool internal) {
    // Don't break on internal spawned processes.
    if (internal && !process_internal_)
        return;

    proxy->breakSimulation();
    ScTraceTool::process_state_change(proxy, event_type, class_type, object,
                                      timestamp, internal);
}

void ScBreakTool::fired(scla::ProxyInterface *proxy) {
    proxy->breakSimulation();
    ScTraceTool::fired(proxy);
}

void ScBreakTool::signal_port_value_update(scla::ProxyInterface *proxy,
                                           sc_core::sc_object *signal) {
    proxy->breakSimulation();
    ScTraceTool::signal_port_value_update(proxy, signal);
}

void ScBreakTool::nb_transport_fw_pre(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay) {
    proxy->breakSimulation();
    ScTraceTool::nb_transport_fw_pre(proxy, trans, phase, delay);
}

void ScBreakTool::b_transport_pre(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay) {
    proxy->breakSimulation();
    ScTraceTool::b_transport_pre(proxy, trans, delay);
}

void ScBreakTool::get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                         tlm::tlm_generic_payload *trans,
                                         tlm::tlm_dmi *dmi_data) {
    proxy->breakSimulation();
    ScTraceTool::get_direct_mem_ptr_pre(proxy, trans, dmi_data);
}

void ScBreakTool::transport_dbg_pre(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans) {
    proxy->breakSimulation();
    ScTraceTool::transport_dbg_pre(proxy, trans);
}

void ScBreakTool::nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay) {
    proxy->breakSimulation();
    ScTraceTool::nb_transport_bw_pre(proxy, trans, phase, delay);
}

void ScBreakTool::invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                                uint64 *start_range,
                                                uint64 *end_range) {
    proxy->breakSimulation();
    ScTraceTool::invalidate_direct_mem_ptr_pre(proxy, start_range, end_range);
}

std::string ScBreakTool::event_header() {
    return " break-event] ";
}

std::string ScBreakTool::process_header() {
    return " break-process] ";
}

std::string ScBreakTool::signal_header() {
    return " break-signal] ";
}

std::string ScBreakTool::signal_port_header() {
    return " break-signal-port] ";
}

std::string ScBreakTool::tlm_header() {
    return " break-";
}

void ScBreakTool::initialize(const std::string &module_name) {
    auto cls = ScTool::register_class<ScBreakTool>(
            module_name + "_sc_break_tool",
            "tests break on SC objects",
            "SystemC Tool for break on SystemC objects");
    cls->add(Attribute("internal", "b", "",
                       ATTR_GETTER(ScBreakTool, process_internal),
                       ATTR_SETTER(ScBreakTool, set_process_internal)));
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics
