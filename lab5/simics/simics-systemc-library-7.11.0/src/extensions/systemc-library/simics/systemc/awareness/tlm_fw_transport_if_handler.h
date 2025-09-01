// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_FW_TRANSPORT_IF_HANDLER_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_FW_TRANSPORT_IF_HANDLER_H

#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/tlm_base_handler.h>
#include <simics/systemc/awareness/tlm_handler_interface.h>
#include <simics/systemc/iface/instrumentation/tlm_fw_action_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>

#include <tlm>
#include <unordered_map>

namespace simics {
namespace systemc {
namespace awareness {

template <typename TYPES = tlm::tlm_base_protocol_types>
class TlmFwTransportIfHandlerBase
    : public TlmBaseHandler, public tlm::tlm_fw_transport_if<TYPES> {
  public:
    typedef tlm::tlm_fw_transport_if<TYPES> FW_IF;
    TlmFwTransportIfHandlerBase() {}

    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    tlm::tlm_sync_enum nb_transport_fw(transaction_type &trans,  // NOLINT
                                       phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            nb_transport_fw_pre, proxy(), &trans, &phase, &t);

        tlm::tlm_sync_enum ret = fw_if()->nb_transport_fw(trans, phase, t);

        // suppress DMI hint when there is an instrumentation tool
        if (controller() && controller()->get_connections_state() !=
            instrumentation::ToolController::EMPTY)
            (&trans)->set_dmi_allowed(false);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            nb_transport_fw_post,
                            proxy(), &trans, &phase, &t, &ret);
        return ret;
    }
    void b_transport(transaction_type &trans,  // NOLINT
                     sc_core::sc_time &local_time_offset) {  //NOLINT
        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            b_transport_pre, proxy(), &trans,
                            &local_time_offset);
        fw_if()->b_transport(trans, local_time_offset);

        // suppress DMI hint when there is an instrumentation tool
        if (controller() && controller()->get_connections_state() !=
            instrumentation::ToolController::EMPTY)
            (&trans)->set_dmi_allowed(false);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            b_transport_post, proxy(), &trans,
                            &local_time_offset);
    }
    bool get_direct_mem_ptr(transaction_type &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  // NOLINT
        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            get_direct_mem_ptr_pre, proxy(), &trans, &dmi_data);

        bool ret = fw_if()->get_direct_mem_ptr(trans, dmi_data);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            get_direct_mem_ptr_post,
                            proxy(), &trans, &dmi_data, &ret);

        // suppress DMI when there is an instrumentation tool
        if (controller() && controller()->get_connections_state() !=
            instrumentation::ToolController::EMPTY) {
                return false;
        }
        return ret;
    }
    unsigned int transport_dbg(transaction_type &trans) {  // NOLINT
        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            transport_dbg_pre, proxy(), &trans);

        unsigned int ret = fw_if()->transport_dbg(trans);

        // suppress DMI hint when there is an instrumentation tool
        if (controller() && controller()->get_connections_state() !=
            instrumentation::ToolController::EMPTY)
            (&trans)->set_dmi_allowed(false);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmFwActionInterface,
                            transport_dbg_post, proxy(), &trans, &ret);
        return ret;
    }

    FW_IF *fw_if() {
        sc_core::sc_interface* sc_iface = iface();
        if (fw_if_map_.find(sc_iface) == fw_if_map_.end()) {
            // A micro performance benchmark shows dynamic_cast is a hotspot,
            // thus the result of it is cached here
            fw_if_map_.emplace(sc_iface, dynamic_cast<FW_IF*>(sc_iface));
        }
        return fw_if_map_[sc_iface];
    }

  private:
    std::unordered_map<sc_core::sc_interface*, FW_IF*> fw_if_map_;
};

template <typename IF_PROVIDER = sc_core::sc_port_base,
          typename TYPES = tlm::tlm_base_protocol_types>
class TlmFwTransportIfHandler: public tlm::tlm_fw_transport_if<TYPES>,
                               public TlmMultiHandlerInterface {
  public:
    typedef tlm::tlm_fw_transport_if<TYPES> FW_IF;
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    explicit TlmFwTransportIfHandler(IF_PROVIDER *fw_if) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIfaceProvider<IF_PROVIDER,
                                                   TYPES, FW_IF>(fw_if));
    }
    explicit TlmFwTransportIfHandler(FW_IF *fw_if) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIface(fw_if));
    }
    explicit TlmFwTransportIfHandler(
            tlm_utils::callback_binder_fw<TYPES> *binder_fw)
        : registry_(binder_fw, this) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIface(binder_fw));
    }

    tlm::tlm_sync_enum nb_transport_fw(transaction_type &trans,  // NOLINT
                                       phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return first_.nb_transport_fw(trans, phase, t);
    }
    void b_transport(transaction_type &trans,  // NOLINT
                     sc_core::sc_time &local_time_offset) {  //NOLINT
        first_.b_transport(trans, local_time_offset);
    }

    bool get_direct_mem_ptr(transaction_type &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  //NOLINT
        return first_.get_direct_mem_ptr(trans, dmi_data);
    }
    unsigned int transport_dbg(transaction_type &trans) {  // NOLINT
        return first_.transport_dbg(trans);
    }
    virtual TlmHandlerInterface *firstHandler() {
        return &first_;
    }
    virtual TlmHandlerInterface *secondHandler() {
        return &second_;
    }

  private:
    TlmFwTransportIfHandlerBase<TYPES> first_;
    TlmFwTransportIfHandlerBase<TYPES> second_;
    TlmMultiHandlerRegistry registry_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
