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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_TOOL_H

#include <simics/cc-api.h>
#include <simics/simulator-iface/instrumentation-tool.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/iface/instrumentation/event_action_interface.h>
#include <simics/systemc/iface/instrumentation/process_action_interface.h>
#include <simics/systemc/iface/instrumentation/signal_action_interface.h>
#include <simics/systemc/iface/instrumentation/signal_port_action_interface.h>
#include <simics/systemc/iface/instrumentation/tlm_fw_action_interface.h>
#include <simics/systemc/iface/instrumentation/tlm_bw_action_interface.h>
#include <simics/systemc/iface/instrumentation/tool_interface.h>
#include <simics/systemc/iface/instrumentation/tool_simics_adapter.h>

#include <memory>
#include <set>
#include <vector>
#include <string>

namespace scl = simics::systemc;
namespace scla = scl::awareness;
namespace scli = scl::iface::instrumentation;

namespace simics {
namespace systemc {
namespace tools {

class ScTool : public simics::ConfObject,
               public scli::EventActionInterface,
               public scli::ProcessActionInterface,
               public scli::SignalActionInterface,
               public scli::SignalPortActionInterface,
               public scli::TlmFwActionInterface,
               public scli::TlmBwActionInterface,
               public scli::ToolInterface {
  public:
    explicit ScTool(simics::ConfObjectRef o)
        : ConfObject(o) {}
    virtual conf_object_t *connect(conf_object_t *controller,
                                   attr_value_t args);
    virtual void disconnect(conf_object_t *conn);

    // Registers a simics configuration class with all action interfaces.
    template <class T>
    static simics::ConfClassPtr register_class(
            std::string name, std::string long_name, std::string documentation);

  protected:
    // scli::EventActionInterface
    virtual void triggered(scla::ProxyInterface *proxy,
                           const char *event_type,
                           const char *class_type,
                           void *object,
                           sc_core::sc_time *timestamp) {}

    // scli::ProcessActionInterface
    virtual void process_state_change(scla::ProxyInterface *proxy,
                                      const char *event_type,
                                      const char *class_type,
                                      void *object,
                                      sc_core::sc_time *timestamp,
                                      bool internal) {}

    // scli::SignalActionInterface
    virtual void fired(scla::ProxyInterface *proxy) {}

    // scli::SignalPortActionInterface
    virtual void signal_port_value_update(scla::ProxyInterface *proxy,
                                          sc_core::sc_object *signal) {}

    // scli::TlmFwActionInterface
    virtual void nb_transport_fw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay) {}
    virtual void nb_transport_fw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret) {}
    virtual void b_transport_pre(scla::ProxyInterface *proxy,
                                 tlm::tlm_generic_payload *trans,
                                 sc_core::sc_time *delay) {}
    virtual void b_transport_post(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay) {}
    virtual void get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                        tlm::tlm_generic_payload *trans,
                                        tlm::tlm_dmi *dmi_data) {}
    virtual void get_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                         tlm::tlm_generic_payload *trans,
                                         tlm::tlm_dmi *dmi_data,
                                         bool *ret) {}
    virtual void transport_dbg_pre(scla::ProxyInterface *proxy,
                                   tlm::tlm_generic_payload *trans) {}
    virtual void transport_dbg_post(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans,
                                    unsigned int *ret) {}

    // scli::TlmBwActionInterface
    virtual void nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay) {}
    virtual void nb_transport_bw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret) {}
    virtual void invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                               uint64 *start_range,
                                               uint64 *end_range) {}
    virtual void invalidate_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                                uint64 *start_range,
                                                uint64 *end_range) {}
};

template <class T>
simics::ConfClassPtr ScTool::register_class(std::string name,
                                            std::string long_name,
                                            std::string documentation) {
    auto cls = simics::make_class<T>(name, long_name,
                                     documentation,
                                     Sim_Class_Kind_Pseudo);
    cls->add(scl::iface::createAdapter<scli::ToolSimicsAdapter<T>>());
    return cls;
}

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
