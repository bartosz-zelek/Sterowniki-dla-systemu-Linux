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

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_INTERFACE_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_INTERFACE_H

#include <simics/systemc/tlm2simics/transaction_handler_interface.h>

#include <memory>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

/** Interface used by tlm2simics gaskets, implemented by Gasket base class. */
class GasketInterface {
  public:
    typedef std::shared_ptr<GasketInterface> Ptr;
    /// Target object in Simics side receiving the TLM transaction
    virtual void set_transaction_handler(
            TransactionHandlerInterface *transaction_handler) = 0;

    /// Calling this method will end up calling the same method on
    /// the target socket, that will forward the call back to the
    /// initiator socket who is required to drop the DMI pointers
    /// matching the range.
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) = 0;
    virtual sc_core::sc_object *get_initiator_socket() const = 0;
    virtual std::string gasket_name() const = 0;
    virtual TransactionHandlerInterface *transaction_handler() = 0;

    /// For gaskets support non-blocking transport, call this function
    /// to register the non-blocking transport interface
    virtual void register_nb_transport_fw() {}

    virtual ~GasketInterface() {}
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
