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

#ifndef SIMICS_SYSTEMC_TOOLS_SC_PROTOCOL_CHECKER_TOOL_H
#define SIMICS_SYSTEMC_TOOLS_SC_PROTOCOL_CHECKER_TOOL_H

#include <simics/systemc/awareness/tlm2_base_protocol_checker.h>
#include <simics/systemc/context.h>
#include <simics/systemc/tools/sc_tool.h>

#include <map>
#include <string>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace tools {

class ScProtocolCheckerTool : public ScTool {
  public:
    explicit ScProtocolCheckerTool(simics::ConfObjectRef o) : ScTool(o) {}
    virtual ~ScProtocolCheckerTool() {
        std::map<scla::ProxyInterface *,
                 tlm_utils::tlm2_base_protocol_checker *>::iterator i;
        for (i = checkers_.begin(); i != checkers_.end(); ++i)
            delete i->second;
    }

    virtual void nb_transport_fw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay) {
        simics::systemc::Context context(proxy->simulation());
        start_phase_[std::make_pair(proxy, trans)] = *phase;
        checker(proxy).nb_transport_fw_pre_checks(*trans, *phase, *delay);
    }
    virtual void nb_transport_fw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).nb_transport_fw_post_checks(*trans,
            start_phase_[std::make_pair(proxy, trans)], *phase, *delay, *ret);
    }
    virtual void b_transport_pre(scla::ProxyInterface *proxy,
                                 tlm::tlm_generic_payload *trans,
                                 sc_core::sc_time *delay) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).b_transport_pre_checks(*trans, *delay);
    }

    virtual void b_transport_post(scla::ProxyInterface *proxy,
                                  tlm::tlm_generic_payload *trans,
                                  sc_core::sc_time *delay) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).b_transport_post_checks(*trans, *delay);
    }
    virtual void get_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                        tlm::tlm_generic_payload *trans,
                                        tlm::tlm_dmi *dmi_data) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).get_direct_mem_ptr_pre_checks(*trans, *dmi_data);
    }
    virtual void get_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                         tlm::tlm_generic_payload *trans,
                                         tlm::tlm_dmi *dmi_data,
                                         bool *ret) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).get_direct_mem_ptr_post_checks(*trans, *dmi_data);
    }
    virtual void transport_dbg_pre(scla::ProxyInterface *proxy,
                                   tlm::tlm_generic_payload *trans) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).transport_dbg_pre_checks(*trans);
    }

    virtual void transport_dbg_post(scla::ProxyInterface *proxy,
                                    tlm::tlm_generic_payload *trans,
                                    unsigned int *ret) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).transport_dbg_post_checks(*trans, *ret);
    }
    virtual void nb_transport_bw_pre(scla::ProxyInterface *proxy,
                                     tlm::tlm_generic_payload *trans,
                                     tlm::tlm_phase *phase,
                                     sc_core::sc_time *delay) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).nb_transport_bw_pre_checks(*trans, *phase, *delay);
    }
    virtual void nb_transport_bw_post(scla::ProxyInterface *proxy,
                                      tlm::tlm_generic_payload *trans,
                                      tlm::tlm_phase *phase,
                                      sc_core::sc_time *delay,
                                      tlm::tlm_sync_enum *ret) {
        simics::systemc::Context context(proxy->simulation());
        checker(proxy).nb_transport_bw_post_checks(*trans, *phase, *delay,
                                                   *ret);
    }
    virtual void invalidate_direct_mem_ptr_pre(scla::ProxyInterface *proxy,
                                               sc_dt::uint64 *start_range,
                                               sc_dt::uint64 *end_range) {
    }
    virtual void invalidate_direct_mem_ptr_post(scla::ProxyInterface *proxy,
                                                sc_dt::uint64 *start_range,
                                                sc_dt::uint64 *end_range) {
    }
    tlm_utils::tlm2_base_protocol_checker &checker(
            scla::ProxyInterface *proxy) {
        if (checkers_.find(proxy) == checkers_.end())
            checkers_.insert(std::make_pair(proxy,
                new tlm_utils::tlm2_base_protocol_checker(proxy)));

        return *checkers_.find(proxy)->second;
    }
    static void initialize(const std::string &module_name);

  private:
    std::map<scla::ProxyInterface *,
             tlm_utils::tlm2_base_protocol_checker *> checkers_;
    std::map<std::pair<scla::ProxyInterface *, tlm::tlm_generic_payload *>,
             tlm::tlm_phase> start_phase_;
};

}  // namespace tools
}  // namespace systemc
}  // namespace simics

#endif
