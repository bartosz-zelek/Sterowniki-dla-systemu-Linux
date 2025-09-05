// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2021 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/simics2tlm/i3c_slave.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void I3cSlave::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_, &gasket_->payload());
}

void I3cSlave::start(uint8_t address) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::start, address=0x%x", address);
    extension_.start(address);
}

void I3cSlave::write(uint8_t value) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::write, value=0x%x", value);
    extension_.write(value);
}

void I3cSlave::sdr_write(types::bytes_t data) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::sdr_write");
    extension_.sdr_write(data);
}

void I3cSlave::read() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::read");
    extension_.read();
}

void I3cSlave::daa_read() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::daa_read");
    extension_.daa_read();
}

void I3cSlave::stop() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::stop");
    extension_.stop();
}

void I3cSlave::ibi_start() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::ibi_start");
    extension_.ibi_start();
}

void I3cSlave::ibi_acknowledge(types::i3c_ack_t ack) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cSlave::ibi_acknowledge, ack=%s",
                 ack == types::I3C_ack ? "I3C_ack" : "I3C_noack");
    extension_.ibi_acknowledge(ack);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
