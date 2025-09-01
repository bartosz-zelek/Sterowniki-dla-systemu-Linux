// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_INITIATOR_SOCKET_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_INITIATOR_SOCKET_H

#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <simics/conf-object.h>

#include <systemc>

namespace unittest {
template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class MockTlmInitiatorSocket : public sc_core::sc_module,
    public tlm::tlm_bw_transport_if<TYPES> {
  public:
    SC_CTOR(MockTlmInitiatorSocket)
        : initiator_socket("initiator_socket"), obj_(&conf_),
          invalidate_direct_mem_ptr_count_(0) {
        initiator_socket.bind(*this);
    }
    tlm::tlm_sync_enum nb_transport_bw(typename TYPES::tlm_payload_type& trans,  // NOLINT
                                       typename TYPES::tlm_phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return tlm::TLM_COMPLETED;
    }
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        ++invalidate_direct_mem_ptr_count_;
    }
    tlm::tlm_initiator_socket<BUSWIDTH, TYPES, N, POL> initiator_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    int invalidate_direct_mem_ptr_count_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockSimpleInitiatorSocket : public sc_core::sc_module {
  public:
    SC_CTOR(MockSimpleInitiatorSocket)
        : initiator_socket("initiator_socket"), obj_(&conf_) {
    }
    tlm_utils::simple_initiator_socket<MockSimpleInitiatorSocket, BUSWIDTH,
                                       TYPES> initiator_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockSimpleInitiatorSocketTagged : public sc_core::sc_module {
  public:
    SC_CTOR(MockSimpleInitiatorSocketTagged)
        : initiator_socket("initiator_socket"), obj_(&conf_) {
    }
    tlm_utils::simple_initiator_socket_tagged<MockSimpleInitiatorSocketTagged,
                                              BUSWIDTH, TYPES> initiator_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types,
          unsigned int N = 0,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class MockMultiPassthroughInitiatorSocket : public sc_core::sc_module {
  public:
    SC_CTOR(MockMultiPassthroughInitiatorSocket)
        : initiator_socket("initiator_socket"), obj_(&conf_),
          invalidate_direct_mem_ptr_count_(0),
          nb_transport_bw_count_(0) {
        initiator_socket.register_invalidate_direct_mem_ptr(this,
            &MockMultiPassthroughInitiatorSocket::invalidate_direct_mem_ptr);
        initiator_socket.register_nb_transport_bw(this,
            &MockMultiPassthroughInitiatorSocket::nb_transport_bw);
    }
    void invalidate_direct_mem_ptr(int id, sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        ++invalidate_direct_mem_ptr_count_;
    }
    tlm::tlm_sync_enum nb_transport_bw(int id,
                                       typename TYPES::tlm_payload_type& trans,  // NOLINT
                                       typename TYPES::tlm_phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        ++nb_transport_bw_count_;
        return tlm::TLM_COMPLETED;
    }
    tlm_utils::multi_passthrough_initiator_socket<
        MockMultiPassthroughInitiatorSocket,
        BUSWIDTH, TYPES, N, POL> initiator_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    int invalidate_direct_mem_ptr_count_;
    int nb_transport_bw_count_;
};

}  // namespace unittest

#endif
