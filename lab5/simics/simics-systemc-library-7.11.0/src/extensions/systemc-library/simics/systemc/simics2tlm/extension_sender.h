// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2015 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_EXTENSION_SENDER_H
#define SIMICS_SYSTEMC_SIMICS2TLM_EXTENSION_SENDER_H

#include <simics/base/log.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>

namespace simics {
namespace systemc {
namespace simics2tlm {

/** Specialized extension sender for simics2tlm gaskets. */
//:: pre simics2tlm_ExtensionSender {{
class ExtensionSender : public iface::ExtensionSenderInterface {
  public:
    void init(simics2tlm::GasketInterface::Ptr gasket) {
        // coverity[copy_instead_of_move]
        gasket_ = gasket;
    }
    virtual iface::Transaction transaction() {
        return pool_.acquire();
    }
    virtual void send_extension(iface::Transaction *transaction) {
        gasket_->trigger(transaction);
    }
    virtual void send_failed(iface::Transaction *transaction) {
        SIM_LOG_ERROR(gasket_->simics_obj(), Log_TLM,
                      "Extension not processed correctly.");
    }

  private:
    simics2tlm::GasketInterface::Ptr gasket_;
    iface::TransactionPool pool_;
};
// }}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#endif
