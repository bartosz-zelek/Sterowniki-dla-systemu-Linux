// -*- mode: C++; c-file-style: "virtutech-c++" -*-

/*
  © 2020 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef SIMICS_SYSTEMC_TLM2SIMICS_GASKET_DISPATCHER_H
#define SIMICS_SYSTEMC_TLM2SIMICS_GASKET_DISPATCHER_H


#include <simics/systemc/tlm2simics/gasket.h>

#include <tlm>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <map>
#include <utility>
#include <vector>

namespace simics {
namespace systemc {
namespace tlm2simics {

class GasketDispatcherBase {
  public:
    GasketDispatcherBase() {
        context_.insert(
                std::make_pair(sc_core::sc_get_curr_simcontext(), this));
    }
    virtual ~GasketDispatcherBase() {}
    static void cleanCache() {
        auto range = context_.equal_range(sc_core::sc_get_curr_simcontext());
        for (auto it = range.first; it != range.second; ++it)
            delete it->second;

        context_.erase(sc_core::sc_get_curr_simcontext());
    }

  private:
    static std::multimap<sc_core::sc_simcontext *,
                         GasketDispatcherBase *> context_;
};

template <unsigned int BUSWIDTH = 32,
          typename TYPES = tlm::tlm_base_protocol_types,
          int N = 1,
          sc_core::sc_port_policy POL = sc_core::SC_ONE_OR_MORE_BOUND>
class GasketDispatcher
    : public sc_core::sc_module, public GasketDispatcherBase {
  public:
    SC_CTOR(GasketDispatcher) {
        target_socket_.register_nb_transport_fw(
                this, &GasketDispatcher::nb_transport_fw);
        target_socket_.register_b_transport(
                this, &GasketDispatcher::b_transport);
        target_socket_.register_transport_dbg(
                this, &GasketDispatcher::transport_dbg);
        target_socket_.register_get_direct_mem_ptr(
                this, &GasketDispatcher::get_direct_mem_ptr);
        multi_initiator_.register_invalidate_direct_mem_ptr(
                this, &GasketDispatcher::invalidate_direct_mem_ptr);
        multi_initiator_.register_nb_transport_bw(
                this, &GasketDispatcher::nb_transport_bw);
    }
    static GasketInterface::Ptr bind(
            tlm::tlm_initiator_socket<BUSWIDTH, TYPES, N, POL> *socket,
            const char *name,
            const simics::ConfObjectRef &simics_obj) {
        GasketDispatcher<BUSWIDTH, TYPES, N, POL> *dispatcher = NULL;
        if (dispatchers_.find(socket) == dispatchers_.end()) {
            dispatcher = new GasketDispatcher<BUSWIDTH, TYPES, N, POL>(
                    sc_core::sc_gen_unique_name("GasketDispatcher"));
            dispatchers_[socket] = dispatcher;
            (*socket)(dispatcher->target_socket_);
            dispatcher->simics_obj_ = simics_obj;
        }

        dispatcher = dispatchers_[socket];
        assert(dispatcher);
        Gasket<BUSWIDTH, TYPES> *gasket = new Gasket<BUSWIDTH, TYPES>(
                name, simics_obj);

        dispatcher->receivers_.push_back(
                std::make_pair(dispatcher->receivers_.size(),
                               gasket));
        gasket->bind(dispatcher->multi_initiator_);
        return GasketInterface::Ptr(gasket);
    }
    virtual ~GasketDispatcher() {
        for (auto it = dispatchers_.begin(); it != dispatchers_.end(); ++it) {
            if (it->second == this) {
                dispatchers_.erase(it);
                return;
            }
        }
    }

  private:
    tlm::tlm_sync_enum nb_transport_fw(
            tlm::tlm_generic_payload &trans, tlm::tlm_phase &phase,  // NOLINT
            sc_core::sc_time &local_time) {  // NOLINT
        int idx = index(&trans);
        if (idx >= 0)
            return multi_initiator_[idx]->nb_transport_fw(trans, phase,
                                                          local_time);

        return tlm::TLM_COMPLETED;
    }
    tlm::tlm_sync_enum nb_transport_bw(
            int idx, tlm::tlm_generic_payload &trans,  // NOLINT
            tlm::tlm_phase &phase, sc_core::sc_time &local_time) {  // NOLINT
        return target_socket_->nb_transport_bw(trans, phase, local_time);
    }
    void b_transport(
            tlm::tlm_generic_payload &trans, sc_core::sc_time &local_time) {  // NOLINT
        int idx = index(&trans);
        if (idx >= 0)
            multi_initiator_[idx]->b_transport(trans, local_time);
    }
    unsigned int transport_dbg(tlm::tlm_generic_payload &trans) {  // NOLINT
        int idx = index(&trans);
        if (idx >= 0)
            return multi_initiator_[idx]->transport_dbg(trans);

        return 0;
    }
    void invalidate_direct_mem_ptr(int idx, sc_dt::uint64 start_range,
                                   sc_dt::uint64 end_range) {
        target_socket_->invalidate_direct_mem_ptr(start_range, end_range);
    }

    bool get_direct_mem_ptr(
            tlm::tlm_generic_payload &trans, tlm::tlm_dmi &dmi) {  // NOLINT
        int idx = index(&trans);
        if (idx >= 0)
            return multi_initiator_[idx]->get_direct_mem_ptr(trans, dmi);

        return false;
    }

    int index(tlm::tlm_generic_payload *trans) {
        if ((sc_core::SC_ELABORATION |
             sc_core::SC_BEFORE_END_OF_ELABORATION |
             sc_core::SC_END_OF_ELABORATION) & sc_core::sc_get_status()) {
            SIM_LOG_ERROR(simics_obj_, 0, "b_transport called before "
                          "completion of SystemC SC_END_OF_ELABORATION");
            return -1;
        }

        if (receivers_.size() <= 0) {
            SIM_LOG_ERROR(simics_obj_, 0, "b_transport called without "
                          "bound socket");
            return -1;
        }

        if (receivers_.size() > 1) {
            for (auto it = receivers_.begin(); it != receivers_.end(); ++it) {
                TransactionHandlerInterface *handler =
                        it->second->transaction_handler();
                simics::systemc::iface::ReceiverInterface *receiver =
                        handler->receiver();
                if (receiver && receiver->probe(trans)) {
                    return it->first;
                }
            }
        }

        // Use first gasket as default for transactions without extensions
        return 0;
    }

    static std::map<sc_core::sc_object*,
                     GasketDispatcher<BUSWIDTH, TYPES, N, POL> *> dispatchers_;

    std::vector<std::pair<size_t, GasketInterface *> > receivers_;
    tlm_utils::multi_passthrough_initiator_socket<
        GasketDispatcher,
        BUSWIDTH,
        TYPES> multi_initiator_;
    tlm_utils::simple_target_socket<
        GasketDispatcher<BUSWIDTH, TYPES, N, POL>,
        BUSWIDTH, TYPES> target_socket_;
    simics::ConfObjectRef simics_obj_;
};

template <unsigned int BUSWIDTH,
          typename TYPES,
          int N,
          sc_core::sc_port_policy POL>
std::map<sc_core::sc_object*,
         GasketDispatcher<BUSWIDTH, TYPES, N, POL> *>
GasketDispatcher<BUSWIDTH, TYPES, N, POL>::dispatchers_;

}  // namespace tlm2simics
}  // namespace systemc
}  // namespace simics

#endif
