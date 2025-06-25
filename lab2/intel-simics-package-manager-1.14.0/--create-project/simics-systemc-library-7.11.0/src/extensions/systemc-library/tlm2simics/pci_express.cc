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

#if !defined(SIMICS_6_API)
#define SIMICS_6_API
#include <simics/systemc/tlm2simics/pci_express.h>
#undef SIMICS_6_API
#endif
#include <simics/devs/pci.h>

#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

int PciExpress::send_message(int type, const std::vector<uint8_t> &payload) {
    byte_string_t byte_string = {};
    byte_string.len = payload.size();
    if (payload.size() > 0) {
        byte_string.str = const_cast<uint8 *>(&payload[0]);
    }

    return get_interface<pci_express_interface_t>()->send_message(
        target().object(), device_,
        static_cast<pcie_message_type_t>(type), byte_string);
}

iface::ReceiverInterface *PciExpress::receiver() {
    return receiver_;
}

PciExpress::~PciExpress() {
    delete receiver_;
}

tlm::tlm_response_status PciExpress::simics_transaction(
        ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    if (!device_) {
        device_ = simics_obj.object();
    }

    if (receiver_->handle(trans)) {
        return tlm::TLM_OK_RESPONSE;
    }

    SIM_LOG_SPEC_VIOLATION(1, simics_obj, 0, "Expected PciExpressExtension");
    return tlm::TLM_COMMAND_ERROR_RESPONSE;
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
