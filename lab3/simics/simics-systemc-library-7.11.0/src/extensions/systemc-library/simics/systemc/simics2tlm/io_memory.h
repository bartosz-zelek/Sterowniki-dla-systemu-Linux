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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_IO_MEMORY_H
#define SIMICS_SYSTEMC_SIMICS2TLM_IO_MEMORY_H

#include <simics/systemc/iface/io_memory_interface.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/simics2tlm/multi_gasket_owner.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

#define DEPRECATED_FUNC(f) _deprecated_ ## f

/**
 * Class that implements the Simics io_memory interface and translates it into
 * a TLM transaction. This particular interface class is derived from
 * MultiGasketOwner making it capable of routing the interface to different
 * destinations based on the transaction data.
 *
 * The TLM2 return codes are translated to Simics like this:
 *   TLM_OK_RESPONSE => Sim_PE_No_Exception,
 *   TLM_ADDRESS_ERROR_RESPONSE => Sim_PE_IO_Not_Taken or
 *                Sim_PE_Inquiry_Outside_Memory on inquiry access,
 *   remaining TLM2 errors => Sim_PE_IO_Error or
 *                Sim_PE_Inquiry_Unhandled on inquiry access
 */
class IoMemory : public simics::systemc::iface::IoMemoryInterface,
                 public MultiGasketOwner {
  public:
    IoMemory() {
        set_type();
    }
    int DEPRECATED_FUNC(map)(addr_space_t memory_or_io, map_info_t info) {
        return 0;  // short circuit deprecated functionality
    }
    exception_type_t operation(generic_transaction_t *mem_op,
                               const types::map_info_t &info);

  private:
    iface::TransactionPool pool_;
};

#undef DEPRECATED_FUNC

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
