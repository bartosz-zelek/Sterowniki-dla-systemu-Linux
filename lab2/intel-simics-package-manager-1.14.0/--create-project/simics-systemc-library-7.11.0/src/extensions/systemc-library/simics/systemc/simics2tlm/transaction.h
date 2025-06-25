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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_TRANSACTION_H
#define SIMICS_SYSTEMC_SIMICS2TLM_TRANSACTION_H

#include <simics/systemc/iface/transaction_interface.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/simics2tlm/gasket_owner.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

//:: pre simics2tlm_Transaction {{
/**
 * Class that implements the Simics transaction interface and translates it into
 * a TLM transaction.
 *
 * The TLM2 return codes are translated to Simics like this:
 *   TLM_OK_RESPONSE => Sim_PE_No_Exception,
 *   TLM_ADDRESS_ERROR_RESPONSE => Sim_PE_IO_Not_Taken or
 *                Sim_PE_Inquiry_Outside_Memory on inquiry access,
 *   remaining TLM2 errors => Sim_PE_IO_Error or
 *                Sim_PE_Inquiry_Unhandled on inquiry access
 */
class Transaction : public simics::systemc::iface::TransactionInterface,
                    public GasketOwner {
  public:
    exception_type_t issue(transaction_t *transaction, uint64 addr);

  private:
    /*
     * Update the TLM transaction before sending it over to the SystemC side
     * By default this function does nothing since the TLM transaction has
     * been filled with the basic required information. It can used to modify
     * the filled information or add more information including customized
     * extensions.
     *
     * @param simics_transaction the transaction received from Simics side
     * @param tlm_transaction the TLM transaction to be sent over to the SystemC
     *                        side
     */
    virtual void update_transaction(const transaction_t *simics_transaction,
                                    tlm::tlm_generic_payload *tlm_transaction) {
    }

    iface::TransactionPool pool_;
};
// }}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
