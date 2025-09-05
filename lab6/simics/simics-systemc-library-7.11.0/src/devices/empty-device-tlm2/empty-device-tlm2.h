//               -*- c++ -*-

/*
  © 2010 Intel Corporation
*/

//  empty-device-tlm2 - Skeleton code to base new SystemC device module on
//  Transaction-oriented SystemC device defined using the TLM-2.0 structures

#ifndef EMPTY_DEVICE_TLM2_EMPTY_DEVICE_TLM2_H
#define EMPTY_DEVICE_TLM2_EMPTY_DEVICE_TLM2_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

SC_MODULE(empty_device_tlm2) {
  public:
    SC_CTOR(empty_device_tlm2) :
        target_socket("target_socket"),
        register1_(0xdeadbeef) {
        target_socket.register_b_transport(this,
                                           &empty_device_tlm2::b_transport);
    }

    tlm_utils::simple_target_socket<empty_device_tlm2> target_socket;

  private:
    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &t);

    sc_dt::sc_uint<32> register1_;
};

#endif
