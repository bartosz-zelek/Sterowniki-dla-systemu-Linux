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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_BW_TRANSPORT_IF_HANDLER_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_BW_TRANSPORT_IF_HANDLER_H

#include <tlm>

namespace awareness {
namespace unittest {

template <typename IF_PROVIDER = sc_core::sc_port_base,
          typename TYPES = tlm::tlm_base_protocol_types,
          typename BW_IF = tlm::tlm_bw_transport_if<TYPES> >
class MockTlmBwTransportIfHandler : public BW_IF {
  public:
    explicit MockTlmBwTransportIfHandler(IF_PROVIDER *bw_if)
        : if_provider_(bw_if), bw_if_(NULL) {
        invalidate_direct_mem_ptr_count_ = 0;
        nb_transport_bw_count_ = 0;
    }
    explicit MockTlmBwTransportIfHandler(BW_IF *bw_if)
        : if_provider_(NULL), bw_if_(bw_if) {
        invalidate_direct_mem_ptr_count_ = 0;
        nb_transport_bw_count_ = 0;
    }
    typedef typename TYPES::tlm_payload_type transaction_type;
    typedef typename TYPES::tlm_phase_type phase_type;
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        ++invalidate_direct_mem_ptr_count_;
        BW_IF *bw_if = bw_if_
            ? bw_if_ : dynamic_cast<BW_IF*>(if_provider_->get_interface());
        bw_if->invalidate_direct_mem_ptr(start_range, end_range);
    }
    tlm::tlm_sync_enum nb_transport_bw(transaction_type &trans,  // NOLINT
                                       phase_type &phase, sc_core::sc_time &t) {  // NOLINT
        ++nb_transport_bw_count_;
        BW_IF *bw_if = bw_if_
            ? bw_if_ : dynamic_cast<BW_IF*>(if_provider_->get_interface());
        return bw_if->nb_transport_bw(trans, phase, t);
    }
    static void reset() {
        invalidate_direct_mem_ptr_count_ = 0;
        nb_transport_bw_count_ = 0;
    }

    IF_PROVIDER *if_provider_;
    BW_IF *bw_if_;
    static int invalidate_direct_mem_ptr_count_;
    static int nb_transport_bw_count_;
};
template <typename IF_PROVIDER, typename TYPES, typename BW_IF>
int MockTlmBwTransportIfHandler<IF_PROVIDER, TYPES,
                                BW_IF>::invalidate_direct_mem_ptr_count_ = 0;
template <typename IF_PROVIDER, typename TYPES, typename BW_IF>
int MockTlmBwTransportIfHandler<IF_PROVIDER,
                                TYPES, BW_IF>::nb_transport_bw_count_ = 0;

}  // namespace unittest
}  // namespace awareness

#endif  // SYSTEMC_LIBRARY_UNITTEST_MOCK_AWARENESS_MOCK_TLM_BW_TRANSPORT_IF_HANDLER_H
