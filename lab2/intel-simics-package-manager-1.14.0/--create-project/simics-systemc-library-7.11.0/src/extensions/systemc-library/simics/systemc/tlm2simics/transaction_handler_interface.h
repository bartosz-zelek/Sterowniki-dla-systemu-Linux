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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_TRANSACTION_HANDLER_INTERFACE_H
#define SIMICS_SYSTEMC_TLM2SIMICS_TRANSACTION_HANDLER_INTERFACE_H

#include <simics/conf-object.h>
#include <simics/systemc/iface/receiver_interface.h>
#include <tlm>

namespace simics {
namespace systemc {

class InterfaceProvider;

namespace tlm2simics {

class GasketOwner;

/**
 * Interface used by Gasket, implemented by protocol specific transaction
 * handlers.
 */
class TransactionHandlerInterface {
  public:
    virtual bool get_direct_mem_ptr(ConfObjectRef &simics_obj,  // NOLINT
                                    tlm::tlm_generic_payload &trans,  // NOLINT
                                    tlm::tlm_dmi& dmi_data) = 0;  // NOLINT
    virtual tlm::tlm_response_status simics_transaction(
            ConfObjectRef &simics_obj,  // NOLINT
            tlm::tlm_generic_payload *trans) = 0;
    virtual unsigned int debug_transaction(ConfObjectRef &simics_obj,  // NOLINT
                                           tlm::tlm_generic_payload *trans) = 0;
    virtual void update_dmi_allowed(ConfObjectRef &simics_obj,  // NOLINT
                                    tlm::tlm_generic_payload *trans) = 0;
    virtual bool byte_enable_supported(ConfObjectRef &simics_obj,  // NOLINT
                                       tlm::tlm_generic_payload *trans) = 0;
    virtual const GasketOwner *gasket_owner() const = 0;
    virtual const InterfaceProvider *interface_provider() const = 0;
    virtual iface::ReceiverInterface *receiver() {return NULL;}
    virtual ~TransactionHandlerInterface() {
    }
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
