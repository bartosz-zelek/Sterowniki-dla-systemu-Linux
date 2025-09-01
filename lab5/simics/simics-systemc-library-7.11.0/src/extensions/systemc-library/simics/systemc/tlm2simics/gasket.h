// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2012 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_H

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include <simics/systemc/adapter.h>
#include <simics/systemc/context.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/null_simulation.h>
#include <simics/systemc/tlm2simics/null_transaction_handler.h>
#include <simics/systemc/tlm2simics/gasket_interface.h>
#include <simics/systemc/tlm2simics/transaction_handler_interface.h>
#include <simics/systemc/tlm2simics/non_blocking_tlm_extension.h>

#include <string>

namespace simics {
namespace systemc {
namespace tlm2simics {

/** \internal RAII wrapper for ProcessStack */
class ProcessStackHandler {
  public:
    explicit ProcessStackHandler(InternalInterface *internal) {
        pstack_ = internal->process_stack();
        pstack_->push();
    }
    ProcessStackHandler(const ProcessStackHandler&) = delete;
    ProcessStackHandler& operator=(const ProcessStackHandler&) = delete;
    virtual ~ProcessStackHandler() {
        pstack_->pop();
    }

  private:
    ProcessStackInterface *pstack_;
};

/**
 * Implements core functionality for receiving a TLM2 transaction over a
 * socket. This class is associated with a transaction handler implementing the
 * TransactionHandlerInterface to which the Gasket passes the incoming
 * transaction for further processing.
 */
template<unsigned int BUSWIDTH = 32,
         typename TYPES = tlm::tlm_base_protocol_types>
class Gasket : public sc_core::sc_module, public GasketInterface {
  public:
    Gasket(sc_core::sc_module_name,
           const simics::ConfObjectRef &obj)
        : simics_obj_(obj), socket_("target_socket"),
          transaction_handler_(new NullTransactionHandler),
          internal_(NULL),
          initiator_socket_(NULL) {
        FATAL_ERROR_IF(!obj, "Must provide a valid Simics object");
        // only registers a b_transport callback with the
        // simple target socket since it will automatically
        // convert non-blocking call to blocking call
        socket_.register_b_transport(this, &Gasket::b_transport);
        socket_.register_get_direct_mem_ptr(this, &Gasket::get_direct_mem_ptr);
        socket_.register_transport_dbg(this, &Gasket::transport_dbg);
    }
    void init(InternalInterface *internal) {
        internal_ = internal;
    }
    virtual ~Gasket() {
        if (dynamic_cast<NullTransactionHandler *>(transaction_handler_))
            delete transaction_handler_;
    }

    // Used by createGasket() to bind with initiator socket
    template <typename Socket>
    void bind(Socket &sock) {  // NOLINT
        sock(socket_);
        initiator_socket_ = &sock;
    }

    // GasketInterface
    virtual void set_transaction_handler(TransactionHandlerInterface
        *transaction_handler) {
        if (dynamic_cast<NullTransactionHandler *>(transaction_handler_))
            delete transaction_handler_;
        transaction_handler_ = transaction_handler ? transaction_handler
                                                   : new NullTransactionHandler;
    }
    virtual void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                           sc_dt::uint64 end_range) {
        socket_->invalidate_direct_mem_ptr(start_range, end_range);
    }
    sc_core::sc_object *get_initiator_socket() const override {
        return initiator_socket_;
    }
    std::string gasket_name() const override {
        return name();
    }
    TransactionHandlerInterface *transaction_handler() override {
        return transaction_handler_;
    }
    // By registering nb_transport_fw, it no longer uses simple target
    // socket's auto b – nb conversion
    void register_nb_transport_fw() {
        socket_.register_nb_transport_fw(this, &Gasket::nb_transport_fw);
    }

  private:
    void execute_transaction(tlm::tlm_generic_payload &trans,  // NOLINT
                             bool non_blocking) {
        ProcessStackHandler pstack(internal());
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
        tlm::tlm_response_status status = tlm::TLM_INCOMPLETE_RESPONSE;
        {
            if (non_blocking) {
                trans.set_extension(&nb_ext_);
            }

            // Switching context to detect re-entry without setting context.
            Context c(&null_simulation_);
            status = transaction_handler_->simics_transaction(simics_obj_,
                                                              &trans);

            if (non_blocking) {
                trans.clear_extension(&nb_ext_);
            }
        }

        // When execute_transaction successfully returns, the response on
        // the trans should be same with the status
        if (trans.get_response_status() != status &&
            status != tlm::TLM_OK_RESPONSE) {
            const char *msg = "Gasket transaction error";
            switch (status) {
                case tlm::TLM_INCOMPLETE_RESPONSE: {
                    msg = "Gasket did not attempt to execute the command";
                    break;
                }
                case tlm::TLM_ADDRESS_ERROR_RESPONSE: {
                    msg = "Gasket was unable to act on the address attribute, "
                          "or address out-of-range";
                    break;
                }
                case tlm::TLM_COMMAND_ERROR_RESPONSE: {
                    msg = "Gasket was unable to execute the command";
                    break;
                }
                case tlm::TLM_BURST_ERROR_RESPONSE: {
                    msg = "Gasket was unable to act on the data length or "
                          "streaming width";
                    break;
                }
                case tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE: {
                    msg = "Gasket was unable to act on the byte enable";
                    break;
                }
                case tlm::TLM_GENERIC_ERROR_RESPONSE: {
                    break;
                }
                default: {
                }
            }
            SIM_LOG_SPEC_VIOLATION(1, simics_obj_, 0, "%s", msg);
        }
        trans.set_response_status(status);
        transaction_handler_->update_dmi_allowed(simics_obj_, &trans);
    }

    void b_transport(tlm::tlm_generic_payload &trans,  // NOLINT
                     sc_core::sc_time &local_time_offset) {  // NOLINT
        if (trans.get_byte_enable_ptr() != NULL &&
            !transaction_handler_->byte_enable_supported(simics_obj_, &trans)) {
            return;
        }

        // Before we send the transaction to simics we need to synchronize
        // local time.
        if (local_time_offset.value() > 0) {
#ifndef RISC_SIMICS
            wait(local_time_offset);
#endif
            local_time_offset = sc_core::SC_ZERO_TIME;
        }

        execute_transaction(trans, false);
    }

    unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {  // NOLINT
        ProcessStackHandler pstack(internal());
        return transaction_handler_->debug_transaction(simics_obj_, &trans);
    }

    bool get_direct_mem_ptr(tlm::tlm_generic_payload &trans,  // NOLINT
                            tlm::tlm_dmi &dmi_data) {  // NOLINT
        ProcessStackHandler pstack(internal());
        return transaction_handler_->get_direct_mem_ptr(simics_obj_,
                                                        trans, dmi_data);
    }

    // Non-blocking transport interface
    virtual tlm::tlm_sync_enum nb_transport_fw(
            tlm::tlm_generic_payload &trans,  // NOLINT
            tlm::tlm_phase &phase,  // NOLINT
            sc_core::sc_time &local_time_offset) {  // NOLINT
        if (phase == tlm::END_RESP) {
            trans.set_response_status(tlm::TLM_OK_RESPONSE);
            return tlm::TLM_COMPLETED;
        }

        if (phase != tlm::BEGIN_REQ) {
            SIM_LOG_SPEC_VIOLATION(
                    1, simics_obj_, 0,
                    "Gasket was unable to execute the command (invalid phase)");
            trans.set_response_status(tlm::TLM_COMMAND_ERROR_RESPONSE);
            // Return early completion when failure to process
            return tlm::TLM_COMPLETED;
        }

        if (trans.get_byte_enable_ptr() != NULL &&
            !transaction_handler_->byte_enable_supported(simics_obj_, &trans)) {
            SIM_LOG_SPEC_VIOLATION(
                    1, simics_obj_, 0,
                    "Gasket was unable to execute the command (byte enable)");
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            // Return early completion when failure to process
            return tlm::TLM_COMPLETED;
        }

        execute_transaction(trans, true);
        if (trans.get_response_status() == tlm::TLM_INCOMPLETE_RESPONSE) {
            SIM_LOG_INFO(3, simics_obj_, 0, "Gasket deferred the transaction");
            return tlm::TLM_ACCEPTED;
        }
        return tlm::TLM_COMPLETED;
    }

    InternalInterface *internal() {
        if (!internal_) {
            Adapter *adapter = static_cast<Adapter *>(
                    SIM_object_data(simics_obj_));
            assert(adapter);
            internal_ = adapter->internal();
        }

        return internal_;
    }

    simics::ConfObjectRef simics_obj_;
    tlm_utils::simple_target_socket<Gasket, BUSWIDTH, TYPES> socket_;

    // Target object in Simics side receiving the TLM transaction
    TransactionHandlerInterface *transaction_handler_;
    NullSimulation null_simulation_;
    InternalInterface *internal_;
    sc_core::sc_object *initiator_socket_;

    NonBlockingTlmExtension nb_ext_;
};

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
