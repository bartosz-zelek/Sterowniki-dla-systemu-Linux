// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2018 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_IFACE_TRANSACTION_H
#define SIMICS_SYSTEMC_IFACE_TRANSACTION_H

#include <simics/systemc/iface/transaction_extension.h>

#include <tlm>

namespace simics {
namespace systemc {
namespace iface {

/**
 * Class that encapsulates a generic_payload and returns it to the
 * TransactionPool when the Transaction object goes out of scope.
 */
class Transaction {
  public:
    explicit Transaction(tlm::tlm_generic_payload *payload)
      : payload_(payload) {
        acquire();
    }
    Transaction() : payload_(NULL) {}
    /** Increases usage count for payload */
    Transaction(const Transaction &t) {
        payload_ = t.payload_;
        acquire();
    }
    /** Increases usage count for new payload and returns former to pool */
    Transaction &operator = (const Transaction &t) {
        if (this == &t)
            return *this;

        release();
        payload_ = t.payload_;
        acquire();
        return *this;
    }
    /** Returns payload to pool */
    ~Transaction() {
        release();
    }
    /** Access payload with -> */
    tlm::tlm_generic_payload *operator->() {
        return payload_;
    }
    /** Access payload with -> */
    const tlm::tlm_generic_payload *operator->() const {
        return payload_;
    }
    /** Access payload with & */
    operator tlm::tlm_generic_payload &() {
        return *payload_;
    }
    /** Access payload with \* */
    operator tlm::tlm_generic_payload *() {
        return payload_;
    }
    /** Access payload by payload() */
    tlm::tlm_generic_payload *payload() {
        return payload_;
    }
    TransactionExtension *extension() {
        if (!payload_)
            return NULL;

        TransactionExtension *e = NULL;
        payload_->get_extension(e);
        return e;
    }

  private:
    void acquire() {
        if (payload_ && payload_->has_mm())
            payload_->acquire();
    }
    void release() {
        if (payload_ && payload_->has_mm())
            payload_->release();
    }

    tlm::tlm_generic_payload *payload_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
