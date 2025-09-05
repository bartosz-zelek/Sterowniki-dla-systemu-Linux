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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_FW_TRANSPORT_IF_HANDLER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_FW_TRANSPORT_IF_HANDLER_H

#include <tlm>

namespace awareness {
namespace unittest {

template <typename IF_PROVIDER = sc_core::sc_port_base,
          typename TYPES = tlm::tlm_base_protocol_types,
          typename FW_IF = tlm::tlm_fw_transport_if<TYPES> >
class MockTlmFwTransportIfHandler : public FW_IF {
  public:
    explicit MockTlmFwTransportIfHandler(IF_PROVIDER *fw_if)
        : if_provider_(fw_if), fw_if_(NULL) {
        nb_transport_fw_count_ = 0;
        b_transport_count_ = 0;
        get_direct_mem_ptr_count_ = 0;
        transport_dbg_count_ = 0;
    }
    explicit MockTlmFwTransportIfHandler(FW_IF *fw_if)
        : if_provider_(NULL), fw_if_(fw_if) {
        nb_transport_fw_count_ = 0;
        b_transport_count_ = 0;
        get_direct_mem_ptr_count_ = 0;
        transport_dbg_count_ = 0;
    }

    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;
    tlm::tlm_sync_enum nb_transport_fw(transaction_type &trans,  // NOLINT
                                       phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        ++nb_transport_fw_count_;
        FW_IF *fw_if = fw_if_
            ? fw_if_ : dynamic_cast<FW_IF*>(if_provider_->get_interface());
        return fw_if->nb_transport_fw(trans, phase, t);
    }
    void b_transport(transaction_type &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        ++b_transport_count_;
        FW_IF *fw_if = fw_if_
            ? fw_if_ : dynamic_cast<FW_IF*>(if_provider_->get_interface());
        fw_if->b_transport(trans, t);
    }
    bool get_direct_mem_ptr(transaction_type &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  // NOLINT
        ++get_direct_mem_ptr_count_;
        FW_IF *fw_if = fw_if_
            ? fw_if_ : dynamic_cast<FW_IF*>(if_provider_->get_interface());
        return fw_if->get_direct_mem_ptr(trans, dmi_data);
    }
    unsigned int transport_dbg(transaction_type &trans) {  // NOLINT
        ++transport_dbg_count_;
        FW_IF *fw_if = fw_if_
            ? fw_if_ : dynamic_cast<FW_IF*>(if_provider_->get_interface());
        return fw_if->transport_dbg(trans);
    }
    static void reset() {
        nb_transport_fw_count_ = 0;
        b_transport_count_ = 0;
        get_direct_mem_ptr_count_ = 0;
        transport_dbg_count_ = 0;
    }
    IF_PROVIDER *if_provider_;
    FW_IF *fw_if_;
    static int nb_transport_fw_count_;
    static int b_transport_count_;
    static int get_direct_mem_ptr_count_;
    static int transport_dbg_count_;
};
template <typename IF_PROVIDER, typename TYPES, typename FW_IF>
int MockTlmFwTransportIfHandler<IF_PROVIDER,
                                TYPES, FW_IF>::nb_transport_fw_count_ = 0;
template <typename IF_PROVIDER, typename TYPES, typename FW_IF>
int MockTlmFwTransportIfHandler<IF_PROVIDER,
                                TYPES, FW_IF>::b_transport_count_ = 0;
template <typename IF_PROVIDER, typename TYPES, typename FW_IF>
int MockTlmFwTransportIfHandler<IF_PROVIDER,
                                TYPES, FW_IF>::get_direct_mem_ptr_count_ = 0;
template <typename IF_PROVIDER, typename TYPES, typename FW_IF>
int MockTlmFwTransportIfHandler<IF_PROVIDER,
                                TYPES, FW_IF>::transport_dbg_count_ = 0;


}  // namespace unittest
}  // namespace awareness

#endif  // SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_FW_TRANSPORT_IF_HANDLER_H
