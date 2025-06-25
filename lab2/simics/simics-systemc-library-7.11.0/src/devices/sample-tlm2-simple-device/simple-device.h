/*                                                              -*- C++ -*-

  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SAMPLE_TLM2_SIMPLE_DEVICE_SIMPLE_DEVICE_H
#define SAMPLE_TLM2_SIMPLE_DEVICE_SIMPLE_DEVICE_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include <stdint.h>

/**
 * A simple class that implements TLM2 functionality providing two memory
 * mapped registers
 */
class SimpleDevice : public sc_core::sc_module {
  public:
    SimpleDevice(::sc_core::sc_module_name, int access_delay_ns)
        : target_socket("target_socket"),
          access_delay_(sc_core::sc_time(access_delay_ns, sc_core::SC_NS)),
          register1_(0xdeadbeef),
          register2_(0x00c0ffee) {
        target_socket.register_b_transport(this, &SimpleDevice::b_transport);
    }

    void set_register1(uint32_t value);
    uint32_t register1() const;
    void set_register2(uint32_t value);
    uint32_t register2() const;

    tlm_utils::simple_target_socket<SimpleDevice> target_socket;

  private:
    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT: SystemC API
                     sc_core::sc_time &t);  // NOLINT: SystemC API

    const sc_core::sc_time access_delay_;
    uint32_t register1_;
    uint32_t register2_;
};

#endif
