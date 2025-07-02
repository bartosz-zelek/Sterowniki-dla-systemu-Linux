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

#include <simics/systemc/tlm2simics/memory_space.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/devs/memory-space.h>

#include <stdint.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

iface::ReceiverInterface *MemorySpace::receiver() {
    return NULL;
}

unsigned int MemorySpace::transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                                      tlm::tlm_generic_payload *trans,
                                      bool inquiry) {
    conf_object_t *cobj = target().object();
    conf_object_t *initiator = simics_obj;
    physical_address_t addr = trans->get_address();
    uint8_t *buf = static_cast<uint8_t *>(trans->get_data_ptr());
    unsigned int len = trans->get_data_length();
    read_or_write_t type = trans->is_read() ? Sim_RW_Read : Sim_RW_Write;

    if (len == 0) {
        SIM_LOG_SPEC_VIOLATION(1, simics_obj, Log_TLM,
                               "GP data length set to 0");
        trans->set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return len;
    }

    exception_type_t ex;
    if (!inquiry)
        ex = get_interface<memory_space_interface_t>()->access_simple(
                cobj, initiator, addr, buf, len, type, Sim_Endian_Target);
    else
        ex = get_interface<memory_space_interface_t>()->access_simple_inq(
                cobj, initiator, addr, buf, len, type, Sim_Endian_Target);

    if (ex != Sim_PE_No_Exception) {
        SIM_LOG_ERROR(simics_obj, Log_TLM,
                      "%s memory address 0x%llx, size 0x%x",
                      type == Sim_RW_Read ? "reading" : "writing", addr, len);
        trans->set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    } else {
        trans->set_response_status(tlm::TLM_OK_RESPONSE);
    }

    return len;
}

tlm::tlm_response_status MemorySpace::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    transaction(simics_obj, trans, false);
    return trans->get_response_status();
}

unsigned int MemorySpace::debug_transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                                            tlm::tlm_generic_payload *trans) {
    return transaction(simics_obj, trans, true);
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
