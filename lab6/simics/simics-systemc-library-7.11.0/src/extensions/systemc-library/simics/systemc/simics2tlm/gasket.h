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

#ifndef SIMICS_SYSTEMC_SIMICS2TLM_GASKET_H
#define SIMICS_SYSTEMC_SIMICS2TLM_GASKET_H

#include <simics/cc-api.h>
#include <simics/systemc/iface/transaction.h>
#include <simics/systemc/iface/transaction_extension.h>
#include <simics/systemc/iface/transaction_pool.h>
#include <simics/systemc/iface/simulation_interface.h>
#include <simics/systemc/internal_interface.h>
#include <simics/systemc/simics2tlm/gasket_interface.h>
#include <simics/systemc/simics2tlm/dmi_data_table.h>
#include <simics/systemc/thread_pool.h>

#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>

#include <string>

namespace simics {
namespace systemc {
namespace simics2tlm {

/**
 * Implements core functionality for sending a TLM2 transaction over a
 * socket. This class is used by all simics2tlm gaskets through the GasketOwner
 * or MultiGasketOwner base classes.
 */
template<unsigned int BUSWIDTH = 32,
         typename TYPES = tlm::tlm_base_protocol_types>
class Gasket : public sc_core::sc_module,
               public GasketInterface,
               public ThreadCallbackInterface {
  public:
    Gasket(sc_core::sc_module_name, const ConfObjectRef &obj);
    virtual ~Gasket() = default;

    void init(iface::SimulationInterface *simulation,
              InternalInterface *internal);

    // GasketInterface
    bool trigger(iface::Transaction *transaction) override;
    ConfObjectRef &simics_obj() override;
    DmiDataTable *get_dmi_data_table() override;
    void set_type(ClassType *type) override;
    ClassType *type() override;
    sc_core::sc_object *get_target_socket() override;
    void set_dmi(bool enable) override;
    bool is_dmi_enabled() override;
    std::string gasket_name() const override;
    /** Deprecated, use the TransactionPool::acquire() instead. */
    tlm::tlm_generic_payload &payload() override;
    /** Deprecated, use the trigger(iface::Transaction *transaction) instead. */
    bool trigger_transaction() override;
    /** Deprecated, use the TransactionExtension::set_transport_debug(bool)
        instead. */
    void set_inquiry(bool inquiry) override;

    template <typename Socket>
    void bind(Socket &sock) {  // NOLINT
        initiator_socket_.bind(sock);
        target_socket_ = &sock;
    }

  private:
    void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range);
    iface::TransactionExtension::Callback *get_callback(
            iface::Transaction *transaction);
    void send_transaction(iface::Transaction *transaction);
    void update_dmi_data_table(iface::Transaction *transaction,
                               tlm::tlm_generic_payload *payload_before_send);
    void log_failed_transaction(iface::Transaction *transaction);

    // ThreadCallbackInterface
    void run(ThreadInterface *call) override;
    void block(ThreadInterface *call) override;
    void finish(ThreadInterface *call) override;
    void exception(ThreadInterface *call) override;
    iface::SimulationInterface *simulation(ThreadInterface *call) override;

    class ThreadData : public Thread {
      public:
        ThreadData()
            : transaction_(NULL), must_call_sc_pause_(false),
              must_reset_extension_(false),
              simulation_(NULL), internal_(NULL) {}
        void init(iface::Transaction *transaction,
                  iface::SimulationInterface *simulation,
                  InternalInterface *internal) {
            transaction_ = *transaction;
            must_call_sc_pause_ = false;
            must_reset_extension_ = false;
            simulation_ = simulation;
            internal_ = internal;
        }
        const char *thread_name() override {
            return sc_core::sc_gen_unique_name("on_transaction");
        }

        iface::Transaction transaction_;
        bool must_call_sc_pause_;
        bool must_reset_extension_;
        iface::SimulationInterface *simulation_;
        InternalInterface *internal_;
    };

    ConfObjectRef simics_obj_;
    iface::Transaction *transaction_;
    ClassType *type_;
    sc_core::sc_object* target_socket_;
    tlm_utils::simple_initiator_socket<Gasket, BUSWIDTH, TYPES>
        initiator_socket_;
    DmiDataTable dmi_data_table_;
    bool dmi_;
    ThreadPool<ThreadData> thread_pool_;
    iface::SimulationInterface *simulation_;
    InternalInterface *internal_;
    iface::TransactionPool pool_;

    // deprecated
    tlm::tlm_generic_payload deprecated_payload_;
    iface::Transaction deprecated_transaction_;
};

}  // namespace simics2tlm
}  // namespace systemc
}  // namespace simics

#include "gasket.cc"  // necessary for class templates to work NOLINT(build/include)

#endif
