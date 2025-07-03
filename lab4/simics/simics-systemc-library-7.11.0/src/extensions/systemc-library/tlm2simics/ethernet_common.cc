// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2016 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/tlm2simics/ethernet_common.h>
#include <simics/devs/ethernet.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

void EthernetCommon::frame(const simics::types::frags_t *frame, int crc) {
    return get_interface<ethernet_common_interface_t>()->frame(
        target().object(), reinterpret_cast<const frags_t *>(frame),
        static_cast<eth_frame_crc_status_t>(crc));
}

iface::ReceiverInterface *EthernetCommon::receiver() {
    return receiver_;
}

EthernetCommon::~EthernetCommon() {
    delete receiver_;
}

tlm::tlm_response_status EthernetCommon::simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (receiver_->handle(trans))
        return tlm::TLM_OK_RESPONSE;

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0,
                           "Expected EthernetCommonExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
