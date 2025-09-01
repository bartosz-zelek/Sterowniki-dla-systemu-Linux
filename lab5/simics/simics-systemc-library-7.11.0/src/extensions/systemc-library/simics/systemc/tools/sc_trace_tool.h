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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_TRACE_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_TRACE_TOOL_H

#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/tools/sc_tool.h>

#include <tlm>

#include <vector>
#include <string>

namespace simics {
namespace systemc {
namespace tools {

class ScTraceTool : public ScTool {
  public:
    explicit ScTraceTool(simics::ConfObjectRef o)
        : ScTool(o), sc_timestamp_(false), process_internal_(false) {}
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
    virtual void nb_transport_fw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret);
    virtual void b_transport_pre(scla::ProxyInterface *proxy,
                                 tlm::tlm_generic_payload *trans,
                                 sc_core::sc_time *delay);
    virtual void b_transport_post(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay);
    virtual void get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                        tlm::tlm_generic_payload *trans,
                                        tlm::tlm_dmi *dmi_data);
    virtual void get_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                         tlm::tlm_generic_payload *trans,
                                         tlm::tlm_dmi *dmi_data,
                                         bool *ret);
    virtual void transport_dbg_pre(scla::ProxyInterface *proxy,
                                   tlm::tlm_generic_payload *trans);
    virtual void transport_dbg_post(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans,
                                    unsigned int *ret);

    virtual void nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay);
    virtual void nb_transport_bw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret);
    virtual void invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                               uint64 *start_range,
                                               uint64 *end_range);
    virtual void invalidate_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                                uint64 *start_range,
                                                uint64 *end_range);

    void set_sc_timestamp(const bool &val);
    bool sc_timestamp() const;
    void set_process_internal(const bool &val);
    bool process_internal() const;
    static void initialize(const std::string &module_name);

  protected:
    virtual std::string event_header();
    virtual std::string process_header();
    virtual std::string signal_header();
    virtual std::string signal_port_header();
    virtual std::string tlm_header();
    virtual std::string header(scla::ProxyInterface *proxy, bool is_request,
                               std::string type);
    bool sc_timestamp_;
    bool process_internal_;
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
