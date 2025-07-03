// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#include <simics/systemc/tlm2simics/packet.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/model-iface/packet.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

iface::ReceiverInterface *Packet::receiver() {
    return NULL;
}

unsigned int Packet::transaction(ConfObjectRef &simics_obj,
                                 tlm::tlm_generic_payload *trans,
                                 bool inquiry) {
    if (!trans->is_write()) {
        SIM_LOG_SPEC_VIOLATION(1, simics_obj, Log_TLM,
                               "GP must be write transaction");
        trans->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    if (trans->get_address()) {
        SIM_LOG_SPEC_VIOLATION(1, simics_obj, Log_TLM,
                               "GP address must be 0");
        trans->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return 0;
    }

    bytes_t data;
    data.data = static_cast<uint8_t *>(trans->get_data_ptr());
    data.len = trans->get_data_length();

    get_interface<packet_interface_t>()->transfer(target().object(), data);
    trans->set_response_status(tlm::TLM_OK_RESPONSE);
    return data.len;
}

tlm::tlm_response_status Packet::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    transaction(simics_obj, trans, false);
    return trans->get_response_status();
}

unsigned int Packet::debug_transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                                       tlm::tlm_generic_payload *trans) {
    return transaction(simics_obj, trans, true);
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
