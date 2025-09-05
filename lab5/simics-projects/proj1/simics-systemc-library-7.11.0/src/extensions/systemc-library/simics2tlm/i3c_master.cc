// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2019 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/simics2tlm/i3c_master.h>
#include <simics/systemc/adapter_log_groups.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

void I3cMaster::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_, &gasket_->payload());
}

void I3cMaster::acknowledge(types::i3c_ack_t ack) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cMaster::acknowledge, ack=%s",
                 ack == types::I3C_ack ? "I3C_ack" : "I3C_noack");
    extension_.acknowledge(ack);
}

void I3cMaster::read_response(uint8_t value, bool more) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cMaster::read_response, value=0x%x, more=%s",
                 value, more ? "True" : "False");
    extension_.read_response(value, more);
}

void I3cMaster::daa_response(uint64_t id, uint8_t bcr, uint8_t dcr) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cMaster::daa_response, id=%lu, bcr=0x%x, dcr=0x%x",
                 id, bcr, dcr);
    extension_.daa_response(id, bcr, dcr);
}
void I3cMaster::ibi_request() {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cMaster::ibi_request");
    extension_.ibi_request();
}
void I3cMaster::ibi_address(uint8_t address) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "I3cMaster::ibi_address, address=0x%x", address);
    extension_.ibi_address(address);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

