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

#ifndef MOCK_TLM2SIMICS_MOCK_GASKET_H
#define MOCK_TLM2SIMICS_MOCK_GASKET_H

#include <simics/cc-api.h>
#include <systemc>
#include <tlm>

#include <simics/systemc/tlm2simics/gasket_interface.h>

#include <string>

namespace scl = simics::systemc;

namespace unittest {
namespace tlm2simics {

class MockGasket : public simics::systemc::tlm2simics::GasketInterface {
  public:
    MockGasket() : invalidate_direct_mem_ptr_call_cnt_(0),
                   transaction_handler_(NULL) {
    }

    virtual void set_transaction_handler(
        scl::tlm2simics::TransactionHandlerInterface *transaction_handler) {
        transaction_handler_ = transaction_handler;
    }
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) {
        ++invalidate_direct_mem_ptr_call_cnt_;
    }
    sc_core::sc_object *get_initiator_socket() const override {
        return NULL;
    }
    std::string gasket_name() const override {
        return "MockGasket";
    }
    scl::tlm2simics::TransactionHandlerInterface *
    transaction_handler() override {
        return transaction_handler_;
    }

    int invalidate_direct_mem_ptr_call_cnt_;
    scl::tlm2simics::TransactionHandlerInterface *transaction_handler_;
};

}  // namespace tlm2simics
}  // namespace unittest

#endif
