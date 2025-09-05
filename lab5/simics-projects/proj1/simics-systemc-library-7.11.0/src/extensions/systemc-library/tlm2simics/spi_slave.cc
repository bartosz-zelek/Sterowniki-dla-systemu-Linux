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

#include <simics/systemc/tlm2simics/spi_slave.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/devs/serial-peripheral-interface.h>
#include <simics/util/dbuffer.h>

namespace simics {
namespace systemc {
namespace tlm2simics {

iface::ReceiverInterface *SpiSlave::receiver() {
    return NULL;
}

unsigned int SpiSlave::transaction(simics::ConfObjectRef &simics_obj,
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

    dbuffer_t *dbuf = new_dbuffer();
    unsigned int len = trans->get_data_length();
    dbuffer_append_data(dbuf, trans->get_data_ptr(), len);

    // The 'first' and 'last' params are not used, set to 0
    get_interface<
        serial_peripheral_interface_slave_interface_t>()->spi_request(
                target().object(), 0, 0, len * 8, dbuf);
    dbuffer_free(dbuf);
    trans->set_response_status(tlm::TLM_OK_RESPONSE);
    return len;
}

tlm::tlm_response_status SpiSlave::simics_transaction(
        simics::ConfObjectRef &simics_obj,
        tlm::tlm_generic_payload *trans) {
    transaction(simics_obj, trans, false);
    return trans->get_response_status();
}

unsigned int SpiSlave::debug_transaction(simics::ConfObjectRef &simics_obj,  // NOLINT
                                         tlm::tlm_generic_payload *trans) {
    return transaction(simics_obj, trans, true);
}

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics
