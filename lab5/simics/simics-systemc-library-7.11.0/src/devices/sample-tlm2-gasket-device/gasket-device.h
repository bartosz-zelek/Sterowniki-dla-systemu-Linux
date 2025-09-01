/*                                                              -*- C++ -*-

  © 2017 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_GASKET_DEVICE_GASKET_DEVICE_H
#define SAMPLE_TLM2_GASKET_DEVICE_GASKET_DEVICE_H

#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>

class GasketDevice : public sc_core::sc_module {
  public:
    SC_CTOR(GasketDevice)
        : socket32_("socket32")
        , socket64_("socket64")
        , multi_socket_("multi_socket")
        , multi_socket64_("multi_socket64")
        , initiator_socket_("initiator_socket")
        , multi_initiator_socket_("multi_initiator_socket")
        , sc_in_("sc_in")
        , sc_out_("sc_out")
        , sc_in_2_("sc_in_2")
        , sc_out_2_("sc_out_2")
        , sc_in_vec_("sc_in_vec", VECTOR_SIZE)
        , sc_out_vec_("sc_out_vec", VECTOR_SIZE) {
        socket32_.register_b_transport(this, &GasketDevice::b_transport);
        socket64_.register_b_transport(this, &GasketDevice::b_transport);
        multi_socket_.register_b_transport(this,
                                           &GasketDevice::multi_b_transport);
        multi_socket64_.register_b_transport(this,
                                             &GasketDevice::multi_b_transport);
        SC_METHOD(in_changed);
        sensitive << sc_in_;
        dont_initialize();

        SC_METHOD(in_2_changed);
        sensitive << sc_in_2_;
        dont_initialize();
    }
    void in_changed() {
        sc_out_ = sc_in_;
    }
    void in_2_changed() {
        sc_out_2_ = sc_in_2_;
    }

    static const int VECTOR_SIZE = 100;

  private:
    tlm_utils::simple_target_socket<GasketDevice> socket32_;
    tlm_utils::simple_target_socket<GasketDevice, 64> socket64_;
    tlm_utils::multi_passthrough_target_socket<GasketDevice> multi_socket_;
    // add one extra multi socket for the late binder test
    tlm_utils::multi_passthrough_target_socket<GasketDevice, 64>
        multi_socket64_;

    tlm_utils::simple_initiator_socket<GasketDevice> initiator_socket_;
    tlm_utils::multi_passthrough_initiator_socket<GasketDevice>
        multi_initiator_socket_;
    sc_core::sc_in<bool> sc_in_;
    sc_core::sc_out<bool> sc_out_;
    sc_core::sc_inout<bool> sc_in_2_;
    sc_core::sc_inout<bool> sc_out_2_;

    sc_core::sc_vector<sc_core::sc_in<bool>> sc_in_vec_;
    sc_core::sc_vector<sc_core::sc_out<bool>> sc_out_vec_;

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                     sc_core::sc_time &local_time);  // NOLINT: SystemC API
    void multi_b_transport(int idx, tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                           sc_core::sc_time &local_time);  // NOLINT: SystemC API
};

// <add id="sample-tlm2-gasket-device/gasket-device">
// <insert-until text="// EOF_GASKET_DEVICE"/></add>
class Top : public sc_core::sc_module {
  public:
    SC_CTOR(Top)
        : target_socket_("target_socket")
        , initiator_socket_("initiator_socket") {
        target_socket_.register_b_transport(this, &Top::b_transport);
    }

  private:
    tlm_utils::simple_target_socket<Top> target_socket_;
    tlm_utils::simple_initiator_socket<Top> initiator_socket_;

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                     sc_core::sc_time &local_time) {  // NOLINT: SystemC API
        initiator_socket_->b_transport(trans, local_time);
    }
};
// EOF_GASKET_DEVICE

// <add id="sample-tlm2-gasket-device/gasket-vector-device">
// <insert-until text="// EOF_GASKET_VECTOR_DEVICE"/></add>
class TopVector : public sc_core::sc_module {
  public:
    SC_CTOR(TopVector)
        : sc_in_vec_("sc_in_vec", VECTOR_SIZE)
        , sc_out_vec_("sc_out_vec", VECTOR_SIZE) {}

    static const int VECTOR_SIZE = 100;

  private:
    sc_core::sc_vector<sc_core::sc_in<bool>> sc_in_vec_;
    sc_core::sc_vector<sc_core::sc_out<bool>> sc_out_vec_;
};
// EOF_GASKET_VECTOR_DEVICE

#endif
