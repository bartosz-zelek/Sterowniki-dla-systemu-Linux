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

#ifndef SIMICS_SYSTEMC_IFACE_TRANSACTION_EXTENSION_H
#define SIMICS_SYSTEMC_IFACE_TRANSACTION_EXTENSION_H

#include <tlm>

namespace simics {
namespace systemc {
namespace iface {

class Transaction;

class TransactionStatus {
  public:
    enum Status {
        INVALID,
        OK,
        FAIL,
        ACTIVE
    };
    TransactionStatus() : status_(INVALID) {}
    TransactionStatus(Status status) : status_(status) {}  // NOLINT
    bool invalid() {
        return status_ == INVALID;
    }
    bool ok() {
        return status_ == OK;
    }
    bool fail() {
        return status_ == FAIL;
    }
    bool active() {
        return status_ == ACTIVE;
    }

  private:
    Status status_;
};

class TransactionExtension : public tlm::tlm_extension<TransactionExtension> {
  public:
    class Callback {
      public:
        virtual bool is_async(iface::Transaction *transaction) = 0;
        virtual void defer(iface::Transaction *transaction) = 0;
        virtual void complete(iface::Transaction *transaction,
                              bool success) = 0;
        virtual ~Callback() {}
    };
    TransactionExtension() {
        reset();
    }
    TransactionStatus status() {
        return status_;
    }
    void reset() {
        status_ = TransactionStatus::INVALID;
        transport_debug_ = false;
        callback_ = NULL;
    }
    void set_status(TransactionStatus status) {
        status_ = status;
    }
    bool transport_debug() {
        return transport_debug_;
    }
    /** For true, the gasket uses transport_dbg instead of b_transport. */
    void set_transport_debug(bool transport_debug) {
        transport_debug_ = transport_debug;
    }
    void set_callback(Callback *callback) {
        callback_ = callback;
    }
    Callback *callback() {
        return callback_;
    }
    virtual tlm::tlm_extension_base *clone() const {
        return new TransactionExtension(*this);
    }
    virtual void copy_from(tlm::tlm_extension_base const &ext) {
        *this = static_cast<const TransactionExtension &>(ext);
    }

  private:
    TransactionStatus status_;
    bool transport_debug_;
    Callback *callback_;
};

}  // namespace iface
}  // namespace systemc
}  // namespace simics

#endif
