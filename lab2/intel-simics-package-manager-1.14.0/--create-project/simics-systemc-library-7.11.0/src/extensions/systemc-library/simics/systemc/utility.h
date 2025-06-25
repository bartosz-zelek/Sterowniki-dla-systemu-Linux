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

#ifndef SIMICS_SYSTEMC_UTILITY_H
#define SIMICS_SYSTEMC_UTILITY_H

#include <tlm>

namespace simics {
namespace systemc {
namespace utility {

/** Reset the payload struct to default values. Useful when re-using the same
    payload struct for the next transaction. */
static void reset_payload(tlm::tlm_generic_payload *gp) {
    gp->set_address(0);
    gp->set_command(tlm::TLM_IGNORE_COMMAND);
    gp->set_data_ptr(NULL);
    gp->set_data_length(0);
    gp->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
    gp->set_dmi_allowed(false);
    gp->set_byte_enable_ptr(NULL);
    gp->set_byte_enable_length(0);
    gp->set_streaming_width(0);
    gp->set_gp_option(tlm::TLM_MIN_PAYLOAD);
    // m_extensions, m_mm and m_ref_count left unchannged
}

}  // namespace utility
}  // namespace systemc
}  // namespace simics

#endif
