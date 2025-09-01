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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_TRANSACTION_TRACKER_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_TRANSACTION_TRACKER_TOOL_H

#include <simics/cc-api.h>
#include <simics/systemc/awareness/proxy_interface.h>
#include <simics/systemc/tools/sc_tool.h>
#include <simics/systemc/tools/sc_transaction_tracker_extension.h>

#include <systemc>
#include <tlm>

#include <deque>
#include <queue>
#include <string>

namespace simics {
namespace systemc {
namespace tools {

class ScTransactionTrackerTool : public ScTool {
  public:
    explicit ScTransactionTrackerTool(simics::ConfObjectRef o);

    virtual void nb_transport_fw_pre(scla::ProxyInterface *proxy_iface,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *time);

    virtual void nb_transport_fw_post(scla::ProxyInterface *proxy_iface,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *time,
                                      tlm::tlm_sync_enum *ret);

    virtual void b_transport_pre(scla::ProxyInterface *proxy_iface,
                                 tlm::tlm_generic_payload *trans,
                                 sc_core::sc_time *time);

    virtual void b_transport_post(scla::ProxyInterface *proxy_iface,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *time);

    virtual void transport_dbg_pre(scla::ProxyInterface *proxy_iface,
                                   tlm::tlm_generic_payload *trans);

    virtual void transport_dbg_post(scla::ProxyInterface *proxy_iface,
                                    tlm::tlm_generic_payload *trans,
                                    unsigned int *ret);

    virtual void nb_transport_bw_pre(scla::ProxyInterface *proxy_iface,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *time);

    virtual void nb_transport_bw_post(scla::ProxyInterface *proxy_iface,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *time,
                                      tlm::tlm_sync_enum *ret);
    static void initialize(const std::string &module_name);

  private:
    void allocate_extensions();
    TransactionTrackerExtension* find_free_extension();

    void handle_extension_pre(scla::ProxyInterface *proxy_iface,
                              tlm::tlm_generic_payload *trans);

    void handle_extension_post(scla::ProxyInterface *proxy_iface,
                               tlm::tlm_generic_payload *trans);

    typedef std::deque<TransactionTrackerExtension> extensions_t;
    extensions_t extensions_;

    typedef std::queue<TransactionTrackerExtension *> extensions_free_list_t;
    extensions_free_list_t extensions_free_list_;
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
