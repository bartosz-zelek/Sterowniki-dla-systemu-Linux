// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2014 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_NULL_GASKET_H
#define SIMICS_SYSTEMC_TLM2SIMICS_NULL_GASKET_H

#include <simics/systemc/tlm2simics/gasket_interface.h>
#include <simics/systemc/instance_counter.h>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

/**
 * Utility class that counts the number of instances. This class is used to
 * validate that all gaskets have been assigned before running the
 * simulation.
 */
class NullGasket : public GasketInterface,
                   public simics::systemc::InstanceCounter<NullGasket> {
  public:
    void set_transaction_handler(
            TransactionHandlerInterface *transaction_handler) override {
    }
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) override {
    }
    sc_core::sc_object *get_initiator_socket() const override {
        return NULL;
    }
    std::string gasket_name() const override {
        return "NullGasket";
    }
    TransactionHandlerInterface *transaction_handler() override {
        return NULL;
    }
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
