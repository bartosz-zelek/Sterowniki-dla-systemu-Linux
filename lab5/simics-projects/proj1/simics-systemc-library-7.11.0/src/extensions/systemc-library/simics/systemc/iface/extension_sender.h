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

#ifndef SIMICS_SYSTEMC_IFACE_EXTENSION_SENDER_H
#define SIMICS_SYSTEMC_IFACE_EXTENSION_SENDER_H

#include <simics/systemc/iface/extension_sender_interface.h>
#include <simics/systemc/iface/transaction.h>
#include <simics/systemc/iface/transaction_pool.h>

#include <tlm>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Generic extension sender initialized with a TLM2 initiator socket of
 * TSocket type. The payload with the extension is sent using b_transport()
 * interface method of the socket.
 */
template<typename TSocket>
class ExtensionSender : public ExtensionSenderInterface {
  public:
    ExtensionSender()
        : socket_(NULL), delay_(sc_core::SC_ZERO_TIME), payload_(NULL) {
    }
    void init(TSocket *socket) {
        socket_ = socket;
    }
    void set_delay(sc_core::sc_time delay) {
        delay_ = delay;
    }
    void set_payload(tlm::tlm_generic_payload *payload) {
        payload_ = payload;
    }
    virtual Transaction transaction() {
        if (payload_)
            return Transaction(payload_);

        return pool_.acquire();
    }
    virtual void send_extension(Transaction *transaction) {
        (*socket_)->b_transport(*transaction, delay_);
    }
    virtual void send_failed(Transaction *transaction) {
        // TODO(ah): SC_REPORT_ERROR might end up as a critical in Simics,
        // which stops the simulation. Is this what we wanted? A critical
        // sounds a bit harsh, while an error is more correct. How do we
        // achieve that?
        SC_REPORT_ERROR("/intel/ExtensionSender",
                        "Extension not processed correctly.");
    }
    virtual ~ExtensionSender() {}

  private:
    TSocket *socket_;
    sc_core::sc_time delay_;
    tlm::tlm_generic_payload *payload_;
    TransactionPool pool_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
