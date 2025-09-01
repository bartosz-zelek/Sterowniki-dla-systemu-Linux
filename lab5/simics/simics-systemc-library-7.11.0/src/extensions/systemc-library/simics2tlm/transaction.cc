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

#include <simics/base/log.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/simics2tlm/transaction.h>

#include <vector>

namespace {
class DataBuffer {
  public:
    explicit DataBuffer(unsigned int size)
        : data_(size, 0) {
        buffer_.data = &data_.front();
        buffer_.len = size;
    }
    unsigned char *memory() {
        return &data_.front();
    }
    void copyToBuffer(transaction_t *transaction) {
        SIM_get_transaction_bytes(transaction, buffer_);
    }
    void copyFromBuffer(transaction_t *transaction) {
        bytes_t bytes;
        bytes.data = buffer_.data;
        bytes.len = buffer_.len;
        SIM_set_transaction_bytes(transaction, bytes);
    }

  protected:
    std::vector<unsigned char> data_;
    buffer_t buffer_;
};

class Callback
    : public simics::systemc::iface::TransactionExtension::Callback {
  public:
    Callback(conf_object_t *obj, transaction_t *transaction,
             DataBuffer *data_buffer)
     : obj_(obj), transaction_(transaction), deferred_(NULL),
       data_buffer_(data_buffer), result_(Sim_PE_No_Exception),
       success_(false) {
    }
    Callback(const Callback&) = delete;
    Callback& operator=(const Callback&) = delete;
    virtual ~Callback() {
        delete data_buffer_;
    }
    bool is_async(simics::systemc::iface::Transaction *transaction) {
        return deferred_ != NULL;
    }
    void defer(simics::systemc::iface::Transaction *transaction) {
        deferred_ = SIM_defer_transaction(obj_, transaction_);
    }
    void complete(simics::systemc::iface::Transaction *transaction,
                  bool success) {
        transaction_t *t = deferred_ ? deferred_ : transaction_;
        if (SIM_transaction_is_read(t))
            data_buffer_->copyFromBuffer(t);

        success_ = success;
        if (!success_) {
            if (transaction->payload()->get_response_status() ==
                    tlm::TLM_ADDRESS_ERROR_RESPONSE) {
                result_ = SIM_transaction_is_inquiry(t)
                    ? Sim_PE_Inquiry_Outside_Memory
                    : Sim_PE_IO_Not_Taken;
            } else {
                result_ = SIM_transaction_is_inquiry(t)
                    ? Sim_PE_Inquiry_Unhandled
                    : Sim_PE_IO_Error;
            }
        }

        if (deferred_) {
            SIM_complete_transaction(deferred_,
                                     static_cast<exception_type_t>(result_));
            delete this;
        }
    }
    exception_type_t result() {
        return result_;
    }

  private:
    conf_object_t *obj_;
    transaction_t *transaction_;
    transaction_t *deferred_;
    DataBuffer *data_buffer_;
    exception_type_t result_;
    bool success_;
};
}  // namespace

namespace simics {
namespace systemc {
namespace simics2tlm {

exception_type_t Transaction::issue(transaction_t *transaction,
                                    uint64 addr) {
    if (!SIM_transaction_is_write(transaction) &&
        !SIM_transaction_is_read(transaction)) {
        SIM_LOG_SPEC_VIOLATION(1, gasket_->simics_obj(), Log_TLM,
                               "Transaction must be read or write");
        return Sim_PE_IO_Error;
    }

    unsigned int size = SIM_transaction_size(transaction);
    DataBuffer *data_buffer = new DataBuffer(size);
    iface::Transaction t = pool_.acquire();

    t->set_data_ptr(data_buffer->memory());
    t->set_address(addr);
    t->set_data_length(size);
    t->set_streaming_width(size);
    t.extension()->set_transport_debug(SIM_transaction_is_inquiry(transaction));
    if (SIM_transaction_is_read(transaction)) {
        // Read operation
        t->set_read();
    } else {
        // Write operation
        t->set_write();
        data_buffer->copyToBuffer(transaction);
    }

    Callback *cb = new Callback(gasket_->simics_obj(), transaction,
                                data_buffer);
    t.extension()->set_callback(cb);
    update_transaction(transaction, t.payload());
    gasket_->trigger(&t);
    if (!cb->is_async(&t)) {
        exception_type_t result = cb->result();
        delete cb;
        return result;
    }

    return Sim_PE_Deferred;
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
