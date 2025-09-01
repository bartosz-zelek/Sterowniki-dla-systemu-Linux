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

#ifndef SIMICS_SYSTEMC_AWARENESS_TLM_BW_TRANSPORT_IF_HANDLER_H
#define SIMICS_SYSTEMC_AWARENESS_TLM_BW_TRANSPORT_IF_HANDLER_H



#include <simics/systemc/awareness/proxy.h>
#include <simics/systemc/awareness/tlm_base_handler.h>
#include <simics/systemc/awareness/tlm_handler_interface.h>
#include <simics/systemc/awareness/tlm_multi_handler_registry.h>
#include <simics/systemc/iface/instrumentation/tlm_bw_action_interface.h>

#include <tlm>
#include <unordered_map>

namespace simics {
namespace systemc {
namespace awareness {

template <typename TYPES = tlm::tlm_base_protocol_types>
class TlmBwTransportIfHandlerBase
    : public TlmBaseHandler,
      public tlm::tlm_bw_transport_if<TYPES> {
  public:
    typedef tlm::tlm_bw_transport_if<TYPES> BW_IF;
    TlmBwTransportIfHandlerBase()
        : is_active_(false) {}
    virtual void enable() {
        if (is_active_)
            return;

        if (!iface())
            return;

        is_active_ = true;
        bw_if()->invalidate_direct_mem_ptr(0,
            static_cast<sc_dt::uint64>(-1));
    }
    virtual void disable() {
        is_active_ = false;
    }

    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmBwActionInterface,
                            invalidate_direct_mem_ptr_pre,
                            proxy(), &start_range, &end_range);
        BW_IF* bw = bw_if();
        // MPInitiatorSocket does not provide an iface on its export.
        // When a tool is inserted to the connected target socket,
        // invalidate_direct_mem_ptr is invoked via the enable method shown
        // above.
        if (bw)
            bw->invalidate_direct_mem_ptr(start_range, end_range);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmBwActionInterface,
                            invalidate_direct_mem_ptr_post,
                            proxy(), &start_range, &end_range);
    }
    tlm::tlm_sync_enum nb_transport_bw(transaction_type &trans,  // NOLINT
                                       phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        // suppress DMI hint when there is an instrumentation tool
        if (controller() && controller()->get_connections_state() !=
            instrumentation::ToolController::EMPTY)
            (&trans)->set_dmi_allowed(false);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmBwActionInterface,
                            nb_transport_bw_pre,
                            proxy(), &trans, &phase, &t);

        tlm::tlm_sync_enum ret = tlm::TLM_COMPLETED;
        BW_IF* bw = bw_if();
        // MPInitiatorSocket does not provide an iface on its export.
        if (bw)
            ret = bw->nb_transport_bw(trans, phase, t);

        DISPATCH_TOOL_CHAIN(controller(),
                            iface::instrumentation::TlmBwActionInterface,
                            nb_transport_bw_post,
                            proxy(), &trans, &phase, &t, &ret);
        return ret;
    }

    BW_IF *bw_if() {
        sc_core::sc_interface* sc_iface = iface();
        if (bw_if_map_.find(sc_iface) == bw_if_map_.end()) {
            // A micro performance benchmark shows dynamic_cast is a hotspot,
            // thus the result of it is cached here
            bw_if_map_.emplace(sc_iface, dynamic_cast<BW_IF*>(sc_iface));
        }
        return bw_if_map_[sc_iface];
    }

  private:
    bool is_active_;
    std::unordered_map<sc_core::sc_interface*, BW_IF*> bw_if_map_;
};

template <typename IF_PROVIDER = sc_core::sc_port_base,
          typename TYPES = tlm::tlm_base_protocol_types>
class TlmBwTransportIfHandler : public tlm::tlm_bw_transport_if<TYPES>,
                                public TlmMultiHandlerInterface {
  public:
    typedef tlm::tlm_bw_transport_if<TYPES> BW_IF;
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;

    explicit TlmBwTransportIfHandler(IF_PROVIDER *bw_if) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIfaceProvider<IF_PROVIDER, TYPES,
                                                   BW_IF>(bw_if));
    }
    explicit TlmBwTransportIfHandler(BW_IF *bw_if) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIface(bw_if));
    }
    explicit TlmBwTransportIfHandler(
            tlm_utils::callback_binder_bw<TYPES> *binder_bw)
        : registry_(binder_bw, this) {
        first_.set_tlm_iface(new TlmIface(&second_));
        second_.set_tlm_iface(new TlmIface(binder_bw));
    }

    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        first_.invalidate_direct_mem_ptr(start_range, end_range);
    }
    tlm::tlm_sync_enum nb_transport_bw(transaction_type &trans,  // NOLINT
                                       phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return first_.nb_transport_bw(trans, phase, t);
    }

    virtual TlmHandlerInterface *firstHandler() {
        return &first_;
    }
    virtual TlmHandlerInterface *secondHandler() {
        return &second_;
    }

  private:
    TlmBwTransportIfHandlerBase<TYPES> first_;
    TlmBwTransportIfHandlerBase<TYPES> second_;
    TlmMultiHandlerRegistry registry_;
};

}  // namespace awareness
}  // namespace systemc
}  // namespace simics

#endif
