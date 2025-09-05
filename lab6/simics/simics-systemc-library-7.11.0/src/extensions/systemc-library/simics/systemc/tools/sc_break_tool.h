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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_BREAK_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_BREAK_TOOL_H

#include <simics/systemc/tools/sc_trace_tool.h>
#include <string>

namespace simics {
namespace systemc {
namespace tools {

class ScBreakTool : public ScTraceTool {
  public:
    explicit ScBreakTool(simics::ConfObjectRef o) : ScTraceTool(o) {}
    virtual void triggered(scla::ProxyInterface *proxy,
                           const char *event_type,
                           const char *class_type,
                           void *object,
                           sc_core::sc_time *timestamp);
    virtual void process_state_change(scla::ProxyInterface *proxy,
                                      const char *event_type,
                                      const char *class_type,
                                      void *object,
                                      sc_core::sc_time *timestamp,
                                      bool internal);
    virtual void fired(scla::ProxyInterface *proxy);
    virtual void signal_port_value_update(scla::ProxyInterface *proxy,
                                          sc_core::sc_object *signal);
    virtual void nb_transport_fw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay);
    virtual void b_transport_pre(scla::ProxyInterface *proxy,
                                 tlm::tlm_generic_payload *trans,
                                 sc_core::sc_time *delay);
    virtual void get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                        tlm::tlm_generic_payload *trans,
                                        tlm::tlm_dmi *dmi_data);
    virtual void transport_dbg_pre(scla::ProxyInterface *proxy,
                                   tlm::tlm_generic_payload *trans);

    virtual void nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay);
    virtual void invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                               uint64 *start_range,
                                               uint64 *end_range);
    static void initialize(const std::string &module_name);

  protected:
    virtual std::string event_header();
    virtual std::string process_header();
    virtual std::string signal_header();
    virtual std::string signal_port_header();
    virtual std::string tlm_header();
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
