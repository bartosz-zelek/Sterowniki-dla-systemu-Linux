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

#ifndef SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TARGET_SOCKET_H
#define SYSTEMC_LIBRARY_UNITTEST_MOCK_MOCK_TARGET_SOCKET_H

#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/passthrough_target_socket.h>
#include <tlm_utils/simple_target_socket.h>


#include <systemc>

namespace unittest {

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class MockTlmTargetSocket : public sc_core::sc_module,
    public tlm::tlm_fw_transport_if<TYPES> {
  public:
    SC_CTOR(MockTlmTargetSocket)
        : target_socket("target_socket"), obj_(&conf_), b_transport_count_(0) {
        target_socket.bind(*this);
    }

    void b_transport(typename TYPES::tlm_payload_type &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        ++b_transport_count_;
    }
    tlm::tlm_sync_enum nb_transport_fw(typename TYPES::tlm_payload_type &trans,  // NOLINT
                                       typename TYPES::tlm_phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return tlm::TLM_COMPLETED;
    }
    bool get_direct_mem_ptr(typename TYPES::tlm_payload_type &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  // NOLINT
        return true;
    }
    unsigned int transport_dbg(typename TYPES::tlm_payload_type &trans) {  // NOLINT
        return 0;
    }

    tlm::tlm_target_socket<BUSWIDTH, TYPES, N, POL> target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    int b_transport_count_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockSimpleTargetSocket : public sc_core::sc_module {
  public:
    SC_CTOR(MockSimpleTargetSocket)
        : target_socket("target_socket"), obj_(&conf_) {
    }
    tlm_utils::simple_target_socket<MockSimpleTargetSocket, BUSWIDTH, TYPES>
        target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockSimpleTargetSocketTagged : public sc_core::sc_module {
  public:
    SC_CTOR(MockSimpleTargetSocketTagged)
        : target_socket("target_socket"), obj_(&conf_) {
    }
    tlm_utils::simple_target_socket_tagged<MockSimpleTargetSocketTagged,
                                           BUSWIDTH, TYPES> target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockPassthroughTargetSocket : public sc_core::sc_module {
  public:
    SC_CTOR(MockPassthroughTargetSocket)
        : target_socket("target_socket"), obj_(&conf_) {
    }
    tlm_utils::passthrough_target_socket<MockPassthroughTargetSocket,
                                         BUSWIDTH, TYPES> target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types>
class MockPassthroughTargetSocketTagged : public sc_core::sc_module {
  public:
    SC_CTOR(MockPassthroughTargetSocketTagged)
        : target_socket("target_socket"), obj_(&conf_) {
    }
    tlm_utils::passthrough_target_socket_tagged<
        MockPassthroughTargetSocketTagged, BUSWIDTH, TYPES> target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types,
          unsigned int N = 0,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class MockMultiPassthroughTargetSocket : public sc_core::sc_module {
  public:
    SC_CTOR(MockMultiPassthroughTargetSocket)
        : target_socket("target_socket"), obj_(&conf_), b_transport_count_(0) {
        target_socket.register_nb_transport_fw(this,
            &MockMultiPassthroughTargetSocket::nb_transport_fw);
        target_socket.register_b_transport(this,
            &MockMultiPassthroughTargetSocket::b_transport);
        target_socket.register_get_direct_mem_ptr(this,
            &MockMultiPassthroughTargetSocket::get_direct_mem_ptr);
        target_socket.register_transport_dbg(this,
            &MockMultiPassthroughTargetSocket::transport_dbg);
    }
    void b_transport(int id, typename TYPES::tlm_payload_type &trans,  // NOLINT
                     sc_core::sc_time &t) {  // NOLINT
        ++b_transport_count_;
    }
    tlm::tlm_sync_enum nb_transport_fw(int id,
                                       typename TYPES::tlm_payload_type &trans,  // NOLINT
                                       typename TYPES::tlm_phase_type &phase,  // NOLINT
                                       sc_core::sc_time &t) {  // NOLINT
        return tlm::TLM_COMPLETED;
    }
    bool get_direct_mem_ptr(int id, typename TYPES::tlm_payload_type &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  // NOLINT
        return true;
    }
    unsigned int transport_dbg(int id, typename TYPES::tlm_payload_type &trans) {  // NOLINT
        return 0;
    }
    tlm_utils::multi_passthrough_target_socket<
        MockMultiPassthroughTargetSocket, BUSWIDTH, TYPES,
        N, POL> target_socket;
    conf_object_t conf_;
    simics::ConfObjectRef obj_;
    int b_transport_count_;
};

}  // namespace unittest

#endif
