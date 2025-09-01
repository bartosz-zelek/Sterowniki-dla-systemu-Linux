// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2013 Intel Corporation

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
#include <simics/systemc/simics2tlm/pci_express.h>
#undef SIMICS_6_API
#endif
#include <simics/systemc/adapter_log_groups.h>

#include <vector>

namespace simics {
namespace systemc {
namespace simics2tlm {

void PciExpress::gasketUpdated() {
    sender_.init(gasket_);
    extension_.init(&sender_);
}

int PciExpress::send_message(int type, const std::vector<uint8_t> &payload) {
    SIM_LOG_INFO(4, gasket()->simics_obj(), Log_TLM,
                 "PciExpress::send_message, type=%d", type);
    return extension_.send_message(type, payload);
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
