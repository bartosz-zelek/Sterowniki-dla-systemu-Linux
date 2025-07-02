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

#include "gasket.h"

#include <simics/systemc/adapter.h>
#include <simics/systemc/adapter_log_groups.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/kernel_state_modifier.h>
#include <simics/systemc/thread_pool.h>
#include <simics/systemc/utility.h>

#include <string>

namespace simics {
namespace systemc {
namespace simics2tlm {

template<unsigned int BUSWIDTH, typename TYPES>
Gasket<BUSWIDTH, TYPES>::Gasket(sc_core::sc_module_name,
                                const simics::ConfObjectRef &simics_obj)
    : simics_obj_(simics_obj),
      transaction_(NULL),
      type_(NULL),
      target_socket_(NULL),
      initiator_socket_("initiator_socket"),
      dmi_(true),
      simulation_(NULL),
      internal_(NULL),
      deprecated_transaction_(&deprecated_payload_) {
    FATAL_ERROR_IF(!simics_obj_, "Must provide a valid Simics object");
    deprecated_payload_.set_extension(new iface::TransactionExtension);
    initiator_socket_.register_invalidate_direct_mem_ptr(
        this, &Gasket::invalidate_direct_mem_ptr);
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::init(iface::SimulationInterface *simulation,
                                   InternalInterface *internal) {
    simulation_ = simulation;
    internal_ = internal;
}

template<unsigned int BUSWIDTH, typename TYPES>
tlm::tlm_generic_payload &Gasket<BUSWIDTH, TYPES>::payload() {
    simics::systemc::utility::reset_payload(&deprecated_payload_);
    return deprecated_payload_;
}

template<unsigned int BUSWIDTH, typename TYPES>
bool Gasket<BUSWIDTH, TYPES>::trigger_transaction() {
    return trigger(&deprecated_transaction_);
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::run(ThreadInterface *call) {
    ThreadData *data = dynamic_cast<ThreadData *>(call);
    assert(data);
    data->init(transaction_, simulation_, internal_);

    if (transaction_ == NULL ||
        !transaction_->extension()->status().active()) {
        SIM_LOG_CRITICAL(simics_obj(), Log_TLM,
                         "Transaction triggered in non-active state.");
        return;
    }

    send_transaction(&data->transaction_);
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::block(ThreadInterface *call) {
    ThreadData *data = dynamic_cast<ThreadData *>(call);
    assert(data);
    iface::TransactionExtension::Callback *callback =
        get_callback(&data->transaction_);
    if (callback && data->transaction_.extension()->status().active())
        callback->defer(&data->transaction_);

    // Some things have to be cached because the callback is deleted
    // once completed has been called.
    // For sync transaction the extension is reset at end of trigger().
    bool sync = !callback || !callback->is_async(&data->transaction_);
    if (sync)
        data->must_call_sc_pause_ = true;
    else
        data->must_reset_extension_ = true;
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::finish(ThreadInterface *call) {
    ThreadData *data = dynamic_cast<ThreadData *>(call);
    assert(data);
    if (data->must_reset_extension_) {
        data->transaction_.extension()->reset();
    }

    // For the sync case, simulation.runSimulation(sc_core::sc_max_time())
    // ends when this sc_thread finishes by invoking sc_pause here.
    if (data->must_call_sc_pause_)
        data->internal_->pauseSimulationNoBreak();
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::exception(ThreadInterface *call) {
    ThreadData *data = dynamic_cast<ThreadData *>(call);
    assert(data);
}

template<unsigned int BUSWIDTH, typename TYPES>
iface::SimulationInterface *Gasket<BUSWIDTH, TYPES>::simulation(
        ThreadInterface *call) {
    return simulation_;
}

template<unsigned int BUSWIDTH, typename TYPES>
bool Gasket<BUSWIDTH, TYPES>::trigger(iface::Transaction *transaction) {
    if (!simulation_) {
        Adapter *adapter = static_cast<Adapter *>(SIM_object_data(simics_obj_));
        simulation_ = adapter;
        assert(simulation_);
    }

    if (!internal_) {
        Adapter *adapter = static_cast<Adapter *>(SIM_object_data(simics_obj_));
        internal_ = adapter->internal();
        assert(internal_);
    }

    if (!transaction->extension()->status().invalid()) {
        SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                         "A new transaction was triggered while a previous one"
                         " was already in progress.");
        sc_core::sc_stop();
        return false;
    }

    if (sc_core::sc_get_status() == sc_core::SC_STOPPED) {
        SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                         "SystemC kernel has been stopped,"
                         " transactions are no longer possible");
        return false;
    }

    // Use DMI ptr if allowed
    // Note: If read or write access to a certain region of memory causes
    // side effects in a target (that is, causes some other change to the
    // state of the target over and above the state of the memory), the
    // target should not grant DMI access of the given type to that memory
    // region.
    unsigned char *dmi_p = NULL;
    if (dmi_ && (*transaction)->get_command() != tlm::TLM_IGNORE_COMMAND) {
        dmi_p = dmi_data_table_.dmiPointer((*transaction)->get_address(),
                                           (*transaction)->get_data_length(),
                                           (*transaction)->is_read());
    }

    iface::TransactionExtension::Callback *callback = get_callback(transaction);
    if (dmi_p) {
        if ((*transaction)->is_read()) {
            memcpy((*transaction)->get_data_ptr(), dmi_p,
                   (*transaction)->get_data_length());
        } else {
            memcpy(dmi_p, (*transaction)->get_data_ptr(),
                   (*transaction)->get_data_length());
        }

        if (callback)
            callback->complete(transaction, true);

        return true;
    }

    transaction->extension()->set_status(iface::TransactionStatus::ACTIVE);

    sc_core::sc_curr_proc_kind kind =
        sc_core::sc_get_current_process_handle().proc_kind();

    if (sc_core::sc_get_status() == sc_core::SC_RUNNING
        && (kind == sc_core::SC_THREAD_PROC_
            || kind == sc_core::SC_CTHREAD_PROC_)) {
        // already on a running thread, don't need a new one and spec
        // prohibits us from calling sc_start twice. Re-use the current
        // thread.
        //
        // If an asynchronous transaction re-enters, we end up here and
        // handle the re-entry as synchronous.
        send_transaction(transaction);
    } else if (transaction->extension()->transport_debug()) {
        // transport_dbg does not need a thread, just call the method
        send_transaction(transaction);
    } else {
        // Here we assume that SystemC is _not_ running, because we intend
        // to call sc_start() to handle the TLM transaction in our
        // SystemC thread. Specification prohibits calling sc_start
        // twice.
        if (sc_core::sc_get_status() != sc_core::SC_PAUSED) {
            sc_core::sc_process_handle handle
                = sc_core::sc_get_current_process_handle();
            SIM_LOG_ERROR(simics_obj_, Log_TLM,
                          "TLM2 spec-violation: Attempting to call b_transport"
                          " from a non-thread process."
                          " Current process: %s (%s) @ %s",
                          handle.name(),
                          ((handle.proc_kind() == sc_core::SC_METHOD_PROC_)
                           ? "SC_METHOD_PROC" : "SC_NO_PROC"),
                          handle.get_parent_object()->name());
            // Call it anyway, and hope for the best. The issue has been
            // logged as an error and failure to call wait from a non-thread
            // process will be caught by the kernel
            send_transaction(transaction);
        } else {
            // The transaction is passed to the SystemC thread during do_work.
            transaction_ = transaction;
            ThreadInterface::CallReturn r = thread_pool_.call(this);
            transaction_ = NULL;
            assert(r != ThreadInterface::CALL_RETURN_ERROR_IN_USE);

            if (callback && callback->is_async(transaction))
                return true;

            while (transaction->extension()->status().active()
                   && sc_core::sc_pending_activity()) {
                if (!simulation_->runSimulation(sc_core::sc_max_time()))
                    break;
            }
        }
    }

    // Transaction status is overwritten after reset
    bool success = transaction->extension()->status().ok();
    transaction->extension()->reset();
    internal_->process_stack()->waitForExitCondition(kind);
    return success;
}

template<unsigned int BUSWIDTH, typename TYPES>
simics::ConfObjectRef &Gasket<BUSWIDTH, TYPES>::simics_obj() {
    return simics_obj_;
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::set_inquiry(bool inquiry) {
    deprecated_transaction_.extension()->set_transport_debug(inquiry);
}

template<unsigned int BUSWIDTH, typename TYPES>
DmiDataTable *Gasket<BUSWIDTH, TYPES>::get_dmi_data_table() {
    return &dmi_data_table_;
}

template<unsigned int BUSWIDTH, typename TYPES>
sc_core::sc_object *Gasket<BUSWIDTH, TYPES>::get_target_socket() {
    return target_socket_;
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::set_type(ClassType *type) {
    type_ = type;
}

template<unsigned int BUSWIDTH, typename TYPES>
ClassType *Gasket<BUSWIDTH, TYPES>::type() {
    return type_;
}

template<unsigned int BUS_WIDTH, typename TYPES>
void Gasket<BUS_WIDTH, TYPES>::set_dmi(bool enable) {
    dmi_ = enable;
}

template<unsigned int BUS_WIDTH, typename TYPES>
bool Gasket<BUS_WIDTH, TYPES>::is_dmi_enabled() {
    return dmi_;
}

template<unsigned int BUS_WIDTH, typename TYPES>
std::string Gasket<BUS_WIDTH, TYPES>::gasket_name() const {
    return name();
}

// The start_range and end_range arguments shall be the first and last
// addresses of the address range for which DMI access is to be invalidated
template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::
invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                          sc_dt::uint64 end_range) {
    dmi_data_table_.remove(start_range, end_range);
}

template<unsigned int BUSWIDTH, typename TYPES>
iface::TransactionExtension::Callback *Gasket<BUSWIDTH, TYPES>::get_callback(
        iface::Transaction *transaction) {
    if (!transaction)
        return NULL;

    return transaction->extension()->callback();
}

static void emptyHandler(const sc_core::sc_report& report,
                         const sc_core::sc_actions& actions) {
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::send_transaction(
        iface::Transaction *transaction) {
    sc_core::sc_report_handler_proc handler = NULL;
    // The address and extension attributes may be modified
    // by the target, save a copy for DMI usage
    iface::Transaction transaction_copy;
    try {
        if (dmi_) {
            transaction_copy = pool_.acquire();
            transaction_copy->deep_copy_from(*transaction);
        }
        if (transaction->extension()->transport_debug()) {
            initiator_socket_->transport_dbg(*transaction);
        } else {
            sc_core::sc_time local_time = sc_core::SC_ZERO_TIME;
            initiator_socket_->b_transport(*transaction, local_time);
            // Before we return the result to simics we need to synchronize
            // local time.
            if (local_time > sc_core::SC_ZERO_TIME) {
#ifndef RISC_SIMICS
                wait(local_time);
#endif
            }
        }
        if (dmi_) {
            update_dmi_data_table(transaction, transaction_copy.payload());
        }
    } catch(const sc_core::sc_halt &e) {
        SIM_LOG_CRITICAL(simics_obj(), Log_TLM,
                         "sc_halt exception caught in send_transaction()");
        simulation_->stopSimulation();
    } catch(const sc_core::sc_user &e) {
        SIM_LOG_CRITICAL(simics_obj(), Log_TLM,
                         "sc_user exception caught in send_transaction()");
        simulation_->stopSimulation();
    } catch(const sc_core::sc_unwind_exception &e) {
        // Replace custom report handler with empty handler to avoid indirect
        // throw from sc_unwind_exception DTOR
        handler = sc_core::sc_report_handler::get_handler();
        sc_core::sc_report_handler::set_handler(emptyHandler);
        SIM_LOG_CRITICAL(simics_obj_, Log_TLM,
                         "exception caught in send_transaction(): %s",
                         e.what());
        simulation_->stopSimulation();
        // avoid (false positive) warning from GCC about unused function
        emptyHandler(*sc_core::sc_report_handler::get_cached_report(),
                     sc_core::SC_STOP);
    } catch(...) {
        SIM_LOG_CRITICAL(simics_obj(), Log_TLM,
                         "unknown exception caught in send_transaction()");
        simulation_->stopSimulation();
    }
    if (handler)
        sc_core::sc_report_handler::set_handler(handler);

    bool success = (*transaction)->is_response_ok();
    transaction->extension()->set_status(
            success ? iface::TransactionStatus::OK
                    : iface::TransactionStatus::FAIL);

    log_failed_transaction(transaction);
    iface::TransactionExtension::Callback *callback = get_callback(transaction);
    if (callback)
        callback->complete(transaction, success);
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::update_dmi_data_table(
        iface::Transaction *transaction,
        tlm::tlm_generic_payload *payload_before_send) {
    // Check DMI hint response
    if ((*transaction)->is_dmi_allowed()) {
        tlm::tlm_dmi dmi;
        // get_direct_mem_ptr shall not call wait
        if (initiator_socket_->get_direct_mem_ptr(*payload_before_send, dmi)) {
            SIM_LOG_INFO(4, simics_obj_, Log_TLM,
                         "DMI descriptor received for 0x%x-0x%x (type:%s%s)",
                         static_cast<unsigned int>(dmi.get_start_address()),
                         static_cast<unsigned int>(dmi.get_end_address()),
                         dmi.is_read_allowed() ? "R":"",
                         dmi.is_write_allowed() ? "W":"");

            // Add one more item to DMI data table
            dmi_data_table_.add(dmi);
        }
    }
}

template<unsigned int BUSWIDTH, typename TYPES>
void Gasket<BUSWIDTH, TYPES>::log_failed_transaction(
        iface::Transaction *transaction) {
    if (!transaction->extension()->status().ok()) {
        if (transaction->extension()->status().active()) {
            SIM_LOG_ERROR(simics_obj_, Log_TLM,
                          "transaction did not complete as expected");
        } else {
            SIM_LOG_INFO(4, simics_obj(), Log_TLM, "transaction failed");
        }
    }
}

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics
